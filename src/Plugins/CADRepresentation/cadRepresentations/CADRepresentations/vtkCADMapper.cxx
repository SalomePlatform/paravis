// Copyright (C) 2023-2025  CEA, EDF
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

#include <vtkCellData.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLBufferObject.h>
#include <vtkOpenGLCellToVTKCellMap.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextureObject.h>
#include <vtkSelectionNode.h>

vtkStandardNewMacro(vtkCADMapper);

//-----------------------------------------------------------------------------
void vtkCADMapper::SetOpacity(vtkIdType cellId, unsigned char opacity)
{
  if(this->PrimColors.empty())
  {
    return;
  }

  for (vtkIdType primId : this->CellToPrimMap[cellId])
  {
    this->PrimColors[4 * primId + 3] = opacity;
  }
}

//-----------------------------------------------------------------------------
void vtkCADMapper::ToggleResetColor(vtkIdType cellId)
{
  if(this->PrimColors.empty())
  {
    return;
  }

  const int opacity = this->GroupModeEnabled ? 0 : 255;

  // Retrieve "default" colors
  unsigned char color[4];
  this->Colors->GetTypedTuple(cellId, color);

  for (vtkIdType primId : this->CellToPrimMap[cellId])
  {
    this->PrimColors[4 * primId] = color[0];
    this->PrimColors[4 * primId + 1] = color[1];
    this->PrimColors[4 * primId + 2] = color[2];
    this->PrimColors[4 * primId + 3] = opacity;
  }
}

//-----------------------------------------------------------------------------
void vtkCADMapper::ToggleSelectColor(vtkIdType cellId)
{
  if(this->PrimColors.empty())
  {
    return;
  }

  auto* selectionColors =
      this->GroupModeEnabled ? this->GroupSelectionColor : this->SelectionColor;

  const int opacity = this->GroupModeEnabled ? this->SavedCellOpacities[cellId] : 255;

  for (vtkIdType primId : this->CellToPrimMap[cellId])
  {
    this->PrimColors[4 * primId] = selectionColors[0];
    this->PrimColors[4 * primId + 1] = selectionColors[1];
    this->PrimColors[4 * primId + 2] = selectionColors[2];
    this->PrimColors[4 * primId + 3] = opacity;
  }
}

//-----------------------------------------------------------------------------
void vtkCADMapper::TogglePreselectColor(vtkIdType cellId)
{
  if(this->PrimColors.empty())
  {
    return;
  }

  for (vtkIdType primId : this->CellToPrimMap[cellId])
  {
    this->PrimColors[4 * primId] = this->PreselectionColor[0];
    this->PrimColors[4 * primId + 1] = this->PreselectionColor[1];
    this->PrimColors[4 * primId + 2] = this->PreselectionColor[2];
    this->PrimColors[4 * primId + 3] = 255;
  }
}

//-----------------------------------------------------------------------------
void vtkCADMapper::ForceUpdate()
{
  this->Modified();
  this->NeedUpdate = true;
}

//-----------------------------------------------------------------------------
void vtkCADMapper::BuildCellTextures(vtkRenderer* ren, vtkActor* vtkNotUsed(actor),
  vtkCellArray* vtkNotUsed(prims)[4], int vtkNotUsed(representation))
{
  if (!this->NeedUpdate)
  {
    return;
  }

  // Add the preselection
  for (vtkIdType idx :
    this->SelectionCache[std::make_tuple(0, 0, this->PreselectedCellId)])
  {
    this->TogglePreselectColor(idx);
  }

  // Fill OpenGL related buffers
  if (!this->CellScalarTexture)
  {
    this->CellScalarTexture = vtkTextureObject::New();
    this->CellScalarBuffer = vtkOpenGLBufferObject::New();
    this->CellScalarBuffer->SetType(vtkOpenGLBufferObject::TextureBuffer);
  }
  this->CellScalarTexture->SetContext(static_cast<vtkOpenGLRenderWindow*>(ren->GetVTKWindow()));
  this->CellScalarBuffer->Upload(this->PrimColors, vtkOpenGLBufferObject::TextureBuffer);
  this->CellScalarTexture->CreateTextureBuffer(
    static_cast<unsigned int>(this->PrimColors.size() / 4), 4, VTK_UNSIGNED_CHAR,
    this->CellScalarBuffer);

  // Reset preselection
  for (vtkIdType idx :
    this->SelectionCache[std::make_tuple(0, 0, this->PreselectedCellId)])
  {
    this->ToggleResetColor(idx);
  }

  this->NeedUpdate = false;
}

//-----------------------------------------------------------------------------
void vtkCADMapper::BeginSelect()
{
  this->Selecting = true;
}

//-----------------------------------------------------------------------------
void vtkCADMapper::EndSelect()
{
  this->Selecting = false;
}

//-----------------------------------------------------------------------------
void vtkCADMapper::CreateGroup()
{
  this->SavedGroups.emplace_back(std::make_pair(this->SelectedIds, this->CurrentArrayName));
  this->SavedGroupVisibilities.emplace_back(false);
  this->ResetSelection();
}

//-----------------------------------------------------------------------------
void vtkCADMapper::SetGroupVisibility(int groupIdx, bool visibility)
{
  if(!this->GroupModeEnabled)
  {
    return;
  }

  // Reconstruct the cache if needed (if a different array for selection by value is choosen)
  this->BuildSelectionCache(this->SavedGroups[groupIdx].second.c_str(), 0, this->CurrentInput);

  // Save the group visibility
  this->SavedGroupVisibilities[groupIdx] = visibility;

  // Show the selected group
  for (vtkIdType id : this->SavedGroups[groupIdx].first)
  {
    for (vtkIdType idx : this->SelectionCache[std::make_tuple(0, 0, id)])
    {
      if (visibility)
      {
        this->ToggleSelectColor(idx);
      }
      else
      {
        this->ToggleResetColor(idx);
      }
    }
    this->SelectedIds.emplace(id);
  }
  this->ForceUpdate();
}

//-----------------------------------------------------------------------------
void vtkCADMapper::SetGroupOpacity(int groupIdx, unsigned char opacity)
{
  if(!this->GroupModeEnabled)
  {
    return;
  }

  // Reconstruct the cache if needed (if a different array for selection by value is choosen)
  this->BuildSelectionCache(this->SavedGroups[groupIdx].second.c_str(), 0, this->CurrentInput);

  // Save the opacity of the selected group and update it directly if currently visible
  for (vtkIdType id : this->SavedGroups[groupIdx].first)
  {
    for (vtkIdType idx : this->SelectionCache[std::make_tuple(0, 0, id)])
    {
      this->SavedCellOpacities[idx] = opacity;
      if(this->SavedGroupVisibilities[groupIdx])
      {
        this->SetOpacity(idx, opacity);
      }
    }
  }
  this->ForceUpdate();
}

//-----------------------------------------------------------------------------
void vtkCADMapper::SetGroupModeEnabled(bool enabled)
{
  this->GroupModeEnabled = enabled;

  // Set the group mode colors and opacity and clear the selections
  this->InitializePrimColors();
  this->SelectedIds.clear();
  this->PreselectedCellId = -1;

  // Show all visible groups
  for (vtkIdType groupId = 0; groupId < this->SavedGroups.size(); groupId++)
  {
    if (this->SavedGroupVisibilities[groupId])
    {
      this->SetGroupVisibility(groupId, true);
    }
  }

  this->ForceUpdate();
}

//-----------------------------------------------------------------------------
void vtkCADMapper::ResetSelection()
{
  for (vtkIdType id : this->SelectedIds)
  {
    for (vtkIdType idx : this->SelectionCache[std::make_tuple(0, 0, id)])
    {
      this->ToggleResetColor(idx);
    }
  }

  this->SelectedIds.clear();
  this->PreselectedCellId = -1;
  this->ForceUpdate();
}

//-----------------------------------------------------------------------------
void vtkCADMapper::Initialize()
{
  vtkPolyData* input = vtkPolyData::SafeDownCast(this->GetInput());
  if(!input)
  {
    vtkErrorMacro("Unable to retrieve input polydata.");
    return;
  }

  vtkCellArray* prims[4];

  prims[0] = input->GetVerts();
  prims[1] = input->GetLines();
  prims[2] = input->GetPolys();
  prims[3] = input->GetStrips();

  vtkIdType nbVerts = input->GetNumberOfVerts();
  vtkIdType nbLines = input->GetNumberOfLines();
  vtkIdType nbPolys = input->GetNumberOfPolys();

  // Store the mapping from OpenGL primitives to VTK cells
  this->CellCellMap->BuildCellSupportArrays(prims, VTK_SURFACE, input->GetPoints());

  // Create the mapping from VTK cells to OpenGL primitives (inverse of CellCellMap)
  this->CellToPrimMap.clear();
  this->CellToPrimMap.resize(nbVerts + nbLines + nbPolys);
  for (auto& v : this->CellToPrimMap)
  {
    // heuristic : for performances, we assume that we will have at most 10 primitives per cell
    // it's just a reserve, the algorithm will still works if there is more primitives
    v.reserve(10);
  }

  for (size_t i = 0; i < this->CellCellMap->GetSize(); i++)
  {
    this->CellToPrimMap[this->CellCellMap->GetValue(i)].push_back(i);
  }

  // Reset the default colors for all VTK cells
  if (vtkUnsignedCharArray::SafeDownCast(input->GetCellData()->GetArray("RGB")))
  {
    input->GetCellData()->RemoveArray("RGB");
  }

  vtkNew<vtkUnsignedCharArray> colorArray;
  colorArray->SetNumberOfComponents(3);
  colorArray->SetNumberOfTuples(nbVerts + nbLines + nbPolys);
  colorArray->SetName("RGB");

  input->GetCellData()->SetScalars(colorArray);

  for (int i = 0; i < nbVerts; i++)
  {
    colorArray->SetTypedTuple(i, this->VertexColor);
  }

  for (int i = 0; i < nbLines; i++)
  {
    colorArray->SetTypedTuple(nbVerts + i, this->EdgeColor);
  }

  for (int i = 0; i < nbPolys; i++)
  {
    colorArray->SetTypedTuple(nbVerts + nbLines + i, this->FaceColor);
  }

  // Initialize the primitive colors from the created color array
  this->InitializePrimColors();

  // Clear saved group visibilities
  this->SavedGroupVisibilities.clear();

  // Initialize saved cell opacities
  this->SavedCellOpacities.clear();
  this->SavedCellOpacities.resize(nbVerts + nbLines + nbPolys, 255);

  // Reset texture and selection
  if (this->CellScalarTexture)
  {
    this->CellScalarTexture->Delete();
    this->CellScalarTexture = nullptr;
  }

  if (this->CellScalarBuffer)
  {
    this->CellScalarBuffer->Delete();
    this->CellScalarBuffer = nullptr;
  }

  this->SelectedIds.clear();
  this->PreselectedCellId = -1;
  this->ForceUpdate();
}

//----------------------------------------------------------------------------
void vtkCADMapper::InitializePrimColors()
{
  vtkPolyData* input = vtkPolyData::SafeDownCast(this->GetInput());
  if(!input)
  {
    vtkErrorMacro("Unable to retrieve input polydata.");
    return;
  }

  vtkUnsignedCharArray* colorArray = vtkUnsignedCharArray::SafeDownCast(input->GetCellData()->GetAbstractArray("RGB"));
  if(!colorArray)
  {
    vtkErrorMacro("Unable to retrieve the input color array.");
    return;
  }

  // Map the VTK cell color values to the OpenGL primitives color values
  unsigned char* colorPtr = colorArray->GetPointer(0);
  const int opacity = this->GroupModeEnabled ? 0 : 255;

  this->PrimColors.clear();
  this->PrimColors.reserve(4 * this->CellCellMap->GetSize());
  for (size_t i = 0; i < this->CellCellMap->GetSize(); i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->PrimColors.push_back(colorPtr[this->CellCellMap->GetValue(i) * 3 + j]);
    }
    this->PrimColors.push_back(opacity);
  }
}

//----------------------------------------------------------------------------
void vtkCADMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkCADMapper::AddCellIdsToSelectionPrimitives(vtkPolyData* poly, const char* arrayName,
  unsigned int processId, unsigned int compositeIndex, vtkIdType selectedId)
{
  this->BuildSelectionCache(arrayName, false, poly);
  this->CurrentArrayName = arrayName;

  // If we are selecting an already selected cell, remove it
  auto idIterator = this->SelectedIds.find(selectedId);
  if (idIterator != this->SelectedIds.end())
  {
    if (!this->Selecting)
    {
      return;
    }
    for (vtkIdType idx :
      this->SelectionCache[std::make_tuple(processId, compositeIndex, selectedId)])
    {
      this->ToggleResetColor(idx);
    }
    this->SelectedIds.erase(idIterator);
    return;
  }

  if (!this->Selecting)
  {
    if (this->PreselectedCellId == selectedId)
    {
      return;
    }
    this->PreselectedCellId = selectedId;
    this->ForceUpdate();
  }
  else
  {
    this->PreselectedCellId = -1;
    for (vtkIdType idx :
      this->SelectionCache[std::make_tuple(processId, compositeIndex, selectedId)])
    {
      this->ToggleSelectColor(idx);
    }
    this->SelectedIds.emplace(selectedId);
    this->ForceUpdate();
  }
}

//----------------------------------------------------------------------------

vtkIdType vtkCADMapper::GetPreselectedId()
{
  return this->PreselectedCellId;
}
