// PARAVIS : ParaView wrapper SALOME module
//
// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : PVGUI_Module.cxx
// Author : Julia DOROVSKIKH

#include <Standard_math.hxx>  // E.A. must be included before Python.h to fix compilation on windows
#ifdef HAVE_FINITE
#undef HAVE_FINITE            // VSR: avoid compilation warning on Linux : "HAVE_FINITE" redefined
#endif
#include <vtkPython.h> // Python first
#include "PVGUI_Module.h"

#ifdef PARAVIS_WITH_FULL_CORBA
# include "PARAVIS_Gen_i.hh"
#endif

//#include "PV_Tools.h"

#include "PVGUI_ViewModel.h"
#include "PVGUI_ViewManager.h"
#include "PVGUI_ViewWindow.h"
#include "PVGUI_Tools.h"
#include "PVGUI_ParaViewSettingsPane.h"
#include "PVGUI_OutputWindowAdapter.h"
#include "PVGUI_Behaviors.h"

#include <SUIT_DataBrowser.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ExceptionHandler.h>

// SALOME Includes
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOMEDS_SObject.hxx"

#include "LightApp_SelectionMgr.h"
#include "LightApp_NameDlg.h"

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>
#include <SALOMEDS_Tool.hxx>
#include <PyInterp_Interp.h>
#include <PyInterp_Dispatcher.h>
#include <PyConsole_Console.h>
#include <PyConsole_Interp.h>

#include <sstream>

#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QInputDialog>
#include <QMenu>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QToolBar>
#include <QTextStream>
#include <QShortcut>
#include <QDockWidget>
#include <QHelpEngine>

#include <pqApplicationCore.h>
//#include <pqPVApplicationCore.h>
#include <pqActiveView.h>
#include <pqObjectBuilder.h>
#include <pqOptions.h>
#include <pqRenderView.h>
#include <pqServer.h>
#include <pqUndoStack.h>
#include <pqVCRController.h>
#include <pqTabbedMultiViewWidget.h>
#include <pqPipelineSource.h>
#include <pqActiveObjects.h>
#include <vtkProcessModule.h>
#include <vtkSMSession.h>
#include <vtkPVSession.h>
#include <vtkPVProgressHandler.h>
#include <pqParaViewBehaviors.h>
#include <pqHelpReaction.h>
#include <vtkOutputWindow.h>
#include <pqPluginManager.h>
#include <pqSettings.h>
#include <pqPythonDialog.h>
#include <pqPythonManager.h>
#include <pqPythonShell.h>
#include <pqLoadDataReaction.h>
#include <vtkEventQtSlotConnect.h>
#include <pqPythonScriptEditor.h>
#include <pqDataRepresentation.h>
#include <pqPipelineRepresentation.h>
#include <pqLookupTableManager.h>
#include <pqDisplayColorWidget.h>
#include <pqColorToolbar.h>
#include <pqScalarBarVisibilityReaction.h>

#include <pqServerResource.h>
#include <pqServerConnectReaction.h>
#include <pqServerDisconnectReaction.h>

#include <pqServerManagerObserver.h>
#include <vtkClientServerInterpreterInitializer.h>
#include <vtkPVConfig.h>

#include <PARAVIS_version.h>

#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

//----------------------------------------------------------------------------
PVGUI_Module* ParavisModule = 0;

/*!
  \mainpage

  <h2>Building and installing PARAVIS</h2>
  As any other SALOME module, PARAVIS requires PARAVIS_ROOT_DIR environment variable to be set to PARAVIS
  installation directory.
  Other variables needed for correct detection of ParaView location:
  \li PVHOME - points at the ParaView installation directory tree
  \li PVVERSION - number of ParaView version

  It also requires common SALOME environment including GUI_ROOT_DIR and other prerequsites.


  PARAVIS module can be launched using the following commands:
  \li Full SALOME configuration
  \code
  runSalome --modules="PARAVIS"
  \endcode

  <h2>ParaView GUI integration</h2>
  <h3>ParaView GUI integration overview</h3>

  The main idea is to reuse ParaView GUI internal logic as much as possible, providing a layer 
  between it and SALOME GUI that hides the following SALOME GUI implementation details from ParaView:

  \li SALOME GUI executable and Qt event loop
  \li SALOME GUI desktop
  \li Dock windows areas
  \li SALOME menu and toolbar managers

  Major part of the integration is implemented in PVGUI_Module class.

  <h3>ParaView client initalization</h3>

  ParaView client initalization is performed when an instance of PVGUI_Module class has been created 
  and \link PVGUI_Module::initialize() PVGUI_Module::initialize()\endlink method is called by SALOME GUI.
  The actual client start-up is done in \link PVGUI_Module::pvInit() PVGUI_Module::pvInit()\endlink method. 
  

  <h3>Multi-view manager</h3>

  SALOME GUI requires that each kind of view be implemnted with help of (at least) three classes. For ParaView multi-view manager 
  these are:

  \li PVGUI_ViewManager - view manager class
  \li PVGUI_Viewer      - view model class
  \li PVGUI_ViewWindow  - view window class that acts as a parent for %pqViewManager

  Single instances of PVGUI_ViewManager and PVGUI_ViewWindow classes are created by \link PVGUI_Module::showView() 
  PVGUI_Module::showView()\endlink method upon the first PARAVIS module activation. The same method hides the multi-view manager 
  when the module is deactivated (the user switches to another module or a study is closed). 
  A special trick is used to make PVGUI_ViewWindow the parent of %pqViewManager widget. It is created initally by %pqMainWindowCore
  with the desktop as a parent, so when it is shown PVGUI_ViewWindow instance is passed to its setParent() method. In  
  \link PVGUI_ViewWindow::~PVGUI_ViewWindow() PVGUI_ViewWindow::~PVGUI_ViewWindow()\endlink the parent is nullified to avoid deletion
  of %pqViewManager widget that would break %pqMainWindowCore class.

  <h3>ParaView plugins</h3>
  ParaView server and client plugins are managed by %pqMainWindowCore slots that has full access to PARAVIS menus and toolbars. 
  As a result they appears automatically in PARAVIS menus and toolbars if loaded successfully.
*/

/*!
  \class PVGUI_Module
  \brief Implementation 
         SALOME module wrapping ParaView GUI.
*/


/*
  Fix for the issue 21730: [CEA 596] Slice of polyhedron in PARAVIS returns no cell.
  Wrap vtkEDFCutter filter.
*/

extern "C" void vtkEDFCutterCS_Initialize(vtkClientServerInterpreter*);
static void vtkEDFHelperInit();

void vtkEDFHelperInit(vtkClientServerInterpreter* interp){
    vtkEDFCutterCS_Initialize(interp);
}

void vtkEDFHelperInit() {
    vtkClientServerInterpreterInitializer::GetInitializer()->
        RegisterCallback(&vtkEDFHelperInit);
}


  _PTR(SComponent)
  ClientFindOrCreateParavisComponent(_PTR(Study) theStudyDocument)
  {
    _PTR(SComponent) aSComponent = theStudyDocument->FindComponent("PARAVIS");
    if (!aSComponent) {
      _PTR(StudyBuilder) aStudyBuilder = theStudyDocument->NewBuilder();
      aStudyBuilder->NewCommand();
      int aLocked = theStudyDocument->GetProperties()->IsLocked();
      if (aLocked) theStudyDocument->GetProperties()->SetLocked(false);
      aSComponent = aStudyBuilder->NewComponent("PARAVIS");
      _PTR(GenericAttribute) anAttr =
        aStudyBuilder->FindOrCreateAttribute(aSComponent, "AttributeName");
      _PTR(AttributeName) aName (anAttr);

      //CORBA::ORB_var anORB = PARAVIS::PARAVIS_Gen_i::GetORB();
      int dnu;
      CORBA::ORB_var anORB = CORBA::ORB_init(dnu, 0); // suppose ORB was already initialized
      SALOME_NamingService *NamingService = new SALOME_NamingService( anORB );
      CORBA::Object_var objVarN = NamingService->Resolve("/Kernel/ModulCatalog");
      SALOME_ModuleCatalog::ModuleCatalog_var Catalogue =
        SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
      SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent( "PARAVIS" );
      if (!Comp->_is_nil()) {
        aName->SetValue(Comp->componentusername());
      }

      anAttr = aStudyBuilder->FindOrCreateAttribute(aSComponent, "AttributePixMap");
      _PTR(AttributePixMap) aPixmap (anAttr);
      aPixmap->SetPixMap( "pqAppIcon16.png" );

      // Create Attribute parameters for future using
      anAttr = aStudyBuilder->FindOrCreateAttribute(aSComponent, "AttributeParameter");

      aStudyBuilder->DefineComponentInstance(aSComponent, PVGUI_Module::GetEngine()->GetIOR());
      if (aLocked) theStudyDocument->GetProperties()->SetLocked(true);
      aStudyBuilder->CommitCommand();
    }
    return aSComponent;
  }

/*!
  Clean up function; used to stop ParaView progress events when
  exception is caught by global exception handler.
*/
void paravisCleanUp()
{
  if ( pqApplicationCore::instance() ) {
    pqServer* s = pqApplicationCore::instance()->getActiveServer();
    if ( s ) s->session()->GetProgressHandler()->CleanupPendingProgress();
  }
}

/*!
  \brief Constructor. Sets the default name for the module.
*/
PVGUI_Module::PVGUI_Module()
  : SalomeApp_Module( "PARAVIS" ),
    mySelectionControlsTb( -1 ),
    mySourcesMenuId( -1 ),
    myFiltersMenuId( -1 ),
    myMacrosMenuId(-1),
    myToolbarsMenuId(-1),
    myRecentMenuId(-1),
    myOldMsgHandler(0),
    myTraceWindow(0),
    myStateCounter(0),
    myInitTimer(0),
    myPushTraceTimer(0)
{
#ifdef HAS_PV_DOC
  Q_INIT_RESOURCE( PVGUI );
#endif
  ParavisModule = this;

  // Clear old copies of embedded macros files
  QString aDestPath = QString( "%1/.config/%2/Macros" ).arg( QDir::homePath() ).arg( QApplication::applicationName() );
  QStringList aFilter;
  aFilter << "*.py";

  QDir aDestDir(aDestPath);
  QStringList aDestFiles = aDestDir.entryList(aFilter, QDir::Files);
  foreach (QString aMacrosPath, getEmbeddedMacrosList()) {
    QString aMacrosName = QFileInfo(aMacrosPath).fileName();
    if (aDestFiles.contains(aMacrosName)) {
      aDestDir.remove(aMacrosName);
    }
  }
}

/*!
  \brief Destructor.
*/
PVGUI_Module::~PVGUI_Module()
{
  if (myPushTraceTimer)
    delete myPushTraceTimer;
  if (myInitTimer)
    delete myInitTimer;
  // Disconnect from server
  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      MESSAGE("~PVGUI_Module(): Disconnecting from remote server ...");
      pqServerDisconnectReaction::disconnectFromServer();
    }
}

PARAVIS_ORB::PARAVIS_Gen_var PVGUI_Module::GetEngine()
{
  return PVGUI_ViewerModel::GetEngine();
}

/*!
  \brief Initialize module. Creates menus, prepares context menu, etc.
  \param app SALOME GUI application instance
*/
void PVGUI_Module::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // Create ParaViS actions
  createActions();
  // Create ParaViS menus
  createMenus();

  // Uncomment to debug ParaView initialization
  // "aa" used instead of "i" as GDB doesn't like "i" variables :)
  /*
  int aa = 1;
  while( aa ){
    aa = aa;
  }
  */
  
  // Initialize ParaView client
  pvInit();

  // Create GUI elements (menus, toolbars, dock widgets)
  SalomeApp_Application* anApp = getApp();
  SUIT_Desktop* aDesktop = anApp->desktop();

  // Remember current state of desktop toolbars
  QList<QToolBar*> foreignToolbars = aDesktop->findChildren<QToolBar*>();

  setupDockWidgets();

  pvCreateActions();
  pvCreateToolBars();
  pvCreateMenus();

  QList<QDockWidget*> activeDocks = aDesktop->findChildren<QDockWidget*>();
  QList<QMenu*> activeMenus = aDesktop->findChildren<QMenu*>();

  PVGUI_Behaviors * behav = new PVGUI_Behaviors(this);
  behav->instanciateAllBehaviors(aDesktop);

  // Setup quick-launch shortcuts.
  QShortcut *ctrlSpace = new QShortcut(Qt::CTRL + Qt::Key_Space, aDesktop);
  QObject::connect(ctrlSpace, SIGNAL(activated()),
    pqApplicationCore::instance(), SLOT(quickLaunch()));

  // Find Plugin Dock Widgets
  QList<QDockWidget*> currentDocks = aDesktop->findChildren<QDockWidget*>();
  QList<QDockWidget*>::iterator i;
  for (i = currentDocks.begin(); i != currentDocks.end(); ++i) {
    if(!activeDocks.contains(*i)) {
      myDockWidgets[*i] = false; // hidden by default
      (*i)->hide();
    }
  }

  // Find Plugin Menus
  QList<QMenu*> currentMenus = aDesktop->findChildren<QMenu*>();
  QList<QMenu*>::iterator im;
  for (im = currentMenus.begin(); im != currentMenus.end(); ++im) {
    if(!activeMenus.contains(*im)) {
        myMenus.append(*im);
    }
  }

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QString aPath = resMgr->stringValue("resources", "PARAVIS", QString());
  if (!aPath.isNull()) {
    MyCoreApp->loadConfiguration(aPath + QDir::separator() + "ParaViewFilters.xml");
    MyCoreApp->loadConfiguration(aPath + QDir::separator() + "ParaViewReaders.xml");
    MyCoreApp->loadConfiguration(aPath + QDir::separator() + "ParaViewSources.xml");
    MyCoreApp->loadConfiguration(aPath + QDir::separator() + "ParaViewWriters.xml");
  }

  // Force creation of the PARAVIS engine
  GetEngine();
  updateObjBrowser();

  // Find created toolbars
  QCoreApplication::processEvents();

  QList<QToolBar*> allToolbars = aDesktop->findChildren<QToolBar*>();
  foreach(QToolBar* aBar, allToolbars) {
    if (!foreignToolbars.contains(aBar)) {
      myToolbars[aBar] = true;
      myToolbarBreaks[aBar] = false;
      aBar->setVisible(false);
      aBar->toggleViewAction()->setVisible(false);
    }
  }

  updateMacros();
 
  // we need to start trace after connection is done
  connect(pqApplicationCore::instance()->getObjectBuilder(), SIGNAL(finishedAddingServer(pqServer*)), 
          this, SLOT(onFinishedAddingServer(pqServer*)));

  connect(pqApplicationCore::instance()->getObjectBuilder(), SIGNAL(dataRepresentationCreated(pqDataRepresentation*)), 
          this, SLOT(onDataRepresentationCreated(pqDataRepresentation*)));

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool isStop = aResourceMgr->booleanValue( "PARAVIS", "stop_trace", false );
  if(!isStop)
    {
      // Start a timer to schedule asap:
      //  - the connection to the server
      //  - the trace start
      myInitTimer = new QTimer(aDesktop);
      QObject::connect(myInitTimer, SIGNAL(timeout()), this, SLOT(onInitTimer()) );
      myInitTimer->setSingleShot(true);
      myInitTimer->start(0);

      // Another timer to regularly push the trace onto the engine:
      myPushTraceTimer = new QTimer(aDesktop);
      QObject::connect(myPushTraceTimer, SIGNAL(timeout()), this, SLOT(onPushTraceTimer()) );
      myPushTraceTimer->setSingleShot(false);
      myPushTraceTimer->start(500);
    }

  this->VTKConnect = vtkEventQtSlotConnect::New();
  
  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  if(pm) {
    vtkPVSession* pvs = dynamic_cast<vtkPVSession*>(pm->GetSession());
    if(pvs) {
      vtkPVProgressHandler* ph = pvs->GetProgressHandler();
      if(ph) {
          this->VTKConnect->Connect(ph, vtkCommand::StartEvent,
                                    this, SLOT(onStartProgress()));
          this->VTKConnect->Connect(ph, vtkCommand::EndEvent,
                                    this, SLOT(onEndProgress()));
      }
    }
  }
  
  connect(&pqActiveObjects::instance(),
          SIGNAL(representationChanged(pqRepresentation*)),
          this, SLOT(onRepresentationChanged(pqRepresentation*)));
}

void PVGUI_Module::onStartProgress()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
}

void PVGUI_Module::onEndProgress()
{
  QApplication::restoreOverrideCursor();
}

void PVGUI_Module::onFinishedAddingServer(pqServer* /*server*/)
{
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool isStop = aResourceMgr->booleanValue( "PARAVIS", "stop_trace", false );
  if(!isStop) 
    startTimer( 500 );
}

void PVGUI_Module::onDataRepresentationCreated(pqDataRepresentation* data) {
  if(!data)
    return;
  
  if(!data->getLookupTable())
    return;
  
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  if(!aResourceMgr)
    return;

  bool visible = aResourceMgr->booleanValue( "PARAVIS", "show_color_legend", false );
  pqLookupTableManager* lut_mgr = pqApplicationCore::instance()->getLookupTableManager();
  
  if(lut_mgr) {
    lut_mgr->setScalarBarVisibility(data,visible);
  }

  connect(data, SIGNAL(dataUpdated()), this, SLOT(onDataRepresentationUpdated()));
}

void PVGUI_Module::onDataRepresentationUpdated() {
  SalomeApp_Study* activeStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  if(!activeStudy) return;
  
  activeStudy->Modified();
}

void PVGUI_Module::onVariableChanged(pqVariableType t, const QString) {
  
  pqDisplayColorWidget* colorWidget = qobject_cast<pqDisplayColorWidget*>(sender());
  if( !colorWidget )
    return;

  if( t == VARIABLE_TYPE_NONE )
    return;

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  
  if(!aResourceMgr)
    return;

  bool visible = aResourceMgr->booleanValue( "PARAVIS", "show_color_legend", false );
  
  if(!visible)
    return;
  
  /*//VTN: getRepresentation is protected
  pqDataRepresentation* data  = colorWidget->getRepresentation();

  if( !data->getLookupTable() )
    return;

  pqLookupTableManager* lut_mgr = pqApplicationCore::instance()->getLookupTableManager();

  if(lut_mgr) {
    lut_mgr->setScalarBarVisibility(data,visible);
  }
  */
  pqColorToolbar* colorTooBar = qobject_cast<pqColorToolbar*>(colorWidget->parent());
  if( !colorTooBar )
    return;

  pqScalarBarVisibilityReaction* scalarBarVisibility = colorTooBar->findChild<pqScalarBarVisibilityReaction *>();
  if(scalarBarVisibility) {
    scalarBarVisibility->setScalarBarVisibility(visible);
  }
}

void PVGUI_Module::execPythonCommand(const QString& cmd, bool inSalomeConsole)
{
  if ( inSalomeConsole ) {
    if ( PyInterp_Dispatcher::Get()->IsBusy() ) return;
    SalomeApp_Application* app =
      dynamic_cast< SalomeApp_Application* >(SUIT_Session::session()->activeApplication());
    PyConsole_Console* pyConsole = app->pythonConsole();
    if (pyConsole)
      pyConsole->exec(cmd);
  }
  else
    {
      SalomeApp_Application* app =
            dynamic_cast< SalomeApp_Application* >(SUIT_Session::session()->activeApplication());
      PyConsole_Interp* pyInterp = app->pythonConsole()->getInterp();
      PyLockWrapper aGil;
      pyInterp->run(cmd.toStdString().c_str());
//      pqPythonManager* manager = qobject_cast<pqPythonManager*>
//      ( pqApplicationCore::instance()->manager( "PYTHON_MANAGER" ) );
//      if ( manager )
//        {
//          pqPythonDialog* pyDiag = manager->pythonShellDialog();
//          if ( pyDiag )
//            {
//              pqPythonShell* shell = pyDiag->shell();
//              if ( shell ) {
//                  shell->executeScript(cmd);
//              }
//            }
//        }
    }
}

/*!
  \brief Initialisation timer event - fired only once, after the GUI loop is ready.
  See creation in initialize().
*/
void PVGUI_Module::onInitTimer()
{
#ifndef PARAVIS_WITH_FULL_CORBA
  connectToExternalPVServer();
#endif

#ifndef WNT
//  if ( PyInterp_Dispatcher::Get()->IsBusy() )
//    {
//      // Reschedule for later
//      MESSAGE("interpreter busy -> rescheduling trace start.");
//      startTimer(500);
//    }
//  else
//    {
      MESSAGE("timerEvent(): About to start trace....");
      execPythonCommand("from paraview import smtrace;smtrace.start_trace()", false);
      MESSAGE("timerEvent(): Trace STARTED....");
//    }
#endif

  MESSAGE("initialize(): Initializing PARAVIS's Python context ...");
  execPythonCommand("import paraview.servermanager as sm; sm.fromGUI=True", false);
  MESSAGE("initialize(): Initialized.");
}
  
/*!
  \brief Get list of embedded macros files
*/
QStringList PVGUI_Module::getEmbeddedMacrosList()
{
  QString aRootDir = getenv("PARAVIS_ROOT_DIR");

  QString aSourcePath = aRootDir + "/bin/salome/Macro";

  QStringList aFilter;
  aFilter << "*.py";

  QDir aSourceDir(aSourcePath);
  QStringList aSourceFiles = aSourceDir.entryList(aFilter, QDir::Files);
  QStringList aFullPathSourceFiles;
  foreach (QString aMacrosName, aSourceFiles) {
    aFullPathSourceFiles << aSourceDir.absoluteFilePath(aMacrosName);
  }
  return aFullPathSourceFiles;
}

void PVGUI_Module::updateMacros()
{
  pqPythonManager* aPythonManager = pqPVApplicationCore::instance()->pythonManager();
  if(!aPythonManager)  {
    return;
  }
  
  foreach (QString aStr, getEmbeddedMacrosList()) {
    aPythonManager->addMacro(aStr);
  }
}


/*!
  \brief Get list of compliant dockable GUI elements
  \param m map to be filled in ("type":"default_position")
*/
void PVGUI_Module::windows( QMap<int, int>& m ) const
{
  m.insert( LightApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  m.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
  // ParaView diagnostic output redirected here
  m.insert( LightApp_Application::WT_LogWindow, Qt::BottomDockWidgetArea );
}

/*!
  \brief Shows (toShow = true) or hides ParaView view window
*/
void PVGUI_Module::showView( bool toShow )
{
  PVGUI_ViewManager
  SalomeApp_Application* anApp = getApp();
  PVGUI_ViewManager* viewMgr =
    dynamic_cast<PVGUI_ViewManager*>( anApp->getViewManager( PVGUI_Viewer::Type(), false ) );
  if ( !viewMgr ) {
    viewMgr = new PVGUI_ViewManager( anApp->activeStudy(), anApp->desktop() );
    anApp->addViewManager( viewMgr );
    connect( viewMgr, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
             anApp, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  }

  PVGUI_ViewWindow* pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->getActiveView() );
  if ( !pvWnd ) {
    pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->createViewWindow() );
  }

  pvWnd->setShown( toShow );
  if ( toShow ) pvWnd->setFocus();
}

/*!
  \brief Slot to show help for proxy.
*/
void PVGUI_Module::showHelpForProxy( const QString& groupname, const QString& proxyname )
{
  pqHelpReaction::showProxyHelp(groupname, proxyname);
}


/*!
  \brief Slot to show the waiting state.
*/
void PVGUI_Module::onPreAccept()
{
  getApp()->desktop()->statusBar()->showMessage(tr("STB_PREACCEPT"));
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

/*!
  \brief Slot to show the ready state.
*/
void PVGUI_Module::onPostAccept()
{
  getApp()->desktop()->statusBar()->showMessage(tr("STB_POSTACCEPT"), 2000);
  QTimer::singleShot(0, this, SLOT(endWaitCursor()));
}

/*!
  \brief Slot to switch off wait cursor.
*/
void PVGUI_Module::endWaitCursor()
{
  QApplication::restoreOverrideCursor();
}

static void ParavisMessageOutput(QtMsgType type, const char *msg)
{
  switch(type)
    {
  case QtDebugMsg:
    vtkOutputWindow::GetInstance()->DisplayText(msg);
    break;
  case QtWarningMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
  case QtCriticalMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
  case QtFatalMsg:
    vtkOutputWindow::GetInstance()->DisplayErrorText(msg);
    break;
    }
}

/*!
  \brief Activate module.
  \param study current study
  \return \c true if activaion is done successfully or 0 to prevent
  activation on error
*/
bool PVGUI_Module::activateModule( SUIT_Study* study )
{
  myOldMsgHandler = qInstallMsgHandler(ParavisMessageOutput);
  
  SUIT_ExceptionHandler::addCleanUpRoutine( paravisCleanUp );

  storeCommonWindowsState();

  bool isDone = SalomeApp_Module::activateModule( study );
  if ( !isDone ) return false;

  showView( true );
  if ( mySourcesMenuId != -1 ) menuMgr()->show(mySourcesMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myFiltersMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myMacrosMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myToolbarsMenuId);
  setMenuShown( true );
  setToolShown( true );

  restoreDockWidgetsState();

  QMenu* aMenu = menuMgr()->findMenu( myRecentMenuId );
  if(aMenu) {
    QList<QAction*> anActns = aMenu->actions();
    for (int i = 0; i < anActns.size(); ++i) {
      QAction* a = anActns.at(i);
      if(a)
        a->setVisible(true);
    }
  }

  QList<QMenu*>::iterator it;
  for (it = myMenus.begin(); it != myMenus.end(); ++it) {
    QAction* a = (*it)->menuAction();
    if(a)
      a->setVisible(true);
  }

  if ( myRecentMenuId != -1 ) menuMgr()->show(myRecentMenuId);

  ClientFindOrCreateParavisComponent(PARAVIS::GetCStudy(this));

  return isDone;
}


/*!
  \brief Deactivate module.
  \param study current study
  \return \c true if deactivaion is done successfully or 0 to prevent
  deactivation on error
*/
bool PVGUI_Module::deactivateModule( SUIT_Study* study )
{
  MESSAGE("PARAVIS deactivation ...")

  QMenu* aMenu = menuMgr()->findMenu( myRecentMenuId );
  if(aMenu) {
    QList<QAction*> anActns = aMenu->actions();
    for (int i = 0; i < anActns.size(); ++i) {
      QAction* a = anActns.at(i);
      if(a)
        a->setVisible(false);
    }
  }

  QList<QMenu*>::iterator it;
  for (it = myMenus.begin(); it != myMenus.end(); ++it) {
    QAction* a = (*it)->menuAction();
    if(a)
      a->setVisible(false);
  }

  QList<QDockWidget*> aStreamingViews = application()->desktop()->findChildren<QDockWidget*>("pqStreamingControls");
  foreach(QDockWidget* aView, aStreamingViews) {
    if (!myDockWidgets.contains(aView))
      myDockWidgets[aView] = aView->isVisible();
  }

  /*if (pqImplementation::helpWindow) {
    pqImplementation::helpWindow->hide();
    }*/
  showView( false );
  // hide menus
  menuMgr()->hide(myRecentMenuId);
  menuMgr()->hide(mySourcesMenuId);
  menuMgr()->hide(myFiltersMenuId);
  menuMgr()->hide(myMacrosMenuId);
  menuMgr()->hide(myToolbarsMenuId);
  setMenuShown( false );
  setToolShown( false );

  saveDockWidgetsState();

  SUIT_ExceptionHandler::removeCleanUpRoutine( paravisCleanUp );

  if (myOldMsgHandler)
    qInstallMsgHandler(myOldMsgHandler);

  restoreCommonWindowsState();
  
  return SalomeApp_Module::deactivateModule( study );
}


/*!
  \brief Called when application is closed.

  Process finalize application functionality from ParaView in order to save server settings
  and nullify application pointer if the application is being closed.

  \param theApp application
*/
void PVGUI_Module::onApplicationClosed( SUIT_Application* theApp )
{
  pqApplicationCore::instance()->settings()->sync();
  int aAppsNb = SUIT_Session::session()->applications().size();
  if (aAppsNb == 1) {
    deleteTemporaryFiles();
    MyCoreApp->deleteLater();
  }
  CAM_Module::onApplicationClosed(theApp);
}


/*!
  \brief Deletes temporary files created during import operation from VISU
*/
void PVGUI_Module::deleteTemporaryFiles()
{
  foreach(QString aFile, myTemporaryFiles) {
    if (QFile::exists(aFile)) {
      QFile::remove(aFile);
    }
  }
}


/*!
  \brief Called when study is closed.

  Removes data model from the \a study.

  \param study study being closed
*/
void PVGUI_Module::studyClosed(SUIT_Study* study)
{
  clearParaviewState();

  SalomeApp_Module::studyClosed(study);
}

/*!
  \brief Called when study is opened.
*/
void PVGUI_Module::onModelOpened()
{
  _PTR(Study) studyDS = PARAVIS::GetCStudy(this);
  if(!studyDS) {
    return;
  }
  
  _PTR(SComponent) paravisComp = 
    studyDS->FindComponent(GetEngine()->ComponentDataType());
  if(!paravisComp) {
    return;
  }

  _PTR(ChildIterator) anIter(studyDS->NewChildIterator(paravisComp));
  for (; anIter->More(); anIter->Next()) {
    _PTR(SObject) aSObj = anIter->Value();
    _PTR(GenericAttribute) anAttr;
    if (!aSObj->FindAttribute(anAttr, "AttributeLocalID")) {
      continue;
    }
    _PTR(AttributeLocalID) anID(anAttr);
    if (anID->Value() == PVSTATEID) {
      myStateCounter++;
    }
  }
}

///*!
//  \brief Returns IOR of current engine
//*/
//QString PVGUI_Module::engineIOR() const
//{
//  CORBA::String_var anIOR = GetEngine()->GetIOR();
//  return QString(anIOR.in());
//}


/*!
  \brief Open file of format supported by ParaView
*/
void PVGUI_Module::openFile(const char* theName)
{
  QStringList aFiles;
  aFiles<<theName;
  pqLoadDataReaction::loadData(aFiles);
}

void PVGUI_Module::executeScript(const char *script)
{
#ifndef WNT
  pqPythonManager* manager = qobject_cast<pqPythonManager*>(
                             pqApplicationCore::instance()->manager("PYTHON_MANAGER"));
  if (manager)  {
    pqPythonDialog* pyDiag = manager->pythonShellDialog();
    if (pyDiag) {
      pyDiag->runString(script);  
      }
    }
#endif
}

///**
// *  Debug function printing out the given interpreter's execution context
// */
//void printInterpContext(PyInterp_Interp * interp )
//{
//  // Extract __smtraceString from interpreter's context
//  const PyObject* ctxt = interp->getExecutionContext();
//
//  PyObject* lst = PyDict_Keys((PyObject *)ctxt);
//  Py_ssize_t siz = PyList_GET_SIZE(lst);
//  for (Py_ssize_t i = 0; i < siz; i++)
//    {
//      PyObject * elem = PyList_GetItem(lst, i);
//      if (PyString_Check(elem))
//        {
//          std::cout << "At pos:" << i << ", " << PyString_AsString(elem) << std::endl;
//        }
//      else
//        std::cout << "At pos:" << i << ", not a string!" << std::endl;
//    }
//  Py_XDECREF(lst);
//}

/*!
  \brief Returns trace string
*/
static const QString MYReplaceStr("paraview.simple");
static const QString MYReplaceImportStr("except: from pvsimple import *");
QString PVGUI_Module::getTraceString()
{
  QString traceString;
#ifndef WNT
  {
    PyLockWrapper lck; // Acquire GIL

    const char * code = "from paraview import smtrace;"
                        "__smtraceString = smtrace.get_trace_string()";
    PyRun_SimpleString(code);
    // Now get the value of __smtraceString
    PyObject* main_module = PyImport_AddModule((char*)"__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* string_object = PyDict_GetItemString(global_dict, "__smtraceString");
    char* string_ptr = string_object ? PyString_AsString(string_object) : 0;
    if (string_ptr)  {
        traceString = string_ptr;
    }
  } // release GIL

  if ((!traceString.isNull()) && traceString.length() != 0) {
    int aPos = traceString.indexOf(MYReplaceStr);
    while (aPos != -1) {
      traceString = traceString.replace(aPos, MYReplaceStr.length(), "pvsimple");
      aPos = traceString.indexOf(MYReplaceStr, aPos);
    }
    int aImportPos = traceString.indexOf(MYReplaceImportStr);
    if(aImportPos != -1)
      {
      traceString = traceString.replace(aImportPos, MYReplaceImportStr.length(), "except:\n  import pvsimple\n  from pvsimple import *");
      }
  }
#endif
  return traceString;
}

/*!
  \brief Saves trace string to disk file
*/
void PVGUI_Module::saveTrace(const char* theName)
{
  QFile file(theName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    MESSAGE( "Could not open file:" << theName );
    return;
  }
  QTextStream out(&file);
  out << getTraceString();
  file.close();
}

/*!
  \brief Saves ParaView state to a disk file
*/
void PVGUI_Module::saveParaviewState(const char* theFileName)
{
  pqApplicationCore::instance()->saveState(theFileName);
}

/*!
  \brief Delete all objects for Paraview Pipeline Browser
*/
void PVGUI_Module::clearParaviewState()
{
  QAction* deleteAllAction = action(DeleteAllId);
  if (deleteAllAction) {
    deleteAllAction->activate(QAction::Trigger);
  }
}

/*!
  \brief Restores ParaView state from a disk file

  If toClear == true, the current ojects will be deleted
*/
void PVGUI_Module::loadParaviewState(const char* theFileName)
{
  pqApplicationCore::instance()->loadState(theFileName, getActiveServer());
}

/*!
  \brief Returns current active ParaView server
*/
pqServer* PVGUI_Module::getActiveServer()
{
  return pqApplicationCore::instance()->getActiveServer();
}


/*!
  \brief Creates PARAVIS preference pane 
*/
void PVGUI_Module::createPreferences()
{
  // Paraview settings tab
  int aParaViewSettingsTab = addPreference( tr( "TIT_PVIEWSETTINGS" ) );
  int aPanel = addPreference(QString(), aParaViewSettingsTab, LightApp_Preferences::UserDefined, "PARAVIS", "");
  setPreferenceProperty(aPanel, "content", (qint64)(new PVGUI_ParaViewSettingsPane()));

  // Paravis settings tab
  int aParaVisSettingsTab = addPreference( tr( "TIT_PVISSETTINGS" ) );
  addPreference( tr( "PREF_STOP_TRACE" ), aParaVisSettingsTab, LightApp_Preferences::Bool, "PARAVIS", "stop_trace");

  int aSaveType = addPreference(tr( "PREF_SAVE_TYPE_LBL" ), aParaVisSettingsTab,
                                LightApp_Preferences::Selector,
                                "PARAVIS", "savestate_type");
  QList<QVariant> aIndices;
  QStringList aStrings;
  aIndices<<0<<1<<2;
  aStrings<<tr("PREF_SAVE_TYPE_0");
  aStrings<<tr("PREF_SAVE_TYPE_1");
  aStrings<<tr("PREF_SAVE_TYPE_2");
  setPreferenceProperty(aSaveType, "strings", aStrings);
  setPreferenceProperty(aSaveType, "indexes", aIndices);

  //rnv: imp 21712: [CEA 581] Preference to display legend by default 
  int aDispColoreLegend = addPreference( tr( "PREF_SHOW_COLOR_LEGEND" ), aParaVisSettingsTab,
                                        LightApp_Preferences::Bool, "PARAVIS", "show_color_legend");
}

/*!
  \brief Creates ParaViS context menu popup
*/
void PVGUI_Module::contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle)
{
  SalomeApp_Module::contextMenuPopup(theClient, theMenu, theTitle);
  
  // Check if we are in Object Browser
  SUIT_DataBrowser* ob = getApp()->objectBrowser();
  bool isOBClient = (ob && theClient == ob->popupClientType());
  if (!isOBClient) {
    return;
  }

  // Get list of selected objects
  LightApp_SelectionMgr* aSelectionMgr = getApp()->selectionMgr();
  SALOME_ListIO aListIO;
  aSelectionMgr->selectedObjects(aListIO);
  if (aListIO.Extent() == 1 && aListIO.First()->hasEntry()) {
    QString entry = QString(aListIO.First()->getEntry());
    
    // Get active study
    SalomeApp_Study* activeStudy = 
      dynamic_cast<SalomeApp_Study*>(getApp()->activeStudy());
    if(!activeStudy) {
      return;
    }

    // Get SALOMEDS client study 
    _PTR(Study) studyDS = activeStudy->studyDS();
    if(!studyDS) {
      return;
    }

    QString paravisDataType(GetEngine()->ComponentDataType());
    if(activeStudy && activeStudy->isComponent(entry) && 
       activeStudy->componentDataType(entry) == paravisDataType) {
      // ParaViS module object
      theMenu->addSeparator();
      theMenu->addAction(action(SaveStatePopupId));
    }
    else {
      // Try to get state object
      _PTR(SObject) stateSObj = 
          studyDS->FindObjectID(entry.toLatin1().constData());
      if (!stateSObj) {
          return;
      }
      
      // Check local id
      _PTR(GenericAttribute) anAttr;
      if (!stateSObj->FindAttribute(anAttr, "AttributeLocalID")) {
          return;
      }

      _PTR(AttributeLocalID) anID(anAttr);
      
      if (anID->Value() == PVSTATEID) {
        // Paraview state object
        theMenu->addSeparator();
        theMenu->addAction(action(AddStatePopupId));
        theMenu->addAction(action(CleanAndAddStatePopupId));
        theMenu->addSeparator();
        theMenu->addAction(action(ParaVisRenameId));
        theMenu->addAction(action(ParaVisDeleteId));
      }
    }
  }
}

/*!
  \brief. Show ParaView python trace.
*/
void PVGUI_Module::onShowTrace()
{
  if (!myTraceWindow) {
    myTraceWindow = new pqPythonScriptEditor(getApp()->desktop());
  }
  myTraceWindow->setText(getTraceString());
  myTraceWindow->show();
  myTraceWindow->raise();
  myTraceWindow->activateWindow();
}


/*!
  \brief. Re-initialize ParaView python trace.
*/
void PVGUI_Module::onRestartTrace()
{
  QString script = "from paraview import smtrace\n";
  script += "smtrace.stop_trace()\n";
  script += "smtrace.start_trace()\n";
  execPythonCommand(script, false);
}

/*!
  \brief Show ParaView view.
*/
void PVGUI_Module::onNewParaViewWindow()
{
  showView(true);
}

/*!
  \brief Save state under the module root object.
*/
void PVGUI_Module::onSaveMultiState()
{
  // Create state study object
  
  // Get SALOMEDS client study
  _PTR(Study) studyDS = PARAVIS::GetCStudy(this);
  if(!studyDS) {
    return;
  }
  
  _PTR(SComponent) paravisComp = 
    studyDS->FindComponent(GetEngine()->ComponentDataType());
  if(!paravisComp) {
    return;
  }

  // Unlock the study if it is locked
  bool isLocked = studyDS->GetProperties()->IsLocked();
  if (isLocked) {
    studyDS->GetProperties()->SetLocked(false);
  }
  
  QString stateName = tr("SAVED_PARAVIEW_STATE_NAME") + 
    QString::number(myStateCounter + 1);

  _PTR(StudyBuilder) studyBuilder = studyDS->NewBuilder();
  _PTR(SObject) newSObj = studyBuilder->NewObject(paravisComp);

  // Set name
  _PTR(GenericAttribute) anAttr;
  anAttr = studyBuilder->FindOrCreateAttribute(newSObj, "AttributeName");
  _PTR(AttributeName) nameAttr(anAttr);
  
  nameAttr->SetValue(stateName.toLatin1().constData());

  // Set local id
  anAttr = studyBuilder->FindOrCreateAttribute(newSObj, "AttributeLocalID");
  _PTR(AttributeLocalID) localIdAttr(anAttr);
  
  localIdAttr->SetValue(PVSTATEID);

  // Set file name
  QString stateEntry = QString::fromStdString(newSObj->GetID());
 
  // File name for state saving
  QString tmpDir = QString::fromStdString(SALOMEDS_Tool::GetTmpDir());
  QString fileName = QString("%1_paravisstate:%2").arg(tmpDir, stateEntry);

  anAttr = studyBuilder->FindOrCreateAttribute(newSObj, "AttributeString");
  _PTR(AttributeString) stringAttr(anAttr);
  
  stringAttr->SetValue(fileName.toLatin1().constData());

  // Lock the study back if necessary
  if (isLocked) {
    studyDS->GetProperties()->SetLocked(true);
  }
  
  // Save state
  saveParaviewState(fileName.toLatin1().constData());
  myTemporaryFiles.append(fileName);
  
  // Increment the counter
  myStateCounter++;

  updateObjBrowser();
}

/*!
  \brief Restore the selected state by merging with the current one.
*/
void PVGUI_Module::onAddState()
{
  loadSelectedState(false);
}

/*!
  \brief Clean the current state and restore the selected one.
*/
void PVGUI_Module::onCleanAddState()
{
  loadSelectedState(true);
}

/*!
  \brief Rename the selected object.
*/
void PVGUI_Module::onRename()
{
  LightApp_SelectionMgr* aSelectionMgr = getApp()->selectionMgr();
  SALOME_ListIO aListIO;
  aSelectionMgr->selectedObjects(aListIO);
  
  if (aListIO.Extent() == 1 && aListIO.First()->hasEntry()) {
    std::string entry = aListIO.First()->getEntry();
    
    // Get SALOMEDS client study 
    _PTR(Study) studyDS = PARAVIS::GetCStudy(this);
    if(!studyDS) {
      return;
    }
    
    // Unlock the study if it is locked
    bool isLocked = studyDS->GetProperties()->IsLocked();
    if (isLocked) {
      studyDS->GetProperties()->SetLocked(false);
    }
    
    // Rename the selected state object
    _PTR(SObject) stateSObj = studyDS->FindObjectID(entry);
    if (!stateSObj) {
      return;
    }
    
    _PTR(GenericAttribute) anAttr;
    if (stateSObj->FindAttribute(anAttr, "AttributeName")) {
      _PTR(AttributeName) nameAttr (anAttr);
      QString newName = 
          LightApp_NameDlg::getName(getApp()->desktop(), nameAttr->Value().c_str());
      if (!newName.isEmpty()) {
        nameAttr->SetValue(newName.toLatin1().constData());
        aListIO.First()->setName(newName.toLatin1().constData());
      }
    }
    
    // Lock the study back if necessary
    if (isLocked) {
      studyDS->GetProperties()->SetLocked(true);
    }
    
    // Update object browser
    updateObjBrowser();
    
  }
}

/*!
  \brief Delete the selected objects.
*/
void PVGUI_Module::onDelete()
{
  LightApp_SelectionMgr* aSelectionMgr = getApp()->selectionMgr();
  SALOME_ListIO aListIO;
  aSelectionMgr->selectedObjects(aListIO);
  
  if (aListIO.Extent() == 1 && aListIO.First()->hasEntry()) {
    std::string entry = aListIO.First()->getEntry();
    
    // Get SALOMEDS client study 
    _PTR(Study) studyDS = PARAVIS::GetCStudy(this);
    if(!studyDS) {
      return;
    }
    
    // Unlock the study if it is locked
    bool isLocked = studyDS->GetProperties()->IsLocked();
    if (isLocked) {
      studyDS->GetProperties()->SetLocked(false);
    }
    
    // Remove the selected state from the study
    _PTR(StudyBuilder) studyBuilder = studyDS->NewBuilder();
    _PTR(SObject) stateSObj = studyDS->FindObjectID(entry);
    studyBuilder->RemoveObject(stateSObj);
    
    // Lock the study back if necessary
    if (isLocked) {
      studyDS->GetProperties()->SetLocked(true);
    }
    
    // Update object browser
    updateObjBrowser();
  }
}

void PVGUI_Module::onPushTraceTimer()
{
//  MESSAGE("onPushTraceTimer(): Pushing trace to engine...");
  GetEngine()->PutPythonTraceStringToEngine(getTraceString().toStdString().c_str());
}

/*!
  \brief Discover help project files from the resources.
  \return name of the help file. 
*/
QString PVGUI_Module::getHelpFileName() {
  QString aPVHome(getenv("PVHOME"));
  if (aPVHome.isNull()) {
    qWarning("Wariable PVHOME is not defined");
    return QString();
  }
  QChar aSep = QDir::separator();
  //PARAVIEW_VERSION from the vtkPVConfig.h file
  QString aFileName =  aPVHome + aSep + "share" + aSep + "doc" + aSep + "paraview-"+ PARAVIEW_VERSION + aSep + "paraview.qch";
  return aFileName;
}


/*!
  \brief Load selected paraview state

  If toClear == true, the current state will be cleared
*/
void PVGUI_Module::loadSelectedState(bool toClear)
{
  QString fileName;

  LightApp_SelectionMgr* aSelectionMgr = getApp()->selectionMgr();
  SALOME_ListIO aListIO;
  aSelectionMgr->selectedObjects(aListIO);
  
  if (aListIO.Extent() == 1 && aListIO.First()->hasEntry()) {
    std::string entry = aListIO.First()->getEntry();
    
    // Get SALOMEDS client study 
    _PTR(Study) studyDS = PARAVIS::GetCStudy(this);
    if(!studyDS) {
      return;
    }

    // Check local id
    _PTR(SObject) stateSObj = studyDS->FindObjectID(entry);
    _PTR(GenericAttribute) anAttr;
    if (!stateSObj->FindAttribute(anAttr, "AttributeLocalID")) {
      return;
    }
    _PTR(AttributeLocalID) anID(anAttr);
    if (!anID->Value() == PVSTATEID) {
      return;
    }

    // Get state file name
    if (stateSObj->FindAttribute(anAttr, "AttributeString")) {
      _PTR(AttributeString) aStringAttr(anAttr);
      QString stringValue(aStringAttr->Value().c_str());

      if (QFile::exists(stringValue)) {
          fileName = stringValue;
      }
    }
  }
  
  if (!fileName.isEmpty()) {
    if (toClear) {
      clearParaviewState();
    }

    loadParaviewState(fileName.toLatin1().constData());
  } 
  else {
    SUIT_MessageBox::critical(getApp()->desktop(),
                              tr("ERR_ERROR"),
                              tr("ERR_STATE_CANNOT_BE_RESTORED"));
  }
}

void PVGUI_Module::onRepresentationChanged(pqRepresentation*) {


  //rnv: to fix the issue "21712: [CEA 581] Preference to display legend by default"
  //     find the pqDisplayColorWidget instances and connect the variableChanged SIGNAL on the 
  //     onVariableChanged slot of this class. This connection needs to change visibility 
  //     of the "Colored Legend" after change the "Color By" array.
  QList<pqDisplayColorWidget*> aWidget = getApp()->desktop()->findChildren<pqDisplayColorWidget*>();
  
  for (int i = 0; i < aWidget.size() ; i++ ) {
    if( aWidget[i] ) {
      connect( aWidget[i], SIGNAL ( variableChanged ( pqVariableType, const QString ) ), 
              this, SLOT(onVariableChanged( pqVariableType, const QString) ), Qt::UniqueConnection );
    }    
  }
}


/*!
  \fn CAM_Module* createModule();
  \brief Export module instance (factory function).
  \return new created instance of the module
*/

#ifdef WNT
#define PVGUI_EXPORT __declspec(dllexport)
#else   // WNT
#define PVGUI_EXPORT
#endif  // WNT

extern "C" {

  bool flag = false;
  PVGUI_EXPORT CAM_Module* createModule() {
    if(!flag) {
        vtkEDFHelperInit();
        flag = true;
    }      
    return new PVGUI_Module();
  }
  
  PVGUI_EXPORT char* getModuleVersion() {
    return (char*)PARAVIS_VERSION_STR;
  }
}
