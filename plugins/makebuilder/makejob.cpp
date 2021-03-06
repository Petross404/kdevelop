/* This file is part of KDevelop
    Copyright 2004 Roberto Raggi <roberto@kdevelop.org>
    Copyright 2007 Andreas Pakulat <apaku@gmx.de>
    Copyright 2007 Dukju Ahn <dukjuahn@gmail.com>
    Copyright 2008 Hamish Rodda <rodda@kde.org>
    Copyright 2012 Ivan Shapovalov <intelfx100@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "makejob.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QThread>

#include <KShell>
#include <KConfigGroup>
#include <KLocalizedString>

#include <interfaces/iproject.h>
#include <interfaces/icore.h>
#include <interfaces/iprojectcontroller.h>
#include <project/projectmodel.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <outputview/outputfilteringstrategies.h>

#include "makebuilder.h"
#include "makebuilderpreferences.h"
#include "debug.h"

using namespace KDevelop;

class MakeJobCompilerFilterStrategy : public CompilerFilterStrategy
{
public:
    using CompilerFilterStrategy::CompilerFilterStrategy;

    IFilterStrategy::Progress progressInLine(const QString& line) override;
};

IFilterStrategy::Progress MakeJobCompilerFilterStrategy::progressInLine(const QString& line)
{
    // example string: [ 97%] Built target clang-parser
    static const QRegularExpression re(QStringLiteral("^\\[([\\d ][\\d ]\\d)%\\] (.*)"));

    QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch()) {
        bool ok;
        const int percent = match.capturedRef(1).toInt(&ok);
        if (ok) {
            // this is output from make, likely
            const QString action = match.captured(2);
            return {action, percent};
        }
    }

    return {};
}

MakeJob::MakeJob(QObject* parent, KDevelop::ProjectBaseItem* item,
                 CommandType c,  const QStringList& overrideTargets,
                 const MakeVariables& variables )
    : OutputExecuteJob(parent)
    , m_idx(item->index())
    , m_command(c)
    , m_overrideTargets(overrideTargets)
    , m_variables(variables)
{
    auto bsm = item->project()->buildSystemManager();
    auto buildDir = bsm->buildDirectory(item);

    Q_ASSERT(item && item->model() && m_idx.isValid() && this->item() == item);
    setCapabilities( Killable );
    setFilteringStrategy(new MakeJobCompilerFilterStrategy(buildDir.toUrl()));
    setProperties( NeedWorkingDirectory | PortableMessages | DisplayStderr | IsBuilderHint );

    QString title;
    if( !m_overrideTargets.isEmpty() )
        title = i18n("Make (%1): %2", item->text(), m_overrideTargets.join(QLatin1Char(' ')));
    else
        title = i18n("Make (%1)", item->text());
    setJobName( title );
    setToolTitle( i18n("Make") );
}

MakeJob::~MakeJob()
{
}

void MakeJob::start()
{
    ProjectBaseItem* it = item();
    qCDebug(KDEV_MAKEBUILDER) << "Building with make" << m_command << m_overrideTargets.join(QLatin1Char(' '));
    if (!it)
    {
        setError(ItemNoLongerValidError);
        setErrorText(i18n("Build item no longer available"));
        emitResult();
        return;
    }

    if( it->type() == KDevelop::ProjectBaseItem::File ) {
        setError(IncorrectItemError);
        setErrorText(i18n("Internal error: cannot build a file item"));
        emitResult();
        return;
    }

    setStandardToolView(IOutputView::BuildView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll);

    OutputExecuteJob::start();
}

KDevelop::ProjectBaseItem * MakeJob::item() const
{
    return ICore::self()->projectController()->projectModel()->itemFromIndex(m_idx);
}

MakeJob::CommandType MakeJob::commandType() const
{
    return m_command;
}

QStringList MakeJob::customTargets() const
{
    return m_overrideTargets;
}

QUrl MakeJob::workingDirectory() const
{
    ProjectBaseItem* it = item();
    if(!it)
        return QUrl();

    KDevelop::IBuildSystemManager *bldMan = it->project()->buildSystemManager();
    if( bldMan )
        return bldMan->buildDirectory( it ).toUrl(); // the correct build dir
    else
    {
        // Just build in-source, where the build directory equals the one with particular target/source.
        for( ProjectBaseItem* item = it; item; item = item->parent() ) {
            switch( item->type() ) {
            case KDevelop::ProjectBaseItem::Folder:
            case KDevelop::ProjectBaseItem::BuildFolder:
                return static_cast<KDevelop::ProjectFolderItem*>(item)->path().toUrl();
            case KDevelop::ProjectBaseItem::Target:
            case KDevelop::ProjectBaseItem::File:
                break;
            }
        }
        return QUrl();
    }
}

QStringList MakeJob::privilegedExecutionCommand() const
{
    ProjectBaseItem* it = item();
    if(!it)
        return QStringList();
    KSharedConfigPtr configPtr = it->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );

    bool runAsRoot = builderGroup.readEntry( "Install As Root", false );
    if ( runAsRoot && m_command == InstallCommand )
    {
        QString suCommand = builderGroup.readEntry( "Su Command", QString() );
        bool suCommandIsDigit;
        QStringList suCommandWithArg;
        int suCommandNum = suCommand.toInt(&suCommandIsDigit);

        /*
         * "if(suCommandIsDigit)" block exists only because of backwards compatibility
         * reasons, In earlier versions of KDevelop, suCommand's type was
         * int, if a user upgrades to current version from an older version,
         * suCommandIsDigit will become "true" and we will set suCommand according
         * to the stored config entry.
         */
        if(suCommandIsDigit)
        {
            switch(suCommandNum)
            {
                case 1:
                {
                    suCommand = QStringLiteral("kdesudo");
                    break;
                }
                case 2:
                {
                    suCommand = QStringLiteral("sudo");
                    break;
                }
                default:
                    suCommand = QStringLiteral("kdesu");
            }

            builderGroup.writeEntry("Su Command", suCommand);
            //so that suCommandIsDigit becomes false next time
            //project is opened.
        }

        suCommandWithArg = KShell::splitArgs(suCommand);
        if( suCommandWithArg.isEmpty() )
        {
            suCommandWithArg = QStringList{QStringLiteral("kdesu"), QStringLiteral("-t")};
        }

        return suCommandWithArg;
    }
    return QStringList();
}

QStringList MakeJob::commandLine() const
{
    ProjectBaseItem* it = item();
    if(!it)
        return QStringList();
    QStringList cmdline;

    KSharedConfigPtr configPtr = it->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );

    // TODO: migrate to more generic key term "Make Executable"
    QString makeBin = builderGroup.readEntry("Make Binary", MakeBuilderPreferences::standardMakeExecutable());
    cmdline << makeBin;

    if( ! builderGroup.readEntry("Abort on First Error", true))
    {
        cmdline << (isNMake(makeBin) ? QStringLiteral("/K") : QStringLiteral("-k"));
    }

    // note: nmake does not support the -j flag
    if (!isNMake(makeBin)) {
        if (builderGroup.readEntry("Override Number Of Jobs", false)) {
            int jobCount = builderGroup.readEntry("Number Of Jobs", 1);
            if (jobCount > 0) {
                cmdline << QStringLiteral("-j%1").arg(jobCount);
            }
        } else {
            // use the ideal thread count by default
            cmdline << QStringLiteral("-j%1").arg(QThread::idealThreadCount());
        }
    }

    if( builderGroup.readEntry("Display Only", false) )
    {
        cmdline << (isNMake(makeBin) ? QStringLiteral("/N") : QStringLiteral("-n"));
    }

    QString extraOptions = builderGroup.readEntry("Additional Options", QString());
    if( ! extraOptions.isEmpty() )
    {
        foreach(const QString& option, KShell::splitArgs( extraOptions ) )
            cmdline << option;
    }

    for (MakeVariables::const_iterator it = m_variables.constBegin(); it != m_variables.constEnd(); ++it)
    {
        cmdline += QStringLiteral("%1=%2").arg(it->first, it->second);
    }

    if( m_overrideTargets.isEmpty() )
    {
        QString target;
        switch (it->type()) {
            case KDevelop::ProjectBaseItem::Target:
            case KDevelop::ProjectBaseItem::ExecutableTarget:
            case KDevelop::ProjectBaseItem::LibraryTarget:
                Q_ASSERT(it->target());
                cmdline << it->target()->text();
                break;
            case KDevelop::ProjectBaseItem::BuildFolder:
                target = builderGroup.readEntry("Default Target", QString());
                if( !target.isEmpty() )
                    cmdline << target;
                break;
            default: break;
        }
    }else
    {
        cmdline += m_overrideTargets;
    }

    return cmdline;
}

QString MakeJob::environmentProfile() const
{
    ProjectBaseItem* it = item();
    if(!it)
        return QString();
    KSharedConfigPtr configPtr = it->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );
    return builderGroup.readEntry( "Default Make Environment Profile", QString() );
}

bool MakeJob::isNMake(const QString& makeBin)
{
    return !QFileInfo(makeBin).baseName().compare(QStringLiteral("nmake"), Qt::CaseInsensitive);
}
