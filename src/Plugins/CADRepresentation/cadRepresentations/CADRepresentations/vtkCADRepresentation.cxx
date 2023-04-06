// Copyright (C) 2023  CEA/DEN, EDF R&D
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

#include "vtkCADMapper.h"
#include "vtkCADRepresentation.h"

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPVRenderView.h>
#include <vtkPVView.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>

#include <vtkMultiProcessController.h>
#include <vtkAlgorithmOutput.h>
#include <vtkSMSession.h>
#include <vtkProcessModule.h>
#include <vtkSelectionNode.h>

vtkStandardNewMacro(vtkCADRepresentation);
//----------------------------------------------------------------------------
vtkCADRepresentation::vtkCADRepresentation()
{
  this->Actor->SetMapper(this->Mapper);

  vtkNew<vtkSelection> sel;
  this->Mapper->SetSelection(sel);

  this->SetArrayIdNames(nullptr, nullptr);
}

//----------------------------------------------------------------------------
int vtkCADRepresentation::ProcessViewRequest(vtkInformationRequestKey* request_type, vtkInformation* inInfo,
  vtkInformation* outInfo)
{
  if (!this->Superclass::ProcessViewRequest(request_type, inInfo, outInfo))
  {
    return 0;
  }

  if (request_type == vtkPVView::REQUEST_UPDATE())
  {
    vtkPVRenderView::SetPiece(inInfo, this, this->PolyData);
    vtkPVRenderView::SetGeometryBounds(inInfo, this, this->PolyData->GetBounds());
  }
  else if (request_type == vtkPVView::REQUEST_RENDER())
  {
    vtkAlgorithmOutput* producerPort = vtkPVRenderView::GetPieceProducer(inInfo, this);
    this->Mapper->SetInputConnection(producerPort);

    if(!this->IsInitialized)
    {
      this->Mapper->Initialize();
      this->IsInitialized = true;
    }
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkCADRepresentation::SetVisibility(bool value)
{
  this->Superclass::SetVisibility(value);
  this->Actor->SetVisibility(value);
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::BeginSelect()
{
  if(this->IsInitialized)
  {
    this->Mapper->BeginSelect();
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::EndSelect()
{
  if(this->IsInitialized)
  {
    this->Mapper->EndSelect();
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::CreateGroup()
{
  if(this->IsInitialized)
  {
    this->Mapper->CreateGroup();
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::SetGroupVisibility(int groupIdx, bool visibility)
{
  if(this->IsInitialized)
  {
    this->Mapper->SetGroupVisibility(groupIdx, visibility);
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::SetGroupOpacity(int groupIdx, unsigned char opacity)
{
  if(this->IsInitialized)
  {
    this->Mapper->SetGroupOpacity(groupIdx, opacity);
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::SetGroupModeEnabled(bool enabled)
{
  if(this->IsInitialized)
  {
    this->Mapper->SetGroupModeEnabled(enabled);
  }
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::Reset()
{
  if(this->IsInitialized)
  {
    this->Mapper->ResetSelection();
  }
}

//------------------------------------------------------------------------------
int vtkCADRepresentation::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");

  // Saying INPUT_IS_OPTIONAL() is essential, since representations don't have
  // any inputs on client-side (in client-server, client-render-server mode) and
  // render-server-side (in client-render-server mode).
  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);

  return 1;
}

//------------------------------------------------------------------------------
int vtkCADRepresentation::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (inputVector[0]->GetNumberOfInformationObjects() == 1)
  {
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    this->PolyData->ShallowCopy(polyData);
  }
  else
  {
    this->PolyData->Initialize();
  }

  return this->Superclass::RequestData(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
bool vtkCADRepresentation::AddToView(vtkView* view)
{
  vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
  if (rview)
  {
    rview->GetRenderer()->AddActor(this->Actor);

    // Indicate that this is prop that we are rendering when hardware selection
    // is enabled.
    rview->RegisterPropForHardwareSelection(this, this->Actor);
    return this->Superclass::AddToView(view);
  }
  return false;
}

//------------------------------------------------------------------------------
bool vtkCADRepresentation::RemoveFromView(vtkView* view)
{
  vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
  if (rview)
  {
    rview->GetRenderer()->RemoveActor(this->Actor);
    rview->UnRegisterPropForHardwareSelection(this, this->Actor);
    return this->Superclass::RemoveFromView(view);
  }
  return false;
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::SetSelectionConnection(vtkAlgorithmOutput* input)
{
  // Copied from vtkCompositeRepresentation
  if (!input)
  {
    return;
  }

  vtkMultiProcessController* controller = vtkMultiProcessController::GetGlobalController();
  int numPiece = controller->GetNumberOfProcesses();
  int piece = controller->GetLocalProcessId();
  input->GetProducer()->UpdatePiece(piece, numPiece, 0);

  vtkSmartPointer<vtkSelection> sel;
  int actualNbPieces = numPiece;

  vtkSMSession* session =
    vtkSMSession::SafeDownCast(vtkProcessModule::GetProcessModule()->GetSession());
  if (session)
  {
    actualNbPieces = session->GetNumberOfProcesses(vtkPVSession::DATA_SERVER);
  }

  // in order to handle the case where we are connected to a parallel server using
  // local rendering, we have to compare the number of processes here
  if (numPiece < actualNbPieces && piece == 0)
  {
    vtkSelection* localSel =
      vtkSelection::SafeDownCast(input->GetProducer()->GetOutputDataObject(0));
    sel = vtkSmartPointer<vtkSelection>::New();
    sel->ShallowCopy(localSel);

    for (int i = 1; i < actualNbPieces; i++)
    {
      input->GetProducer()->UpdatePiece(i, actualNbPieces, 0);
      localSel = vtkSelection::SafeDownCast(input->GetProducer()->GetOutputDataObject(0));
      if (localSel->GetNumberOfNodes() > 1)
      {
        vtkWarningMacro("Only the first node of a selection will be considered.");
      }
      vtkSelectionNode* node = localSel->GetNode(0);
      node->GetProperties()->Set(vtkSelectionNode::PROCESS_ID(), i);
      sel->AddNode(localSel->GetNode(0));
    }
  }
  else
  {
    sel = vtkSelection::SafeDownCast(input->GetProducer()->GetOutputDataObject(0));
    if (sel->GetNumberOfNodes() > 1)
    {
      vtkWarningMacro("Only the first node of a selection will be considered.");
    }
    sel->GetNode(0)->GetProperties()->Set(vtkSelectionNode::PROCESS_ID(), piece);
  }

  this->Mapper->GetSelection()->ShallowCopy(sel);
}

//----------------------------------------------------------------------------
void vtkCADRepresentation::SetArrayIdNames(const char* pointArray, const char* cellArray)
{
  vtkCADMapper* mapper = vtkCADMapper::SafeDownCast(this->Mapper);
  mapper->SetPointIdArrayName(pointArray ? pointArray : "vtkOriginalPointIds");
  mapper->SetCellIdArrayName(cellArray ? cellArray : "vtkOriginalCellIds");
}
