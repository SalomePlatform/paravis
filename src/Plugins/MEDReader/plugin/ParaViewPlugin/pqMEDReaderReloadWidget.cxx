// Copyright (C) 2010-2024  CEA, EDF
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
// Author : Anthony Geay

#include "pqMEDReaderReloadWidget.h"

#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProperty.h"
#include "pqUndoStack.h"
#include "vtkSMReaderReloadHelper.h"

#include <QPushButton>
#include <QGridLayout>

pqMEDReaderReloadWidget::pqMEDReaderReloadWidget(vtkSMProxy *smProxy,
                                                 vtkSMProperty *proxyProperty,
                                                 QWidget *pWidget)
: pqPropertyWidget(smProxy, pWidget),
  Property(proxyProperty)
{
  this->setShowLabel(false);

  // Grid Layout
  QGridLayout* gridLayout = new QGridLayout(this);
  gridLayout->setAlignment(Qt::AlignRight);

  // Reload Button
  QPushButton *button = new QPushButton();
  button->setIcon(QIcon(":/ParaViewResources/Icons/pqReloadFile16.png"));
  button->setFixedSize(button->sizeHint());
  gridLayout->addWidget(button);

  // Connection
  connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

pqMEDReaderReloadWidget::~pqMEDReaderReloadWidget()
{
}

void pqMEDReaderReloadWidget::buttonClicked()
{
  // code adapted from ParaView/Qt/ApplicationComponents/pqReloadFilesReaction.cxx
  // which corresponds to right-click reload files

  vtkNew<vtkSMReaderReloadHelper> helper;

  vtkSMSourceProxy* sourceProxy = vtkSMSourceProxy::SafeDownCast(this->proxy());

  if (!helper->SupportsReload(sourceProxy))
  {
    return;
  }

  BEGIN_UNDO_EXCLUDE();
  // as MEDReader support FileSeries, we can either reload files or extend file series
  // (the right-click reload files allows to choose it from a dialog box)
  // as single med file is the main use case, we choose to reload it
  // (instead of extending file series)
  helper->ReloadFiles(sourceProxy);
//  helper->ExtendFileSeries(sourceProxy);
  pqApplicationCore::instance()->render();
  END_UNDO_EXCLUDE();
}
