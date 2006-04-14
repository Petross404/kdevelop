/***************************************************************************
 *   Copyright (C) 2002 by Yann Hodique                                    *
 *   Yann.Hodique@lifl.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <kurl.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcharsets.h>

#include "generalinfowidget.h"
#include "generalinfowidget.moc"
#include "domutil.h"
#include "projectmanager.h"

QString makeRelativePath(const QString& fromPath, const QString& toPath);

GeneralInfoWidget::GeneralInfoWidget(QDomDocument &projectDom, QWidget *parent, const char *name)
        : GeneralInfoWidgetBase(parent, name), m_projectDom(projectDom) {

    connect(project_directory_edit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotProjectDirectoryChanged(const QString&)));
    connect(project_directory_combo, SIGNAL(activated(int)),
            this, SLOT(slotProjectDirectoryComboChanged()));
    readConfig();
}



GeneralInfoWidget::~GeneralInfoWidget() {}

void GeneralInfoWidget::readConfig() {
    if(DomUtil::readBoolEntry(m_projectDom,"/general/absoluteprojectpath",false))
        this->project_directory_combo->setCurrentItem(0);
    else
        this->project_directory_combo->setCurrentItem(1);
    this->project_directory_edit->setText(DomUtil::readEntry(m_projectDom,"/general/projectdirectory","."));
    this->author_edit->setText(DomUtil::readEntry(m_projectDom,"/general/author"));
    this->email_edit->setText(DomUtil::readEntry(m_projectDom,"/general/email"));
    this->version_edit->setText(DomUtil::readEntry(m_projectDom,"/general/version"));
    this->description_edit->setText(DomUtil::readEntry(m_projectDom,"/general/description"));

	QStringList encodings;
	encodings << i18n("Use global editor settings");
	encodings += KGlobal::charsets()->descriptiveEncodingNames();
	QStringList::const_iterator it = encodings.constBegin();
	while ( it != encodings.constEnd() )
	{
		encoding_combo->insertItem( *it );
		++it;
	}
	encoding_combo->setCurrentItem( 0 );

//	const QString DefaultEncoding = KGlobal::charsets()->encodingForName( DomUtil::readEntry( m_projectDom, "/general/defaultencoding", QString::null ) );
	const QString DefaultEncoding = DomUtil::readEntry( m_projectDom, "/general/defaultencoding", QString::null );
	for ( int i = 0; i < encoding_combo->count(); i++ )
	{
		if ( KGlobal::charsets()->encodingForName( encoding_combo->text( i ) ) == DefaultEncoding )
		{
			encoding_combo->setCurrentItem( i );
			break;
		}
	}

}

void GeneralInfoWidget::writeConfig() {
    DomUtil::writeEntry(m_projectDom,"/general/projectdirectory",project_directory_edit->text());
    DomUtil::writeBoolEntry(m_projectDom,"/general/absoluteprojectpath",isProjectDirectoryAbsolute());
    DomUtil::writeEntry(m_projectDom,"/general/email",email_edit->text());
    DomUtil::writeEntry(m_projectDom,"/general/author",author_edit->text());
    DomUtil::writeEntry(m_projectDom,"/general/email",email_edit->text());
    DomUtil::writeEntry(m_projectDom,"/general/version",version_edit->text());
    DomUtil::writeEntry(m_projectDom,"/general/description",description_edit->text());

	QString DefaultEncoding = QString::null;
	if ( encoding_combo->currentItem() > 0 )
	{
		DefaultEncoding = KGlobal::charsets()->encodingForName( encoding_combo->currentText() );
	}
	DomUtil::writeEntry( m_projectDom, "/general/defaultencoding", DefaultEncoding );
}

void GeneralInfoWidget::accept() {
    writeConfig();
}

bool GeneralInfoWidget::isProjectDirectoryAbsolute() {
    return project_directory_combo->currentItem() == 0;
}

QString GeneralInfoWidget::projectDirectory() {
    return ProjectManager::projectDirectory( project_directory_edit->text(), isProjectDirectoryAbsolute() );
}

void GeneralInfoWidget::slotProjectDirectoryChanged( const QString& text ) {
    if(text.isEmpty())
    {
       setProjectDirectoryError(i18n("Please enter a path."));
    }
    else if(isProjectDirectoryAbsolute() && text[0] != '/')
    {
       setProjectDirectoryError(
           i18n("'%1' is not an absolute path.").arg(
               project_directory_edit->text()));
    }
    else if(!isProjectDirectoryAbsolute() && text[0] == '/')
    {
       setProjectDirectoryError(
           i18n("'%1' is not a relative path.").arg(
               project_directory_edit->text()));
    }
    else
    {
        QFileInfo info(projectDirectory());
        if(!info.exists())
           setProjectDirectoryError(
               i18n("'%1' does not exist.").arg(
                   project_directory_edit->text()));
        else if(!info.isDir())
           setProjectDirectoryError(
               i18n("'%1' is not a directory.").arg(
                   project_directory_edit->text()));
        else
           setProjectDirectorySuccess();
    }
}

void GeneralInfoWidget::slotProjectDirectoryComboChanged() {
    QString text = project_directory_edit->text();
    if(isProjectDirectoryAbsolute() && text[0] != '/' )
        project_directory_edit->setText(ProjectManager::projectDirectory(text,false));
    else if(!isProjectDirectoryAbsolute() && text[0] == '/')
    {
        project_directory_edit->setText(KURL(ProjectManager::getInstance()->projectFile(), text).url());
    }
}

void GeneralInfoWidget::setProjectDirectoryError( const QString& error ) {
    project_directory_diagnostic_icon->setPixmap(SmallIcon("no"));
    project_directory_diagnostic_label->setText( error );
}

void GeneralInfoWidget::setProjectDirectorySuccess() {
    project_directory_diagnostic_icon->setPixmap(SmallIcon("ok"));
    if(isProjectDirectoryAbsolute())
        project_directory_diagnostic_label->setText(
            i18n("'%1' is a valid project directory.").arg(projectDirectory()));
    else
        project_directory_diagnostic_label->setText(
            i18n("'%1' is a valid project directory.").arg(projectDirectory()));
}

QString makeRelativePath(const QString& fromPath, const QString& toPath)
{
    if ( fromPath == toPath )
        return ".";

    QStringList fromDirs = QStringList::split( '/', fromPath );
    QStringList toDirs   = QStringList::split( '/', toPath );
    QStringList::iterator fromIt = fromDirs.begin();
    QStringList::iterator toIt   = toDirs.begin();

    QString relative;

    for ( ; (*fromIt) == (*toIt); ++fromIt, ++toIt )
        ;

    for ( ; fromIt != fromDirs.end(); ++fromIt )
        relative += "../";

    for ( ; toIt != toDirs.end(); ++toIt )
        relative += *toIt + "/";

    return relative;
}
