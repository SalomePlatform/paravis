/*=========================================================================

   Program: ParaView
   Module: pqBivariateAnimationManager.cxx

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#include "pqBivariateAnimationManager.h"

#include "pqApplicationCore.h"
#include "pqRenderView.h"
#include "pqRepresentation.h"
#include "pqServerManagerModel.h"
#include "pqView.h"

#include "vtkSMPropertyHelper.h"
#include "vtkSMRepresentationProxy.h"

//-----------------------------------------------------------------------------
pqBivariateAnimationManager::pqBivariateAnimationManager(QObject* p)
  : QObject(p)
{
  pqServerManagerModel* smmodel = pqApplicationCore::instance()->getServerManagerModel();
  QObject::connect(smmodel, SIGNAL(preViewAdded(pqView*)), this, SLOT(onViewAdded(pqView*)));
  QObject::connect(smmodel, SIGNAL(preViewRemoved(pqView*)), this, SLOT(onViewRemoved(pqView*)));

  // Add currently existing views
  for (pqView* view : smmodel->findItems<pqView*>())
  {
    this->onViewAdded(view);
  }
}

//-----------------------------------------------------------------------------
pqBivariateAnimationManager::~pqBivariateAnimationManager() = default;

//-----------------------------------------------------------------------------
void pqBivariateAnimationManager::onRenderEnded()
{
  pqView* view = dynamic_cast<pqView*>(QObject::sender());
  QList<pqRepresentation*> reprs = view->getRepresentations();
  for (int i = 0; i < reprs.count(); ++i)
  {
    vtkSMRepresentationProxy* repr = vtkSMRepresentationProxy::SafeDownCast(reprs[i]->getProxy());
    if (repr && repr->GetProperty("Representation"))
    {
      const char* rs = vtkSMPropertyHelper(repr, "Representation").GetAsString();
      const int visible = vtkSMPropertyHelper(repr, "Visibility").GetAsInt();
      if (rs && !strcmp(rs, "Bivariate Noise Surface") && visible)
      {
        // If the view has a visible bivariate noise representation,
        // then ask for a new render.
        view->render();
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void pqBivariateAnimationManager::onViewAdded(pqView* view)
{
  if (dynamic_cast<pqRenderView*>(view))
  {
    this->Views.insert(view);
    QObject::connect(view, SIGNAL(endRender()), this, SLOT(onRenderEnded()));
  }
}

//-----------------------------------------------------------------------------
void pqBivariateAnimationManager::onViewRemoved(pqView* view)
{
  if (dynamic_cast<pqRenderView*>(view))
  {
    QObject::disconnect(view, SIGNAL(endRender()), this, SLOT(onRenderEnded()));
    this->Views.erase(view);
  }
}
