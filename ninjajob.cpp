/* This file is part of KDevelop
    Copyright 2012 Aleix Pol Gonzalez <aleixpol@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ninjajob.h"
#include <KProcess>
#include <KUrl>
#include <KDebug>
#include <KLocalizedString>
#include <KConfigGroup>
#include <interfaces/iproject.h>
#include <outputview/outputmodel.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/projectmodel.h>
#include <interfaces/iproject.h>
#include <QFile>

NinjaJob::NinjaJob(KDevelop::ProjectBaseItem* item, const QStringList& arguments, QObject* parent)
    : OutputExecuteJob(parent)
    , m_lastLine(false)
    , m_isInstalling(false)
    , m_item(item)
{
    setToolTitle(i18n("Ninja"));
    setCapabilities(Killable);
    setStandardToolView( KDevelop::IOutputView::BuildView );
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll );
    setFilteringStrategy( KDevelop::OutputModel::CompilerFilter );
    setProperties( NeedWorkingDirectory | PortableMessages | DisplayStderr | IsBuilderHint | PostProcessOutput );

    *this << "ninja";
    *this << arguments;

    QStringList targets;
    foreach( const QString& arg, arguments ) {
        if( !arg.startsWith( '-' ) ) {
            targets << arg;
        }
    }
    QString title;
    if( !targets.isEmpty() )
        title = i18n("Ninja (%1): %2", m_item->text(), targets.join(" "));
    else
        title = i18n("Ninja (%1)", m_item->text());
    setJobName( title );
}

void NinjaJob::setIsInstalling( bool isInstalling )
{
    m_isInstalling = isInstalling;
}

KUrl NinjaJob::workingDirectory() const
{
    KUrl workingDir = m_item->project()->buildSystemManager()->buildDirectory( m_item );
    while( !QFile::exists( workingDir.toLocalFile( KUrl::AddTrailingSlash ) + "build.ninja" ) ) {
        KUrl upWorkingDir = workingDir.upUrl();
        if( upWorkingDir.isEmpty() || upWorkingDir == workingDir ) {
            return m_item->project()->buildSystemManager()->buildDirectory( m_item->project()->projectItem() );
        }
        workingDir = upWorkingDir;
    }
    return workingDir;
}

QStringList NinjaJob::privilegedExecutionCommand() const
{
    KSharedConfig::Ptr configPtr = m_item->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "NinjaBuilder" );

    bool runAsRoot = builderGroup.readEntry( "Install As Root", false );
    if ( runAsRoot && m_isInstalling )
    {
        int suCommand = builderGroup.readEntry( "Su Command", 0 );
        QStringList arguments;
        QString suCommandName;
        switch( suCommand ) {
            case 1:
                return QStringList() << "kdesudo" << "-t";

            case 2:
                return QStringList() << "sudo";

            default:
                return QStringList() << "kdesu" << "-t";
        }
    }
    return QStringList();
}

void NinjaJob::signalWhenFinished(const QByteArray& signal, KDevelop::ProjectBaseItem* item)
{
    m_signal = signal;
    m_item = item;
    connect(this, SIGNAL(finished(KJob*)), SLOT(emitProjectBuilderSignal(KJob*)));
}

void NinjaJob::emitProjectBuilderSignal(KJob* job)
{
    Q_ASSERT(!m_signal.isEmpty());
    if(job->error()==0)
        QMetaObject::invokeMethod(parent(), m_signal, Q_ARG(KDevelop::ProjectBaseItem*, m_item));
    else
        QMetaObject::invokeMethod(parent(), "failed", Q_ARG(KDevelop::ProjectBaseItem*, m_item));
}

void NinjaJob::postProcessStderr( const QStringList& lines )
{
    appendLines( lines );
}

void NinjaJob::postProcessStdout( const QStringList& lines )
{
    appendLines( lines );
}

void NinjaJob::appendLines(const QStringList& lines)
{
    if(lines.isEmpty())
        return;
    
    QStringList ret(lines);
    bool prev = false;
    for(QStringList::iterator it=ret.end(); it!=ret.begin(); ) {
        --it;
        bool curr = it->startsWith('[');
        if(prev && curr)
            it = ret.erase(it);
        prev = curr;
    }
    ///NOTE: Find a better way to achieve this without API changes in KDevplatform.
    /*
    if(m_lastLine && ret.first().startsWith('['))
        model()->removeLastLines(1);
    m_lastLine = ret.last().startsWith('[');
    */
    model()->appendLines(ret);
}
