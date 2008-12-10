// PARAVIS : ParaView wrapper SALOME module
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//

#include "PVGUI_Module.h"
#include "PVGUI_Module_impl.h"
#include "PVGUI_ProcessModuleHelper.h"
#include "PVGUI_ViewModel.h"
#include "PVGUI_ViewManager.h"
#include "PVGUI_ViewWindow.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
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
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QToolBar>

#include <pqApplicationCore.h>
#include <pqActiveServer.h>
#include <pqActiveView.h>
#include <pqClientAboutDialog.h>
#include <pqObjectBuilder.h>
#include <pqOptions.h>
#include <pqRenderView.h>
#include <pqRubberBandHelper.h>
#include <pqServer.h>
#include <pqServerManagerModel.h>
#include <pqServerResource.h>
#include <pqUndoStack.h>
#include <pqVCRController.h>
#include <pqViewManager.h>
#include <vtkPVMain.h>
#include <vtkProcessModule.h>

/*
 * Make sure all the kits register their classes with vtkInstantiator.
 * Since ParaView uses Tcl wrapping, all of VTK is already compiled in
 * anyway.  The instantiators will add no more code for the linker to
 * collect.
 */

#include <vtkCommonInstantiator.h>
#include <vtkFilteringInstantiator.h>
#include <vtkGenericFilteringInstantiator.h>
#include <vtkIOInstantiator.h>
#include <vtkImagingInstantiator.h>
#include <vtkInfovisInstantiator.h>
#include <vtkGraphicsInstantiator.h>

#include <vtkRenderingInstantiator.h>
#include <vtkVolumeRenderingInstantiator.h>
#include <vtkHybridInstantiator.h>
#include <vtkParallelInstantiator.h>

#include <vtkPVServerCommonInstantiator.h>
#include <vtkPVFiltersInstantiator.h>
#include <vtkPVServerManagerInstantiator.h>
#include <vtkClientServerInterpreter.h>


//----------------------------------------------------------------------------
// ClientServer wrapper initialization functions.
// Taken from ParaView sources (file pqMain.cxx)
extern "C" void vtkCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGenericFilteringCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkImagingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkInfovisCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkGraphicsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkIOCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkVolumeRenderingCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkHybridCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkWidgetsCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkParallelCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVServerCommonCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkPVFiltersCS_Initialize(vtkClientServerInterpreter*);
extern "C" void vtkXdmfCS_Initialize(vtkClientServerInterpreter *);

//----------------------------------------------------------------------------
void ParaViewInitializeInterpreter(vtkProcessModule* pm)
{
  // Initialize built-in wrapper modules.
  vtkCommonCS_Initialize(pm->GetInterpreter());
  vtkFilteringCS_Initialize(pm->GetInterpreter());
  vtkGenericFilteringCS_Initialize(pm->GetInterpreter());
  vtkImagingCS_Initialize(pm->GetInterpreter());
  vtkInfovisCS_Initialize(pm->GetInterpreter());
  vtkGraphicsCS_Initialize(pm->GetInterpreter());
  vtkIOCS_Initialize(pm->GetInterpreter());
  vtkRenderingCS_Initialize(pm->GetInterpreter());
  vtkVolumeRenderingCS_Initialize(pm->GetInterpreter());
  vtkHybridCS_Initialize(pm->GetInterpreter());
  vtkWidgetsCS_Initialize(pm->GetInterpreter());
  vtkParallelCS_Initialize(pm->GetInterpreter());
  vtkPVServerCommonCS_Initialize(pm->GetInterpreter());
  vtkPVFiltersCS_Initialize(pm->GetInterpreter());
  vtkXdmfCS_Initialize(pm->GetInterpreter());
}

vtkPVMain*                 PVGUI_Module::pqImplementation::myPVMain = 0;
pqOptions*                 PVGUI_Module::pqImplementation::myPVOptions = 0;
PVGUI_ProcessModuleHelper* PVGUI_Module::pqImplementation::myPVHelper = 0;

/*!
  \mainpage

  <h2>Building and installing PARAVIS</h2>
  As any other SALOME module, PARAVIS requires PARAVIS_ROOT_DIR environment variable to be set to PARAVIS
  installation directory.
  Other variables needed for correct detection of ParaView location:
  \li PVSRCHOME - points at the root of ParaView source directory tree
  \li PVINSTALLHOME - points at the top of ParaView build tree (currently, due to some drawbacks in its buld procedure
  ParaView should not be installed, its build directory is used instead).

  It also requires common SALOME environment including GUI_ROOT_DIR and other prerequsites.

  As soon as the environment is set, excute the following commands in a shell:
  \code
  mkdir PARAVIS_BUILD
  cd PARAVIS_BUILD
  ../PARAVIS_SRC/build_configure
  ../PARAVIS_SRC/configure --prefix=${PARAVIS_ROOT_DIR}
  make 
  make docs
  make install
  \endcode

  PARAVIS module can be launched using the following commands:
  \li Light SALOME configuration
  \code
  runLightSalome.sh --modules="PARAVIS"
  \endcode
  or
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
  It simulates actions perfomed by pqMain::Run( QApplication&, pqProcessModuleGUIHelper* ) method.
  
  Custom PVGUI_ProcessModuleHelper class derived from %pqProcessModuleGUIHelper base is the main actor in ParaView 
  client initialization. It initializes the client that uses the main window (SALOME desktop) supplied from the outside, 
  it does not start Qt event loop as this is done in SALOME GUI executable (SUITApp or SALOME_Session_Server), and after all 
  it redirects ParaView diagnostic output to SALOME LogWindow with help of PVGUI_OutputWindowAdapter class.
  This is achieved by reimplementing \link PVGUI_ProcessModuleHelper::InitializeApplication() InitializeApplication()\endlink,
  \link PVGUI_ProcessModuleHelper::appExec() appExec()\endlink, \link PVGUI_ProcessModuleHelper::postAppExec() postAppExec()\endlink
  virtual methods as well as those responsible for main window management.

  <h3>ParaView GUI connection to SALOME desktop</h3>

  ParaView Qt components include pqMainWindowCore class that handles most ParaView GUI client actions,
  updates menu states and connects many GUI components.
  After the client initalization \link PVGUI_Module::initialize() PVGUI_Module::initialize()\endlink method creates
  an instance of internal PVGUI_Module::pqImplementation class (declared PVGUI_Module_impl.h) that wraps some ParaView GUI components: 
  pqMainWindowCore, pqServer, etc. Instance of SALOME desktop widget is passed to pqMainWindowCore instead of ParaView main window.

  Basically it simulates constructor of ParaView client's %MainWindow class (src/Applications/Client/MainWindow.cxx). It creates the same
  menu and toolbar commands using SALOME menu and toolbar managers, connecting the actions to %pqMainWindowCore slots or to the module's 
  slots in some cases. It also sets up dock widgets for ParaView widgets, such as object inspector, pipeline browser, etc.

  As both SALOME destop and ParaView main winodw classes inherit QMainWindow and %pqMainWindowCore deals with QMainWindow API to control
  menus, tollbars and dock widgets, its integration into SALOME GUI is straightforward and smooth.

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

  <h2>Limitations of 2008 year prototype</h2>
  \li SALOME persistence (possibility to save the module data into a tsudy file) is not implemented for PARAVIS module.
  \li As a light module, PARAVIS does not have a CORBA engine that follows SALOME rules, however PARAVIS can use load a CORBA engine 
  on its own if needed.
*/

/*
  A link to the file documentation PVGUI_Module_actions.cxx

  A link to the file documentation PVGUI_Module_impl.h

  A link to the class documentation PVGUI_Module

  The class name as a text (not a link to its documentation) %PVGUI_Module

  A link to the constructor of the class PVGUI_ViewWindow#PVGUI_ViewWindow or PVGUI_Module::PVGUI_Module()

  A link to the destructor of the class PVGUI_ViewWindow#~PVGUI_ViewWindow

  A link to the member function PVGUI_ViewWindow::getVisualParameters or PVGUI_ViewWindow#getVisualParameters

  Arguments of a function should be specified only for the overloaded functions PVGUI_Module::initialize(CAM_Application*)

  A link to some enumeration values PVGUI_Module::OpenFileId and PVGUI_Module::CreateLookmarkId

  A link to a protected member variable of the class PVGUI_ViewWindow#myModel

  A link to a variable \link PVGUI_ViewWindow#myModel using another text\endlink as a link

*/

/*!
  \class PVGUI_Module
  \brief Implementation of light (no-CORBA-engine) 
         SALOME module wrapping ParaView GUI.
*/

/*!
  \brief Constructor. Sets the default name for the module.
*/
PVGUI_Module::PVGUI_Module()
  : LightApp_Module( "PARAVIS" ),
    Implementation( 0 ),
    mySelectionControlsTb( -1 ),
    mySourcesMenuId( -1 ),
    myFiltersMenuId( -1 )
{
}

/*!
  \brief Destructor.
*/
PVGUI_Module::~PVGUI_Module()
{
}

/*!
  \brief Initialize module. Creates menus, prepares context menu, etc.
  \param app SALOME GUI application instance
*/
void PVGUI_Module::initialize( CAM_Application* app )
{
  LightApp_Module::initialize( app );

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
  if ( !Implementation ){
    LightApp_Application* anApp = getApp();

    // Simulate ParaView client main window
    Implementation = new pqImplementation( anApp->desktop() );

    setupDockWidgets();
    
    pvCreateActions();
    pvCreateMenus();
    pvCreateToolBars();
    
    setupDockWidgetsContextMenu();

    // Now that we're ready, initialize everything ...
    Implementation->Core.initializeStates();
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
  \brief Static method, performs initialization of ParaView session.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVGUI_Module::pvInit()
{
  if ( !pqImplementation::myPVMain ){
    // Obtain command-line arguments
    int argc = 0;
    QStringList args = QApplication::arguments();
    char** argv = new char*[args.size()];
    for ( QStringList::const_iterator it = args.begin(); argc < 1 && it != args.end(); it++, argc++ )
      argv[argc] = strdup( (*it).toLatin1().constData() );

    vtkPVMain::SetInitializeMPI(0);  // pvClient never runs with MPI.
    vtkPVMain::Initialize(&argc, &argv); // Perform any initializations.

    // TODO: Set plugin dir from preferences
    //QApplication::setLibraryPaths(QStringList(dir.absolutePath()));

    pqImplementation::myPVMain = vtkPVMain::New();
    if ( !pqImplementation::myPVOptions )
      pqImplementation::myPVOptions = pqOptions::New();
    if ( !pqImplementation::myPVHelper )
      pqImplementation::myPVHelper = PVGUI_ProcessModuleHelper::New();

    pqImplementation::myPVOptions->SetProcessType(vtkPVOptions::PVCLIENT);

    // This creates the Process Module and initializes it.
    int ret = pqImplementation::myPVMain->Initialize(pqImplementation::myPVOptions, 
                                                     pqImplementation::myPVHelper, 
                                                     ParaViewInitializeInterpreter,
                                                     argc, argv);
    if (!ret){
      // Tell process module that we support Multiple connections.
      // This must be set before starting the event loop.
      vtkProcessModule::GetProcessModule()->SupportMultipleConnectionsOn();
      ret = pqImplementation::myPVHelper->Run(pqImplementation::myPVOptions);
    }

    delete[] argv;
    return !ret;
  }
  
  return true;
}
 
/*!
  \brief Static method, cleans up ParaView session at application exit. Not yet implemented.
*/
void PVGUI_Module::pvShutdown()
{
  // TODO...
}  

/*!
  \brief Shows (toShow = true) or hides ParaView view window
*/
void PVGUI_Module::showView( bool toShow )
{
  LightApp_Application* anApp = getApp();
  PVGUI_ViewManager* viewMgr = dynamic_cast<PVGUI_ViewManager*>( anApp->getViewManager( PVGUI_Viewer::Type(), false ) );
  if ( !viewMgr ) {
    viewMgr = new PVGUI_ViewManager( anApp->activeStudy(), anApp->desktop() );
    anApp->addViewManager( viewMgr );
    connect( viewMgr, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
             anApp, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  }

  PVGUI_ViewWindow* pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->getActiveView() );
  if ( !pvWnd ) {
    pvWnd = dynamic_cast<PVGUI_ViewWindow*>( viewMgr->createViewWindow() );
    pvWnd->setMultiViewManager( &Implementation->Core.multiViewManager() );
  }

  pvWnd->setShown( toShow );
}

/*!
  \brief Manage the label of Undo operation.
*/
void PVGUI_Module::onUndoLabel( const QString& label )
{
  action(UndoId)->setText(
    label.isEmpty() ? tr("MEN_CANTUNDO") : QString(tr("MEN_UNDO_ACTION")).arg(label));
  action(UndoId)->setStatusTip(
    label.isEmpty() ? tr("MEN_CANTUNDO") : QString(tr("MEN_UNDO_ACTION_TIP")).arg(label));
}

/*!
  \brief Manage the label of Redo operation.
*/
void PVGUI_Module::onRedoLabel( const QString& label )
{
  action(RedoId)->setText(
    label.isEmpty() ? tr("MEN_CANTREDO") : QString(tr("MEN_REDO_ACTION")).arg(label));
  action(RedoId)->setStatusTip(
    label.isEmpty() ? tr("MEN_CANTREDO") : QString(tr("MEN_REDO_ACTION_TIP")).arg(label));
}

/*!
  \brief Manage the label of Undo Camera operation.
*/
void PVGUI_Module::onCameraUndoLabel( const QString& label )
{
  action(CameraUndoId)->setText(
    label.isEmpty() ? tr("MEN_CANT_CAMERA_UNDO") : QString(tr("MEN_CAMERA_UNDO_ACTION")).arg(label));
  action(CameraUndoId)->setStatusTip(
    label.isEmpty() ? tr("MEN_CANT_CAMERA_UNDO") : QString(tr("MEN_CAMERA_UNDO_ACTION_TIP")).arg(label));
}

/*!
  \brief Manage the label of Redo Camera operation.
*/
void PVGUI_Module::onCameraRedoLabel( const QString& label )
{
  action(CameraRedoId)->setText(
    label.isEmpty() ? tr("MEN_CANT_CAMERA_REDO") : QString(tr("MEN_CAMERA_REDO_ACTION")).arg(label));
  action(CameraRedoId)->setStatusTip(
    label.isEmpty() ? tr("MEN_CANT_CAMERA_REDO") : QString(tr("MEN_CAMERA_REDO_ACTION_TIP")).arg(label));
}

/*!
  \brief Slot to delete all objects.
*/
void PVGUI_Module::onDeleteAll()
{
  pqObjectBuilder* builder = pqApplicationCore::instance()->getObjectBuilder();
  Implementation->Core.getApplicationUndoStack()->beginUndoSet("Delete All");
  builder->destroyPipelineProxies();
  Implementation->Core.getApplicationUndoStack()->endUndoSet();
}

/*!
  \brief Slot to check/uncheck the action for corresponding selection mode.
*/
void PVGUI_Module::onSelectionModeChanged( int mode )
{
  if( toolMgr()->toolBar( mySelectionControlsTb )->isEnabled() ) {
    if(mode == pqRubberBandHelper::SELECT) //surface selection
      action(SelectCellsOnId)->setChecked(true);
    else if(mode == pqRubberBandHelper::SELECT_POINTS) //surface selection
      action(SelectPointsOnId)->setChecked(true);
    else if(mode == pqRubberBandHelper::FRUSTUM)
      action(SelectCellsThroughId)->setChecked(true);
    else if(mode == pqRubberBandHelper::FRUSTUM_POINTS)
      action(SelectPointsThroughId)->setChecked(true);
    else if (mode == pqRubberBandHelper::BLOCKS)
      action(SelectBlockId)->setChecked(true);
    else // INTERACT
      action(InteractId)->setChecked(true);
  }
}

/*!
  \brief Slot to manage the change of axis center.
*/
void PVGUI_Module::onShowCenterAxisChanged( bool enabled )
{
  action(ShowCenterId)->setEnabled(enabled);
  action(ShowCenterId)->blockSignals(true);
  pqRenderView* renView = qobject_cast<pqRenderView*>(
    pqActiveView::instance().current());
  action(ShowCenterId)->setChecked( renView ? renView->getCenterAxesVisibility() : false);
  action(ShowCenterId)->blockSignals(false);
}

/*!
  \brief Slot to set tooltips for the first anf the last frames, i.e. a time range of animation.
*/
void PVGUI_Module::setTimeRanges( double start, double end )
{
  action(FirstFrameId)->setToolTip(QString("First Frame (%1)").arg(start, 0, 'g'));
  action(LastFrameId)->setToolTip(QString("Last Frame (%1)").arg(end, 0, 'g'));
}

/*!
  \brief Slot to manage the plaing process of animation.
*/
void PVGUI_Module::onPlaying( bool playing )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if(playing) {
    disconnect( action(PlayId),                        SIGNAL( triggered() ),
		&Implementation->Core.VCRController(), SLOT( onPlay() ) );
    connect( action(PlayId),                        SIGNAL( triggered() ),
	     &Implementation->Core.VCRController(), SLOT( onPause() ) );
    action(PlayId)->setIcon(QIcon(resMgr->loadPixmap("ParaView",tr("ICON_PAUSE"),false)));
    action(PlayId)->setText("Pa&use");
  }
  else {
    connect( action(PlayId),                        SIGNAL( triggered() ),
	     &Implementation->Core.VCRController(), SLOT( onPlay() ) );
    disconnect( action(PlayId),                        SIGNAL( triggered() ),
		&Implementation->Core.VCRController(), SLOT( onPause() ) );
    action(PlayId)->setIcon(QIcon(resMgr->loadPixmap("ParaView",tr("ICON_PLAY"),false)));
    action(PlayId)->setText("&Play");
  }

  Implementation->Core.setSelectiveEnabledState(!playing);
}

/*!
  \brief Slot to add camera link.
*/
void PVGUI_Module::onAddCameraLink()
{
  pqView* vm = pqActiveView::instance().current();
  pqRenderView* rm = qobject_cast<pqRenderView*>(vm);
  if(rm) rm->linkToOtherView();
  else SUIT_MessageBox::warning(getApp()->desktop(),
				tr("WARNING"), tr("WRN_ADD_CAMERA_LINK"));
}

/*!
  \brief Slot to show information about ParaView.
*/
void PVGUI_Module::onHelpAbout()
{
  pqClientAboutDialog* const dialog = new pqClientAboutDialog(getApp()->desktop());
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();
}

/*!
  \brief Slot to show help for proxy.
*/
void PVGUI_Module::showHelpForProxy( const QString& proxy )
{
  // make sure assistant is ready
  this->makeAssistant();

  if(this->Implementation->AssistantClient) {
    this->Implementation->AssistantClient->openAssistant();
    QString page("%1/Documentation/%2.html");
    page = page.arg(this->Implementation->DocumentationDir);
    page = page.arg(proxy);
    this->Implementation->AssistantClient->showPage(page);
  }
}

QString Locate( const QString& appName )
{
  QString app_dir = QCoreApplication::applicationDirPath();
  const char* inst_dirs[] = {
    "/./",
    "/../bin/",
    "/../../bin/",
    0
  };
  for (const char** dir = inst_dirs; *dir; ++dir) {
    QString path = app_dir;
    path += *dir;
    path += appName;
    //cout << "Checking : " << path.toAscii().data() << " ... ";
    //cout.flush();
    QFileInfo finfo (path);
    if (finfo.exists()) {
      //cout << " Success!" << endl;
      return path;
    }
    //cout << " Failed" << endl;
  }
  return app_dir + QDir::separator() + appName;
}

/*!
  \brief Initialized an assistant client.
*/
void PVGUI_Module::makeAssistant()
{
  if(this->Implementation->AssistantClient)
    return;
  
  QString assistantExe;
  QString profileFile;
  
  const char* assistantName = "assistant";
#ifdef WNT
  const char* binDir = "\\";
  const char* binDir1 = "\\..\\";
#else
  const char* binDir = "/bin/";
  const char* binDir1 = "/bin/bin/";
#endif

  QString helper = QString(getenv("PVHOME")) + binDir + QString("pqClientDocFinder.txt");
  if(!QFile::exists(helper))
    helper = QString(getenv("PVHOME")) + binDir1 + QString("pqClientDocFinder.txt");
  if(QFile::exists(helper)) {
    QFile file(helper);
    if(file.open(QIODevice::ReadOnly)) {
      assistantExe = file.readLine().trimmed();
      profileFile = file.readLine().trimmed();
      // CMake escapes spaces, we need to unescape those.
      assistantExe.replace("\\ ", " ");
      profileFile.replace("\\ ", " ");
    }
  }

  if(assistantExe.isEmpty()) {
    assistantExe = ::Locate(assistantName);//assistantExe = ::Locate(assistantProgName);
    /*
    QString assistant = QCoreApplication::applicationDirPath();
    assistant += QDir::separator();
    assistant += assistantName;
    assistantExe = assistant;
    */
  }

  this->Implementation->AssistantClient = new QAssistantClient(assistantExe, this);
  QObject::connect(this->Implementation->AssistantClient, SIGNAL(error(const QString&)),
                   this,                                  SLOT(assistantError(const QString&)));

  QStringList args;
  args.append(QString("-profile"));

  if(profileFile.isEmpty()) {
    // see if help is bundled up with the application
    QString profile = ::Locate("pqClient.adp");
    /*QCoreApplication::applicationDirPath() + QDir::separator()
      + QString("pqClient.adp");*/
    
    if(QFile::exists(profile))
      profileFile = profile;
  }

  if(profileFile.isEmpty() && getenv("PARAVIEW_HELP")) {
    // not bundled, ask for help
    args.append(getenv("PARAVIEW_HELP"));
  }
  else if(profileFile.isEmpty()) {
    // no help, error out
    SUIT_MessageBox::critical(getApp()->desktop(),"Help error", "Couldn't find"
			      " pqClient.adp.\nTry setting the PARAVIEW_HELP environment variable which"
			      " points to that file");
    delete this->Implementation->AssistantClient;
    return;
  }

  QFileInfo fi(profileFile);
  this->Implementation->DocumentationDir = fi.absolutePath();

  args.append(profileFile);

  this->Implementation->AssistantClient->setArguments(args);
}

/*!
  \brief Slot to call the message handler with the critical message.
*/
void PVGUI_Module::assistantError( const QString& error )
{
  qCritical(error.toAscii().data());
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

/*!
  \brief Returns the ParaView multi-view manager.
*/
pqViewManager* PVGUI_Module::getMultiViewManager() const
{
  pqViewManager* aMVM = 0; 
  if ( Implementation )
    aMVM = &Implementation->Core.multiViewManager();
  return aMVM;
}

/*!
  \brief Processes QEvent::ActionAdded and QEvent::ActionRemoved from Lookmarks toolbar
  in order to register/unregister this action in/from QtxActionToolMgr.
*/
bool PVGUI_Module::eventFilter( QObject* theObject, QEvent* theEvent )
{
  QToolBar* aTB = toolMgr()->toolBar(tr("TOOL_LOOKMARKS"));
  if ( theObject == aTB ) {
    
    if ( theEvent->type() == QEvent::ActionAdded ) {
      QList<QAction*> anActns = aTB->actions();
      for (int i = 0; i < anActns.size(); ++i)
	if ( toolMgr()->actionId(anActns.at(i)) == -1 ) {
	  toolMgr()->setUpdatesEnabled(false);
	  createTool( anActns.at(i), tr("TOOL_LOOKMARKS") );
	  toolMgr()->setUpdatesEnabled(true);
	}
    }

    if ( theEvent->type() == QEvent::ActionRemoved ) {
      QList<QAction*> anActns = aTB->actions();
      QIntList aIDL = toolMgr()->idList();
      for (int i = 0; i < aIDL.size(); ++i) {
	if ( toolMgr()->action(aIDL.at(i))->parent() == aTB
	     &&
	     !anActns.contains( toolMgr()->action(aIDL.at(i)) ) ) {
	  toolMgr()->setUpdatesEnabled(false);
	  toolMgr()->unRegisterAction( aIDL.at(i) );
	  toolMgr()->remove( aIDL.at(i), tr("TOOL_LOOKMARKS") );
	  toolMgr()->setUpdatesEnabled(true);
	}
      }
    }
    
  }

  return QObject::eventFilter( theObject, theEvent );
}

/*!
  \brief Activate module.
  \param study current study
  \return \c true if activaion is done successfully or 0 to prevent
  activation on error
*/
bool PVGUI_Module::activateModule( SUIT_Study* study )
{
  bool isDone = LightApp_Module::activateModule( study );
  if ( !isDone ) return false;

  showView( true );

  if ( mySourcesMenuId != -1 ) menuMgr()->show(mySourcesMenuId);
  if ( myFiltersMenuId != -1 ) menuMgr()->show(myFiltersMenuId);
  setMenuShown( true );
  setToolShown( true );

  toolMgr()->toolBar(tr("TOOL_LOOKMARKS"))->installEventFilter(this);

  // Make default server connection
  if ( Implementation )
    Implementation->Core.makeDefaultConnectionIfNoneExists();

  restoreDockWidgetsState();

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
  toolMgr()->toolBar(tr("TOOL_LOOKMARKS"))->removeEventFilter(this);

  showView( false );

  // hide menus
  menuMgr()->hide(mySourcesMenuId);
  menuMgr()->hide(myFiltersMenuId);
  setMenuShown( false );
  setToolShown( false );

  saveDockWidgetsState();

  return LightApp_Module::deactivateModule( study );
}

/*!
  \brief Compares the contents of the window with the given reference image,
  returns true if they "match" within some tolerance.
*/
bool PVGUI_Module::compareView( const QString& ReferenceImage, double Threshold,
				ostream& Output, const QString& TempDirectory )
{
  if ( Implementation )
    return Implementation->Core.compareView( ReferenceImage, Threshold, Output, TempDirectory );
  return false;
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
  PVGUI_EXPORT CAM_Module* createModule() {
    return new PVGUI_Module();
  }
}
