/***************************************************************************
                     kdevsetup.cpp - the setup dialog for KDevelop
                             -------------------

    begin                : 17 Aug 1998
    copyright            : (C) 1998 by Sandy Meier
    email                : smeier@rz.uni-potsdam.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ckdevsetupdlg.h"

#include "ckdevelop.h"
#include "resource.h"

#include <kmessagebox.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kfiledialog.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qgrid.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// SETUP DIALOG
CKDevSetupDlg::CKDevSetupDlg(KAccel* accel_pa, QWidget *parent, const char *name )
  : QTabDialog( parent, name,TRUE )
{
  accel = accel_pa;
  wantsTreeRefresh=false;

  setCaption( i18n("KDevelop Setup" ));
  config=KGlobal::config();

  // ****************** the General Tab ********************
  w1 = new QWidget( this, "general" );

  QGridLayout *grid1 = new QGridLayout(w1,4,1,15,7);
  QButtonGroup* makeGroup;
  makeGroup = new QButtonGroup( w1, "makeGroup" );
  //makeGroup->setGeometry( 10, 10, 400, 60 );
  grid1->addWidget(makeGroup,0,0);
  makeGroup->setFrameStyle( 49 );
  makeGroup->setTitle(i18n( "Make-Command" ));
  makeGroup->setAlignment( 1 );
  makeGroup->lower();

  QLabel* makeSelectLabel;
  QGridLayout *grid2 = new QGridLayout(makeGroup,1,2,15,7);
  makeSelectLabel = new QLabel( makeGroup, "makeSelectLabel" );
  grid2->addWidget(makeSelectLabel,0,0);
  makeSelectLabel->setText(i18n("Select Make-Command:"));
  makeSelectLabel->setAlignment( 289 );
  makeSelectLabel->setMargin( -1 );

  config->setGroup("General Options");
  QString make_cmd=config->readEntry("Make","make");

  makeSelectLineEdit = new QLineEdit( makeGroup, "makeSelectLineEdit" );
  grid2->addWidget(makeSelectLineEdit,0,1);
  makeSelectLineEdit->setText(make_cmd);

  QString makeSelectMsg = i18n("Make-Command\n\n"
							  "Select your system's make-command.\n"
							  "Usually, this is make, FreeBSD users\n"
							  "may use gmake. Mind that you can also\n"
							  "add option parameters to your make-binary\n"
					  "as well.");
  QWhatsThis::add(makeGroup, makeSelectMsg);
  QWhatsThis::add(makeSelectLabel, makeSelectMsg);
  QWhatsThis::add(makeSelectLineEdit, makeSelectMsg);

  bool autoSave=config->readBoolEntry("Autosave",true);

  QButtonGroup* autosaveGroup;
  autosaveGroup = new QButtonGroup( w1, "autosaveGroup" );
  grid2 = new QGridLayout(autosaveGroup,2,2,15,7);

  autosaveGroup->setFrameStyle( 49 );
  autosaveGroup->setTitle( i18n("Autosave") );
  autosaveGroup->setAlignment( 1 );
  //  autosaveGroup->insert( autoSaveCheck );
  autosaveGroup->lower();

  autoSaveCheck = new QCheckBox( autosaveGroup, "autoSaveCheck" );
  grid2->addWidget(autoSaveCheck,0,0);
  autoSaveCheck->setText(i18n("enable Autosave"));
  autoSaveCheck->setAutoRepeat( FALSE );
  autoSaveCheck->setAutoResize( FALSE );
  autoSaveCheck->setChecked(autoSave);

  QLabel* autosaveTimeLabel;
  autosaveTimeLabel = new QLabel( autosaveGroup, "autosaveTimeLabel" );
  grid2->addWidget(autosaveTimeLabel,1,0);
  autosaveTimeLabel->setText(i18n("Select Autosave time-interval:"));
  autosaveTimeLabel->setAlignment( 289 );
  autosaveTimeLabel->setMargin( -1 );
  autosaveTimeLabel->setEnabled(autoSave);

  autosaveTimeCombo = new QComboBox( FALSE, autosaveGroup, "autosaveTimeCombo" );
  grid2->addWidget(autosaveTimeCombo,1,1);
  autosaveTimeCombo->setSizeLimit( 10 );
  autosaveTimeCombo->setAutoResize( FALSE );
  autosaveTimeCombo->insertItem(i18n("3 min"),0 );
  autosaveTimeCombo->insertItem(i18n("5 min"),1 );
  autosaveTimeCombo->insertItem(i18n("15 min"),2 );
  autosaveTimeCombo->insertItem(i18n("30 min"),3 );
  autosaveTimeCombo->setEnabled(autoSave);
  int configTime=config->readNumEntry("Autosave Timeout",3*60*1000);
  if(configTime==3*60*1000)
    autosaveTimeCombo->setCurrentItem(0);
  if(configTime==5*60*1000)
    autosaveTimeCombo->setCurrentItem(1);
  if(configTime==15*60*1000)
    autosaveTimeCombo->setCurrentItem(2);
  if(configTime==30*60*1000)
    autosaveTimeCombo->setCurrentItem(3);

  QString autosaveMsg = i18n("Autosave\n\nIf autosave is enabled, your currently\n"
                                "changed files will be saved after the\n"
                                "time-interval selected times out.\n\n"
                                "Please select your timeout-value.\n"
                                "Available are: 3 minutes, 5 minutes,\n"
                                "15 minutes and 30 minutes.");
  QWhatsThis::add(autosaveTimeLabel,autosaveMsg);
  QWhatsThis::add(autosaveTimeCombo, autosaveMsg);
  QWhatsThis::add(autoSaveCheck, autosaveMsg);
  QWhatsThis::add(autosaveGroup, autosaveMsg);
  grid1->addWidget(autosaveGroup,1,0);

  QButtonGroup* autoswitchGroup;
  autoswitchGroup = new QButtonGroup( w1, "autoswitchGroup" );

  autoswitchGroup->setFrameStyle( 49 );
  autoswitchGroup->setTitle(i18n( "Autoswitch") );
  autoswitchGroup->setAlignment( 1 );
  //  autoswitchGroup->insert( autoSwitchCheck );
  autoswitchGroup->lower();
  grid2 = new QGridLayout(autoswitchGroup,1,2,15,7);
  autoSwitchCheck = new QCheckBox( autoswitchGroup, "autoSwitchCheck" );
  grid2->addWidget(autoSwitchCheck,0,0);
  autoSwitchCheck->setText(i18n("enable Autoswitch"));
  autoSwitchCheck->setAutoRepeat( FALSE );
  autoSwitchCheck->setAutoResize( FALSE );
  bool autoSwitch=config->readBoolEntry("Autoswitch",true);
  autoSwitchCheck->setChecked( autoSwitch );

  defaultClassViewCheck = new QCheckBox( autoswitchGroup, "defaultClassViewCheck" );
  grid2->addWidget(defaultClassViewCheck,0,1);
  defaultClassViewCheck->setText(i18n( "use Class View as default"));
  defaultClassViewCheck->setAutoRepeat( FALSE );
  defaultClassViewCheck->setAutoResize( FALSE );
  bool defaultcv=config->readBoolEntry("DefaultClassView",true);
  defaultClassViewCheck->setChecked( defaultcv );
  QWhatsThis::add(defaultClassViewCheck, i18n("use Class View as default\n\n"
					      "If this is enabled, KDevelop\n"
					      "will automatically switch to\n"
					      "the Class Viewer when switching.\n"
					      "When disabled, KDevelop will\n"
					      "use Logical File Viewer for\n"
					      "autoswitching."));

  QString autoswitchMsg = i18n("Autoswitch\n\n"
						       "If autoswitch is enabled, KDevelop\n"
						       "will open windows in the working\n"
						       "view automatically according to\n"
						       "most needed functionality.\n\n"
						       "Disableing autoswitch means you\n"
						       "will have to switch to windows\n"
						       "yourself, including turning on and\n"
						       "off the outputwindow.");
  QWhatsThis::add(autoSwitchCheck, autoswitchMsg);
  QWhatsThis::add(autoswitchGroup, autoswitchMsg);
  grid1->addWidget(autoswitchGroup,2,0);
//  connect( autoSwitchCheck, SIGNAL(toggled(bool)),defaultClassViewCheck, SLOT(setEnabled(bool)));

  QButtonGroup* startupGroup = new QButtonGroup( w1, "startupGroup" );
  //startupGroup->setGeometry( 10, 260, 400, 70 );

  grid2 = new QGridLayout(startupGroup,2,2,15,7);
  startupGroup->setFrameStyle( 49 );
  startupGroup->setTitle(i18n("Startup"));
  startupGroup->setAlignment( 1 );
  //	startupGroup->insert( logoCheck );
  //	startupGroup->insert( lastProjectCheck );
  startupGroup->lower();

  QWhatsThis::add(startupGroup, i18n("Startup\n\n"
	                  "The Startup group offers options for\n"
				     "starting KDevelop"));

  config->setGroup("General Options");
  bool logo=config->readBoolEntry("Logo",true);
  bool lastprj=config->readBoolEntry("LastProject",true);

  logoCheck = new QCheckBox( startupGroup, "logoCheck" );
  grid2->addWidget(logoCheck,0,0);
  logoCheck->setText(i18n("Startup Logo"));
  logoCheck->setAutoRepeat( FALSE );
  logoCheck->setAutoResize( FALSE );
  logoCheck->setChecked( logo );

  QWhatsThis::add(logoCheck, i18n("Startup Logo\n\n"
	                  "If Startup Logo is enabled, KDevelop will show the\n"
	                  "logo picture while it is starting."));

  lastProjectCheck = new QCheckBox( startupGroup, "lastProjectCheck" );
  grid2->addWidget(lastProjectCheck,1,0);
  lastProjectCheck->setText(i18n("Load last project"));
  lastProjectCheck->setAutoRepeat( FALSE );
  lastProjectCheck->setAutoResize( FALSE );
  lastProjectCheck->setChecked( lastprj );

  QWhatsThis::add(lastProjectCheck, i18n("Load last project\n\n"
                    "If Load last project is enabled, KDevelop will load\n"
                    "the last project used."));


  config->setGroup("TipOfTheDay");
  bool tip=config->readBoolEntry("show_tod",true);


	tipDayCheck = new QCheckBox( startupGroup, "tipDayCheck" );
                grid2->addWidget(tipDayCheck,0,1);
	tipDayCheck->setText(i18n("Tip of the Day"));
	tipDayCheck->setAutoRepeat( FALSE );
	tipDayCheck->setAutoResize( FALSE );
	tipDayCheck->setChecked( tip );

	QWhatsThis::add(tipDayCheck, i18n("Tip of the Day\n\n"
	                  "If Tip of the Day is enabled, KDevelop will show the\n"
	                  "Tip of the Day every time it starts."));
  grid1->addWidget(startupGroup,3,0);
  connect( autoSwitchCheck, SIGNAL(toggled(bool)),parent, SLOT(slotOptionsAutoswitch(bool)) );
  connect( autoSwitchCheck, SIGNAL(toggled(bool)),defaultClassViewCheck, SLOT(setEnabled(bool)));
  connect( autosaveTimeCombo, SIGNAL(activated(int)),parent, SLOT(slotOptionsAutosaveTime(int)) );
  connect( autoSaveCheck, SIGNAL(toggled(bool)),parent, SLOT(slotOptionsAutosave(bool)) );
  connect( autoSaveCheck, SIGNAL(toggled(bool)),autosaveTimeLabel, SLOT(setEnabled(bool)) );
  connect( autoSaveCheck, SIGNAL(toggled(bool)),autosaveTimeCombo, SLOT(setEnabled(bool)) );
  connect( defaultClassViewCheck,SIGNAL(toggled(bool)),parent,SLOT(slotOptionsDefaultCV(bool)));

  // ****************** the Keys Tab ***************************

//  dict = new QDict<KKeyEntry>( accel->keyDict() );
  //  KKeyChooser* w2 = new KKeyChooser ( dict,this);
  keyMap = accel->keyDict();
  w2 = new QWidget( this, "keys" );
  grid1 = new QGridLayout(w2,2,1,15,7);

  w21 = new KKeyChooser ( &keyMap, w2, true);
  grid1->addWidget( w21,0,0);

  // ****************** the Documentation Tab ********************
  w = new QWidget( this, "documentaion" );
  grid1 = new QGridLayout(w,2,1,15,7);

  config->setGroup("Doc_Location");

  QWhatsThis::add(w, i18n("Enter the path to your QT and KDE-Libs\n"
				"Documentation for the Documentation Browser.\n"
				"QT usually comes with complete Documentation\n"
				"whereas for KDE you can create the Documentation\n"
				"easily by pressing the Update button below."));

  QButtonGroup* docGroup;

  docGroup = new QButtonGroup( w, "docGroup" );
  grid1->addWidget(docGroup,0,0);
  docGroup->setFrameStyle( 49 );
  docGroup->setTitle(i18n("Directories"));
  docGroup->setAlignment( 1 );
  docGroup->lower();
  grid2 = new QGridLayout(docGroup,2,3,15,7);

  qt_edit = new QLineEdit( docGroup, "qt_edit" );
  grid2->addWidget(qt_edit,0,1);
  qt_doc_path= config->readEntry("doc_qt", QT_DOCDIR);
  qt_edit->setText(qt_doc_path);
  qt_edit->setMaxLength( 32767 );

  QPushButton* qt_button;
  qt_button = new QPushButton( docGroup, "qt_button" );
  grid2->addWidget(qt_button,0,2);
  QPixmap pix = BarIcon("open");
  qt_button->setPixmap(pix);
  connect(qt_button,SIGNAL(clicked()),SLOT(slotQtClicked()));

  QLabel* qt_label;
  qt_label = new QLabel( docGroup, "qt_label" );
  grid2->addWidget(qt_label,0,0);
  qt_label->setText( i18n("Qt-Library-Doc:") );

  QString qtdocMsg = i18n("Enter the path to your QT-Documentation\n"
				 "here. To access the path easier please\n"
				 "press the pushbutton on the right to change\n"
				 "directories.\n\n"
				 "Usually the QT-Documentation is\n"
				 "located in <i><blue>$QTDIR/html</i>");

  QWhatsThis::add(qt_edit, qtdocMsg);
  QWhatsThis::add(qt_button, qtdocMsg);
  QWhatsThis::add(qt_label, qtdocMsg);

  kde_edit = new QLineEdit( docGroup, "kde_edit");
  grid2->addWidget(kde_edit,1,1);
  kde_doc_path=config->readEntry("doc_kde", KDELIBS_DOCDIR);
  kde_edit->setText(kde_doc_path);
  kde_edit->setMaxLength( 32767 );
  kde_edit->setEchoMode( QLineEdit::Normal );
  kde_edit->setFrame( TRUE );

  QPushButton* kde_button;
  kde_button = new QPushButton(  docGroup, "kde_button" );
  grid2->addWidget(kde_button,1,2);
  kde_button->setPixmap(pix);
  connect(kde_button,SIGNAL(clicked()),SLOT(slotKDEClicked()));

  QLabel* kde_label;
  kde_label = new QLabel(  docGroup, "kde_label" );
  grid2->addWidget(kde_label,1,0);
  kde_label->setText( i18n("KDE-Libraries-Doc:") );

  QString kdedocMsg = i18n("Enter the path to your KDE-Documentation\n"
				 "here. To access the path easier please\n"
				 "press the pushbutton on the right to change\n"
				 "directories.\n\n"
				 "If you have no kdelibs Documentation installed,\n"
				 "you can create it by selecting the Update button\n"
				 "below.");
  QWhatsThis::add(kde_edit, kdedocMsg);
  QWhatsThis::add(kde_button, kdedocMsg);
  QWhatsThis::add(kde_label, kdedocMsg);



  QButtonGroup* docOptionsGroup;
  docOptionsGroup = new QButtonGroup( w, "docOptionsGroup" );
  grid1->addWidget(docOptionsGroup,1,0);
  docOptionsGroup->setFrameStyle( 49 );
  docOptionsGroup->setTitle(i18n("Options"));
  docOptionsGroup->setAlignment( 1 );
  grid2 = new QGridLayout(docOptionsGroup,3,2,15,7);
  QLabel* update_label;
  update_label = new QLabel( docOptionsGroup, "update_label" );
  grid2->addWidget(update_label,0,0);
  update_label->setText(i18n("Update KDE-Documentation :"));
  update_label->setAlignment( 289 );
  update_label->setMargin( -1 );

  QPushButton* update_button;
  update_button = new QPushButton( docOptionsGroup, "update_button" );
  grid2->addWidget(update_button,0,1);
  connect( update_button, SIGNAL(clicked()), SLOT(slotKDEUpdateReq()) );
  update_button->setText(i18n("Update..."));
  update_button->setAutoRepeat( FALSE );
  update_button->setAutoResize( FALSE );

  QString updateMsg = i18n("Update KDE-Documentation\n\n"
				     "This lets you create or update the\n"
				     "HTML-documentation of the KDE-libs.\n"
				     "Mind that you have kdoc installed to\n"
				     "use this function. Also, the kdelibs\n"
				     "sources have to be available to create\n"
				     "the documentation, as well as the \n"
				     "Qt-Documentation path has to be set to\n"
				     "cross-reference the KDE-Documentation\n"
				     "with the Qt-classes.");
  QWhatsThis::add(update_label, updateMsg);
  QWhatsThis::add(update_button, updateMsg);

  QLabel* create_label;
  create_label = new QLabel( docOptionsGroup, "create_label" );
  grid2->addWidget(create_label,1,0);
  create_label->setText(i18n("Create Search Database :") );
  create_label->setAlignment( 289 );
  create_label->setMargin( -1 );

  QPushButton* create_button;
  create_button = new QPushButton( docOptionsGroup, "create_button" );
  grid2->addWidget(create_button,1,1);
  connect( create_button, SIGNAL(clicked()),parent, SLOT(slotOptionsCreateSearchDatabase()) );
  create_button->setText(i18n("Create..."));
  create_button->setAutoRepeat( FALSE );
  create_button->setAutoResize( FALSE );

  QString createMsg = i18n("Create Search Database\n\n"
						     "This will create a search database for glimpse\n"
						     "which will be used to look up marked text in\n"
						     "the documentation. We recommend updating the\n"
						     "database each time you've changed the documentation\n"
						     "e.g. after a kdelibs-update or installing a new\n"
						     "Qt-library version.");

  QWhatsThis::add(create_label, createMsg);
  QWhatsThis::add(create_button, createMsg);

  kdocCheck = new QCheckBox( docOptionsGroup, "kdocCheck" );
  grid2->addWidget(kdocCheck,2,0);
  kdocCheck->setText(i18n("Create also KDOC-reference of your project"));
  kdocCheck->setAutoRepeat( FALSE );
  kdocCheck->setAutoResize( FALSE );
  bool bCreateKDoc;

  config->setGroup("General Options");
  bCreateKDoc = config->readBoolEntry("CreateKDoc", false);

  kdocCheck->setChecked( bCreateKDoc );

  QWhatsThis::add(kdocCheck, i18n("Create KDOC-reference of your project\n\n"
                    "If this is enabled, on creating the API-Documentation KDoc creates also\n"
                    "a cross reference file of your project into the seleceted kdoc-reference\n"
                    "directory."));

  // ****************** the Debugger Tab ***************************

  config->setGroup("Debug");
  bool useExternalDbg       = config->readBoolEntry("Use external debugger", false);
  QString dbg_cmd           = config->readEntry("External debugger program","kdbg");
  bool displayMangledNames  = config->readBoolEntry("Display mangled names", false);
  bool displayStaticMembers = config->readBoolEntry("Display static members", false);
  bool setBPsOnLibLoad      = config->readBoolEntry("Break on loading libs", false);
  bool dbgFloatingToolbar   = config->readBoolEntry("Enable floating toolbar", false);
  bool dbgTerminal          = config->readBoolEntry("Debug on separate tty console", false);

  w3 = new QWidget( this, "debug" );
  grid1 = new QGridLayout(w3,3,1,15,7);
  dbgExternalCheck = new QCheckBox( w3, "dbgExternal" );
  grid1->addWidget(dbgExternalCheck,0,0);
  dbgExternalCheck->setText(i18n("Use external debugger"));
  dbgExternalCheck->setAutoRepeat( FALSE );
  dbgExternalCheck->setAutoResize( FALSE );
  dbgExternalCheck->setChecked(useExternalDbg);

  QWhatsThis::add(dbgExternalCheck, i18n("Select internal or external debugger\n\n"
	                  "Choose whether to use an external debugger\n"
	                  "or the internal debugger within kdevelop\n"
                    "The internal debugger is a frontend to gdb"));

  dbgExternalGroup = new QButtonGroup( w3, "dbgExternalGroup" );
  grid2 = new QGridLayout(dbgExternalGroup,1,2,15,7);
  dbgExternalGroup->setFrameStyle( 49 );
  dbgExternalGroup->setTitle(i18n( "External" ));
  dbgExternalGroup->setAlignment( 1 );
  dbgExternalGroup->lower();

  dbgSelectCmdLabel = new QLabel( dbgExternalGroup, "dbgSelectLabel" );
  grid2->addWidget(dbgSelectCmdLabel,0,0);
  dbgSelectCmdLabel->setText(i18n("Select debug command:"));
  dbgSelectCmdLabel->setAlignment( 289 );
  dbgSelectCmdLabel->setMargin( -1 );

  dbgExternalSelectLineEdit = new QLineEdit( dbgExternalGroup, "dbgExternalSelectLineEdit" );
  grid2->addWidget(dbgExternalSelectLineEdit,0,1);
  dbgExternalSelectLineEdit->setText(dbg_cmd);
  grid1->addWidget(dbgExternalGroup,1,0);

  QString dbgSelectCmdMsg = i18n("Identify the external debugger\n\n"
	                  "Enter the program name you wish to run\n"
	                  "as your debugger");
  QWhatsThis::add(dbgSelectCmdLabel, dbgSelectCmdMsg);
  QWhatsThis::add(dbgExternalSelectLineEdit, dbgSelectCmdMsg);

  dbgInternalGroup = new QButtonGroup( w3, "dbgInternalGroup" );
  grid1->addWidget(dbgInternalGroup,2,0);
  grid2 = new QGridLayout(dbgInternalGroup,5,1,15,7);
  dbgInternalGroup->setFrameStyle( 49 );
  dbgInternalGroup->setTitle(i18n( "Internal" ));
  dbgInternalGroup->setAlignment( 1 );
  dbgInternalGroup->lower();

  dbgMembersCheck = new QCheckBox( dbgInternalGroup, "dbgMembers" );
  grid2->addWidget( dbgMembersCheck,0,0);
  dbgMembersCheck->setText(i18n("Display static members"));
  dbgMembersCheck->setAutoRepeat( FALSE );
  dbgMembersCheck->setAutoResize( FALSE );
  dbgMembersCheck->setChecked(displayStaticMembers);
  QWhatsThis::add(dbgMembersCheck, i18n("Display static members\n\n"
	                  "Displaying static members makes gdb slower in\n"
                    "producing data within kde and qt.\n"
                    "It may change the \"signature\" of the data\n"
                    "which QString and friends rely on.\n"
                    "But if you need to debug into these values then\n"
                    "check this option" ));

  dbgAsmCheck = new QCheckBox( dbgInternalGroup, "dbgMembers" );
  grid2->addWidget( dbgAsmCheck,1,0);
  dbgAsmCheck->setText(i18n("Display mangled names"));
  dbgAsmCheck->setAutoRepeat( FALSE );
  dbgAsmCheck->setAutoResize( FALSE );
  dbgAsmCheck->setChecked(displayMangledNames);
  QWhatsThis::add(dbgAsmCheck, i18n("Display mangled names\n\n"
	                  "When displaying the disassembled code you\n"
	                  "can select to see the methods mangled names\n"
                    "However, non-mangled names are easier to read." ));

  dbgLibCheck = new QCheckBox( dbgInternalGroup, "dbgMembers" );
  grid2->addWidget( dbgLibCheck,2,0);
  dbgLibCheck->setText(i18n("Try setting BPs on lib load"));
  dbgLibCheck->setAutoRepeat( FALSE );
  dbgLibCheck->setAutoResize( FALSE );
  dbgLibCheck->setChecked(setBPsOnLibLoad);
  QWhatsThis::add(dbgLibCheck, i18n("Set pending breakpoints on loading a library\n\n"
	                  "If GDB hasn't seen a library that will be loaded via\n"
	                  "\"dlopen\" then it'll refuse to set a breakpoint in that code.\n"
                    "We can get gdb to stop on a library load and hence\n"
                    "try to set the pending breakpoints. See docs for more\n"
                    "details and a \"gotcha\" relating to this behaviour.\n\n"
                    "If you are not \"dlopen\"ing libs leave this off." ));

  dbgFloatCheck = new QCheckBox( dbgInternalGroup, "dbgFloatToolbar" );
  //dbgFloatCheck->setGeometry( 20, 240, 310, 25 );
  grid2->addWidget( dbgFloatCheck,3,0);
  dbgFloatCheck->setText(i18n("Enable floating toolbar"));
  dbgFloatCheck->setAutoRepeat( FALSE );
  dbgFloatCheck->setAutoResize( FALSE );
  dbgFloatCheck->setChecked(dbgFloatingToolbar);
  QWhatsThis::add(dbgFloatCheck, i18n("Enable floating toolbar\n\n"
	                  "Use the floating toolbar. This toolbar always stays\n"
                    "on top of all windows so that if the app covers KDevelop\n"
                    "you have control of the app though the small toolbar\n"
                    "Also this toolbar can be docked to the panel\n"
                    "This toolbar is in addition to the toolbar in KDevelop" ));

  dbgTerminalCheck = new QCheckBox( dbgInternalGroup, "dbgTerminalCheck" );
  grid2->addWidget( dbgTerminalCheck,4,0);
  dbgTerminalCheck->setText(i18n("Enable separate terminal for application i/o"));
  dbgTerminalCheck->setAutoRepeat( FALSE );
  dbgTerminalCheck->setAutoResize( FALSE );
  dbgTerminalCheck->setChecked(dbgTerminal);
  QWhatsThis::add(dbgTerminalCheck, i18n("Enable separate terminal for application i/o\n\n"
                    "This allows you to enter terminal input when your\n"
                    "application contains terminal input code (eg cin, fgets etc.) \n"
                    "If you use terminal input in your app, then tick this option.\n"
                    "Otherwise leave this off." ));

  slotSetDebug();
  connect( dbgExternalCheck, SIGNAL(toggled(bool)), SLOT(slotSetDebug()));

//************************** QT-2 directory select *************************//
  w4 = new QWidget( this, "pat" );
  config->setGroup("QT2");
  grid1 = new QGridLayout(w4,2,1,15,7);

  QGroupBox* kde2_box= new QGroupBox(w4,"NoName");
  grid1->addWidget(kde2_box,0,0);
  kde2_box->setTitle(i18n("Qt 2.x / KDE path"));
  grid2 = new QGridLayout(kde2_box,4,2,15,7);

  QLabel* qt2= new QLabel(kde2_box,"NoName");
  qt2->setText(i18n("Qt 2.x directory:"));
  grid2->addWidget(qt2,0,0);

  qt2_edit= new QLineEdit(kde2_box,"NoName");
  qt2_edit->setMaxLength( 32767 );
  grid2->addWidget(qt2_edit,1,0);

  QString qt2_path= config->readEntry("qt2dir");
  qt2_edit->setText(qt2_path);

  QPushButton* qt2_button= new QPushButton(kde2_box,"NoName");
  qt2_button->setPixmap(pix);
  grid2->addWidget(qt2_button,1,1);

  QString qt2Msg = i18n("Set the root directory path leading to your Qt 2.x path, e.g. /usr/lib/qt-2.0");
  QWhatsThis::add(qt2_edit, qt2Msg);
  QWhatsThis::add(qt2_button, qt2Msg);
  QWhatsThis::add(qt2, qt2Msg);


  QLabel* kde2= new QLabel(kde2_box,"NoName");
  kde2->setText(i18n("KDE 2.x directory:"));
  grid2->addWidget(kde2,2,0);

  kde2_edit= new QLineEdit(kde2_box,"NoName");
  grid2->addWidget(kde2_edit,3,0);

  QString kde2_path= config->readEntry("kde2dir");
  kde2_edit->setText(kde2_path);


  QPushButton* kde2_button= new QPushButton(kde2_box,"NoName");
  kde2_button->setPixmap(pix);
 grid2->addWidget(kde2_button,3,1);

  QString kde2Msg = i18n("Set the root directory path leading to your KDE 2 includes/libraries, e.g. /opt/kde2");
  QWhatsThis::add(kde2_edit, kde2Msg);
  QWhatsThis::add(kde2_button, kde2Msg);
  QWhatsThis::add(kde2, kde2Msg);

  connect(qt2_button,SIGNAL(clicked()),SLOT(slotQt2Clicked()));
  connect(kde2_button, SIGNAL(clicked()),SLOT(slotKDE2Clicked()));

  QGroupBox* ppath_box= new QGroupBox(w4,"NoName");
  grid1->addWidget( ppath_box,1,0);
  grid2 = new QGridLayout(ppath_box,2,2,15,7);
  ppath_box->setTitle(i18n("Default Project Path"));

// --- added by Olaf Hartig (olaf@punkbands.de) 22.Feb.2000
  config->setGroup("General Options");
  QLabel* ppath= new QLabel(ppath_box,"NoName");
  ppath->setText(i18n("Project Path:"));
  grid2->addWidget(ppath,0,0);

  ppath_edit= new QLineEdit(ppath_box,"NoName");
  grid2->addWidget(ppath_edit,1,0);

  QString project_path= config->readEntry("ProjectDefaultDir", QDir::homeDirPath());
  ppath_edit->setText(project_path);

  QPushButton* ppath_button= new QPushButton(ppath_box,"NoName");
ppath_button->setPixmap(pix);
 grid2->addWidget(ppath_button,1,1);
  connect(ppath_button, SIGNAL(clicked()),SLOT(slotPPathClicked()));
// ---

  QString ppathMsg = i18n("Set the start directory where to create/load projects here");
  QWhatsThis::add(ppath_edit, ppathMsg);
  QWhatsThis::add(ppath_button, ppathMsg);
  QWhatsThis::add(ppath, ppathMsg);

  // *********** tabs ****************
  addTab(w1, i18n("General"));
  addTab(w2, i18n("Keys"));
  addTab( w, i18n("Documentation" ));
  addTab(w3, i18n("Debugger" ));
  addTab(w4, i18n("Path"));

  // **************set the button*********************
  setDefaultButton(i18n("Default"));
  setOkButton(i18n("OK"));
  setCancelButton(i18n("Cancel"));
  connect( this, SIGNAL(defaultButtonPressed()), SLOT(slotDefault()) );
  connect( this, SIGNAL(applyButtonPressed()), SLOT(slotOkClicked()) );
  connect( this, SIGNAL(applyButtonPressed()),parent, SLOT(slotOptionsMake()) );
  resize(440,420);
}

void CKDevSetupDlg::slotDefault(){

  // General tab
  if(w1->isVisible()){
    makeSelectLineEdit->setText("make");

    autoSaveCheck->setChecked(true);
    autosaveTimeCombo->setCurrentItem(1);

    autoSwitchCheck->setChecked(true);
    defaultClassViewCheck->setChecked(false);
    logoCheck->setChecked(true);
    lastProjectCheck->setChecked(true);
    tipDayCheck->setChecked(true);
  }
  // keychooser tab
  if(w2->isVisible())
    w21->allDefault();
}

void CKDevSetupDlg::slotOkClicked(){
  QString text;
  int answer;

  // check now the documentation locations
  config->setGroup("Doc_Location");
  wantsTreeRefresh=false;

  text = qt_edit->text();
  if(text.right(1) != "/" ){
    text = text + "/";
  }
  QString qt_testfile=text+"classes.html"; // test if the path really is the qt-doc path
  answer=KMessageBox::Yes;
  if(!QFileInfo(qt_testfile).exists())
  {
    answer=KMessageBox::questionYesNo(this,i18n("The chosen path does not lead to the\n"
                                                "Qt-library documentation. Do you really want to save\n"
                                                "this value?"),
                                            i18n("The selected path is not correct!"));
  }

  if (answer==KMessageBox::Yes)
  {
     config->writeEntry("doc_qt",text);
     wantsTreeRefresh |= (qt_doc_path != text);
  }
  answer=KMessageBox::Yes;    // simulate again ok...
  text = kde_edit->text();
  if(text.right(1) != "/" ){
    text = text + "/";
  }
  QString kde_testfile=text+"kdecore/index.html"; // test if the path really is the qt-doc path
  if(!QFileInfo(kde_testfile).exists())
  {
    answer=KMessageBox::questionYesNo(this,i18n("The chosen path does not lead to the\n"
                                                "KDE-library documentation. Do you really want to save\n"
                                                "this value?"),
                                            i18n("The selected path is not correct!"));
  }

  if (answer==KMessageBox::Yes)
  {
     config->writeEntry("doc_kde",text);
     wantsTreeRefresh |= (kde_doc_path != text);
  }

  config->setGroup("General Options");

  bool autosave=autoSaveCheck->isChecked();
  config->writeEntry("Autosave",autosave);

  int timeCurrent=autosaveTimeCombo->currentItem();
  if(timeCurrent==0)
    config->writeEntry("Autosave Timeout",3*60*1000);
  if(timeCurrent==1)
    config->writeEntry("Autosave Timeout",5*60*1000);
  if(timeCurrent==2)
    config->writeEntry("Autosave Timeout",15*60*1000);
  if(timeCurrent==3)
    config->writeEntry("Autosave Timeout",30*60*1000);

  bool autoswitch=autoSwitchCheck->isChecked();
  config->writeEntry("Autoswitch",autoswitch);

  bool defaultcv=defaultClassViewCheck->isChecked();
  config->writeEntry("DefaultClassView",defaultcv);

  config->writeEntry("Make",makeSelectLineEdit->text());

  bool logo=logoCheck->isChecked();
  config->writeEntry("Logo",logo);

  bool kdoc=kdocCheck->isChecked();
  config->writeEntry("CreateKDoc",kdoc);

  bool lastprj=lastProjectCheck->isChecked();
  config->writeEntry("LastProject",lastprj);
	
	config->setGroup("TipOfTheDay");
  config->writeEntry("show_tod",tipDayCheck->isChecked());

  config->setGroup("Debug");
  config->writeEntry("Use external debugger", dbgExternalCheck->isChecked());
  config->writeEntry("External debugger program", dbgExternalSelectLineEdit->text());
  config->writeEntry("Display mangled names", dbgAsmCheck->isChecked());
  config->writeEntry("Display static members", dbgMembersCheck->isChecked());
  config->writeEntry("Break on loading libs", dbgLibCheck->isChecked());
  config->writeEntry("Enable floating toolbar", dbgFloatCheck->isChecked());
  config->writeEntry("Debug on separate tty console", dbgTerminalCheck->isChecked());

  config->setGroup("QT2");
  config->writeEntry("qt2dir", qt2_edit->text());
  config->writeEntry("kde2dir", kde2_edit->text());

// --- added by Olaf Hartig (olaf@punkbands.de) 22.Feb.2000
  config->setGroup("General Options");
  config->writeEntry("ProjectDefaultDir", ppath_edit->text());	
// ---

  accel->setKeyDict(keyMap);
  accel->writeSettings(config);
  config->sync();
  accept();
}

void CKDevSetupDlg::slotQtClicked(){
  QString dir;
  config->setGroup("Doc_Location");
  dir = KFileDialog::getExistingDirectory(config->readEntry("doc_qt", QT_DOCDIR));
  if (!dir.isEmpty()){
    qt_edit->setText(dir);

    if(dir.right(1) != "/" ){
     dir = dir + "/";
    }
    QString qt_testfile=dir+"classes.html"; // test if the path really is the qt-doc path
    if(!QFileInfo(qt_testfile).exists())
      KMessageBox::error(this,i18n("The chosen path does not lead to the\n"
                                   "Qt-library documentation. Please choose the\n"
                                   "correct path."),
                                i18n("The selected path is not correct!"));
  }
}


void CKDevSetupDlg::slotKDEClicked(){
  QString dir;
  config->setGroup("Doc_Location");
  dir = KFileDialog::getExistingDirectory(config->readEntry("doc_kde", KDELIBS_DOCDIR));
  if (!dir.isEmpty()){
    kde_edit->setText(dir);

    if(dir.right(1) != "/" ){
     dir = dir + "/";
    }

    QString kde_testfile=dir+"kdecore/index.html"; // test if the path really is the kde-doc path
    if(!QFileInfo(kde_testfile).exists())
      KMessageBox::error(this,i18n("The chosen path does not lead to the\n"
                                   "KDE-library documentation. Please choose the\n"
                                   "correct path or choose 'Update' to create a new\n"
                                   "documentation"),
                                i18n("The selected path is not correct!"));
  }
}

void CKDevSetupDlg::slotKDEUpdateReq(){
  QString new_path;
  ((CKDevelop*) parent())->slotOptionsUpdateKDEDocumentation();
  config->setGroup("Doc_Location");
  new_path=config->readEntry("doc_kde", KDELIBS_DOCDIR);

  if (kde_doc_path != new_path)
    kde_edit->setText(new_path);
}

void CKDevSetupDlg::slotSetDebug()
{
  bool externalDbg = dbgExternalCheck->isChecked();

  // external options
  dbgExternalGroup->setEnabled(externalDbg);
  dbgExternalSelectLineEdit->setEnabled(externalDbg);
  dbgSelectCmdLabel->setEnabled(externalDbg);

  // internal options
  dbgInternalGroup->setEnabled(!externalDbg);
  dbgMembersCheck->setEnabled(!externalDbg);
  dbgAsmCheck->setEnabled(!externalDbg);
  dbgLibCheck->setEnabled(!externalDbg);
  dbgFloatCheck->setEnabled(!externalDbg);
  dbgTerminalCheck->setEnabled(!externalDbg);
}

void CKDevSetupDlg::slotQt2Clicked(){
  QString dir;
  config->setGroup("QT2");
  dir = KFileDialog::getExistingDirectory(config->readEntry("qt2dir"));
  if (!dir.isEmpty()){
    qt2_edit->setText(dir);

  }
  QString qt_testfile=dir+"include/qapp.h"; // test if the path really is the qt2 path
  if(!QFileInfo(qt_testfile).exists())
  	KMessageBox::error(this,i18n("The chosen path does not lead to the\n"
                                 "Qt-2.x root directory. Please choose the\n"
                                 "correct path."),
                             i18n("The selected path is not correct!"));

}
void CKDevSetupDlg::slotKDE2Clicked(){
  QString dir;
  config->setGroup("QT2");
  dir = KFileDialog::getExistingDirectory(config->readEntry("kde2dir"));
  if (!dir.isEmpty()){
    kde2_edit->setText(dir);

  }
  QString kde_testfile=dir+"include/kmessagebox.h"; // test if the path really is the kde2 path
  if(!QFileInfo(kde_testfile).exists())
  	KMessageBox::error(this,i18n("The chosen path does not lead to the\n"
                                 "KDE-2.x root directory. Please choose the\n"
                                 "correct path."),
                             i18n("The selected path is not correct!"));

}

// --- added by Olaf Hartig (olaf@punkbands.de) 22.Feb.2000
void CKDevSetupDlg::slotPPathClicked(){
  QString dir;
  config->setGroup("General Options");
  dir = KFileDialog::getExistingDirectory(config->readEntry("ProjectDefaultDir", QDir::homeDirPath()));
  if (!dir.isEmpty()){
    ppath_edit->setText(dir);
  }
}
// ---
