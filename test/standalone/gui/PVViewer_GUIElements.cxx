// Copyright (C) 2010-2016  CEA/DEN, EDF R&D
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
// Author: Adrien Bruneton (CEA)

#include <iostream>

#include "PVViewer_GUIElements.h"
#include "PVViewer_Core.h"

#include <pqPVApplicationCore.h>
#include <pqActiveObjects.h>
#include <pqObjectBuilder.h>

#include <pqPropertiesPanel.h>
#include <pqPipelineBrowserWidget.h>
#include <pqTabbedMultiViewWidget.h>
#include <pqParaViewMenuBuilders.h>
#include <pqMainControlsToolbar.h>
#include <pqVCRToolbar.h>
#include <pqAnimationTimeToolbar.h>
#include <pqColorToolbar.h>
#include <pqRepresentationToolbar.h>
#include <pqCameraToolbar.h>
#include <pqAxesToolbar.h>
#include <pqSetName.h>

#include <pqPythonManager.h>
#include <pqApplicationCore.h>

#include <QMainWindow>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QToolBar>
#include <QLayout>

#include <QCoreApplication>

PVViewer_GUIElements * PVViewer_GUIElements::theInstance = 0;

PVViewer_GUIElements::PVViewer_GUIElements(QMainWindow* desk) :
  propertiesPanel(0), pipelineBrowserWidget(0),
  sourcesMenu(0)
{
  propertiesPanel = new pqPropertiesPanel(desk);
  propertiesPanel->hide();
  pipelineBrowserWidget  = new pqPipelineBrowserWidget(desk);
  pipelineBrowserWidget->hide();

  sourcesMenu = new QMenu(0);
  pqParaViewMenuBuilders::buildSourcesMenu(*sourcesMenu, desk);
  filtersMenu = new QMenu(0);
  pqParaViewMenuBuilders::buildFiltersMenu(*filtersMenu, desk);
  macrosMenu = new QMenu(0);
  pqParaViewMenuBuilders::buildMacrosMenu(*macrosMenu);

//  myBuildToolbars(desk);
}

PVViewer_GUIElements * PVViewer_GUIElements::GetInstance(QMainWindow* desk)
{
  if (! theInstance)
    theInstance = new PVViewer_GUIElements(desk);
  return theInstance;
}


pqTabbedMultiViewWidget * PVViewer_GUIElements::getTabbedMultiViewWidget()
{
  pqTabbedMultiViewWidget * multiv =
        qobject_cast<pqTabbedMultiViewWidget*>(PVViewer_Core::GetPVApplication()->manager("MULTIVIEW_WIDGET"));

  // If not found, instanciate it. It will then register automatically as a MULTIVIEW_WIDGET.
  // Also create a single view that will be attached by the tabbedMultiView automatically
  // (in PV this is done automatically upon server connection event).
  if (!multiv)
    {
      multiv = new pqTabbedMultiViewWidget();

      // Create a new view proxy on the server
      pqObjectBuilder* builder = pqApplicationCore::instance()->getObjectBuilder();
      pqServer* active_serv = pqActiveObjects::instance().activeServer();
      pqView * pqview = builder->createView(QString("RenderView"), active_serv);
    }

  return multiv;
}


/**
 * See ParaView source code: pqParaViewMenuBuilders::buildToolbars()
 * to keep this function up to date:
 */
void PVViewer_GUIElements::myBuildToolbars(QMainWindow* mainWindow)
{
  mainToolBar = new pqMainControlsToolbar(mainWindow)
    << pqSetName("MainControlsToolbar");
  mainToolBar->layout()->setSpacing(0);

  vcrToolbar = new pqVCRToolbar(mainWindow)
    << pqSetName("VCRToolbar");
  vcrToolbar->layout()->setSpacing(0);

  timeToolbar = new pqAnimationTimeToolbar(mainWindow)
    << pqSetName("currentTimeToolbar");
  timeToolbar->layout()->setSpacing(0);

  colorToolbar = new pqColorToolbar(mainWindow)
    << pqSetName("variableToolbar");
  colorToolbar->layout()->setSpacing(0);

  reprToolbar = new pqRepresentationToolbar(mainWindow)
    << pqSetName("representationToolbar");
  reprToolbar->layout()->setSpacing(0);

  cameraToolbar = new pqCameraToolbar(mainWindow)
    << pqSetName("cameraToolbar");
  cameraToolbar->layout()->setSpacing(0);

  axesToolbar = new pqAxesToolbar(mainWindow)
    << pqSetName("axesToolbar");
  axesToolbar->layout()->setSpacing(0);

  // Give the macros menu to the pqPythonMacroSupervisor
  pqPythonManager* manager = qobject_cast<pqPythonManager*>(
    pqApplicationCore::instance()->manager("PYTHON_MANAGER"));

  macrosToolbar = new QToolBar("Macros Toolbars", mainWindow)
      << pqSetName("MacrosToolbar");
  manager->addWidgetForRunMacros(macrosToolbar);

  commonToolbar = new QToolBar("Common", mainWindow) << pqSetName("Common");
  commonToolbar->layout()->setSpacing(0);

  dataToolbar = new QToolBar("DataAnalysis", mainWindow) << pqSetName("DataAnalysis");
  dataToolbar->layout()->setSpacing(0);

  //addToolbars(mainWindow);
}

void PVViewer_GUIElements::setToolBarVisible(bool show)
{
  QCoreApplication::processEvents();
  mainAction->setChecked(!show);
  mainAction->setVisible(show);
  mainAction->trigger();
  vcrAction->setChecked(!show);
  vcrAction->setVisible(show);
  vcrAction->trigger();
  timeAction->setChecked(!show);
  timeAction->setVisible(show);
  timeAction->trigger();
  colorAction->setChecked(!show);
  colorAction->setVisible(show);
  colorAction->trigger();
  reprAction->setChecked(!show);
  reprAction->setVisible(show);
  reprAction->trigger();
  cameraAction->setChecked(!show);
  cameraAction->setVisible(show);
  cameraAction->trigger();
  axesAction->setChecked(!show);
  axesAction->setVisible(show);
  axesAction->trigger();
  macrosAction->setChecked(!show);
  macrosAction->setVisible(show);
  macrosAction->trigger();
  commonAction->setChecked(!show);
  commonAction->setVisible(show);
  commonAction->trigger();
  dataAction->setChecked(!show);
  dataAction->setVisible(show);
  dataAction->trigger();
}

void PVViewer_GUIElements::addToolbars(QMainWindow* desk)
{
  desk->addToolBar(Qt::TopToolBarArea, mainToolBar);
  desk->addToolBar(Qt::TopToolBarArea, vcrToolbar);
  desk->addToolBar(Qt::TopToolBarArea, timeToolbar);
  desk->addToolBar(Qt::TopToolBarArea, colorToolbar);
  desk->insertToolBarBreak(colorToolbar);
  desk->addToolBar(Qt::TopToolBarArea, reprToolbar);
  desk->addToolBar(Qt::TopToolBarArea, cameraToolbar);
  desk->addToolBar(Qt::TopToolBarArea, axesToolbar);
  desk->addToolBar(Qt::TopToolBarArea, macrosToolbar);
  desk->addToolBar(Qt::TopToolBarArea, commonToolbar);
  desk->addToolBar(Qt::TopToolBarArea, dataToolbar);

  mainAction = mainToolBar->toggleViewAction();
  vcrAction = vcrToolbar->toggleViewAction();
  timeAction = timeToolbar->toggleViewAction();
  colorAction = colorToolbar->toggleViewAction();
  reprAction = reprToolbar->toggleViewAction();
  cameraAction = cameraToolbar->toggleViewAction();
  axesAction = axesToolbar->toggleViewAction();
  macrosAction = macrosToolbar->toggleViewAction();
  commonAction = commonToolbar->toggleViewAction();
  dataAction = dataToolbar->toggleViewAction();
}

void PVViewer_GUIElements::onEmulateApply()
{
  if (propertiesPanel)
    propertiesPanel->apply();
}

QList<QToolBar*> PVViewer_GUIElements::getToolbars()
{
  QList<QToolBar*> l;
  l << mainToolBar << vcrToolbar << timeToolbar << colorToolbar
    << reprToolbar << cameraToolbar << axesToolbar << macrosToolbar
    << commonToolbar << dataToolbar;
  return l;
}

void PVViewer_GUIElements::setToolBarEnabled(bool enabled)
{
  mainToolBar  ->setEnabled(enabled);
  vcrToolbar   ->setEnabled(enabled);
  timeToolbar  ->setEnabled(enabled);
  colorToolbar ->setEnabled(enabled);
  reprToolbar  ->setEnabled(enabled);
  cameraToolbar->setEnabled(enabled);
  axesToolbar  ->setEnabled(enabled);
  macrosToolbar->setEnabled(enabled);
  commonToolbar->setEnabled(enabled);
  dataToolbar  ->setEnabled(enabled);
}
