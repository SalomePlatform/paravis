// Copyright (C) 2023-2024  CEA, EDF
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

#ifndef vtkCADMapper_h
#define vtkCADMapper_h

#include "CADRepresentationsModule.h" // for export macro

#include <vtkOpenGLPolyDataMapper.h>

#include <set>
#include <vector>

class CADREPRESENTATIONS_EXPORT vtkCADMapper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkCADMapper* New();
  vtkTypeMacro(vtkCADMapper, vtkOpenGLPolyDataMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   *  Set the opacity value for the given VTK cell.
   */
  void SetOpacity(vtkIdType id, unsigned char opacity);

  /**
   *  Set the reset color value for the given VTK cell.
   */
  void ToggleResetColor(vtkIdType id);


  /**
   *  Set the select color value for the given VTK cell.
   */
  void ToggleSelectColor(vtkIdType id);


  /**
   *  Set the preselect color value for the given VTK cell.
   */
  void TogglePreselectColor(vtkIdType id);

  /**
   *  Initialize the map between the VTK cells and OpenGL primitives.
   *  Initialize also the colors of the primitives using this map
   *  and the defined colors (Face/Vertex/Edge colors)
   */
  void Initialize();

  /**
   * Force BuildCellTextures to execute for the next rendering step.
   */
  void ForceUpdate();

  ///@{
  /**
   * Mark the begining and the end of the selection.
   */
  void BeginSelect();
  void EndSelect();
  ///@}

  /**
   * Create a new selection group, and reset the selection.
   */
  void CreateGroup();

  /**
   * If in group mode, set the visibility of the given selection group.
   */
  void SetGroupVisibility(int groupIdx, bool visibility);

  /**
   * If in group mode, set the opacity of the given selection group.
   */
  void SetGroupOpacity(int groupIdx, unsigned char opacity);

  /**
   * Enable/disable the Group Mode. In this mode, you visualize
   * groups you saved (with the CreateGroup function). Only one
   * group can be displayed at a time.
   */
  void SetGroupModeEnabled(bool enabled);

  /**
   * Reset the current selection.
   */
  void ResetSelection();

  /**
   * Get PreSelected Id
   */
  vtkIdType GetPreselectedId();

protected:
  vtkCADMapper() = default;
  ~vtkCADMapper() = default;

  /**
   * Called each time a render pass is done. Pushes the primitive colors (PrimColors)
   * on the OpenGL side.
   */
  void BuildCellTextures(
    vtkRenderer* ren, vtkActor* actor, vtkCellArray* prims[4], int representation) override;

private:
  vtkCADMapper(const vtkCADMapper&) = delete;
  void operator=(const vtkCADMapper&) = delete;

  /**
   * Called during the render pass if the mapper's selection (vtkSelection) changed.
   * - Save the preselected id in order to render pre-selection when BuildCellTextures is called
   * - If a new selected id is furnished, add it to the SelectedId cache and toggle
   *   the selection color for the corresponding vtkCell ids using the SelectionCache map.
   *
   * Selected / preselected ids are values of the data array (arrayName), which allow to
   * map geometry elements to their vtkCell counterparts. This mapping is stored in the
   * SelectionCache map.
   */
  void AddCellIdsToSelectionPrimitives(vtkPolyData* poly, const char* arrayName,
    unsigned int processId, unsigned int compositeIndex, vtkIdType selectedId) override;

  /**
   * Get Selected Id list
   */
  //vtkIdType GetSelectedId();

  /**
   * Initialise the primitive colors using color array stored in the input polydata.
   * Also initialize the opacity values (depending on current mode).
   */
  void InitializePrimColors();

  // VTK cells to OpenGL primitives map
  std::vector<std::vector<vtkIdType>> CellToPrimMap;
  // Store the currenly selected ids (array values)
  std::set<vtkIdType> SelectedIds;
  // Store all the saved selection groups and their respective array name
  // (type of cell to select)
  std::vector<std::pair<std::set<vtkIdType>, std::string>> SavedGroups;
  // OpenGL primitives color, used in the BuildCellTextures method.
  std::vector<unsigned char> PrimColors;
  // Store the current preselected id (array value)
  vtkIdType PreselectedCellId = -1;
  bool NeedUpdate = false;
  bool Selecting = false;
  std::string CurrentArrayName;
  bool GroupModeEnabled = false;

  std::vector<bool> SavedGroupVisibilities;
  std::vector<unsigned char> SavedCellOpacities;

  const unsigned char FaceColor[3] = { 255, 255, 255 };
  const unsigned char EdgeColor[3] = { 0, 255, 0 };
  const unsigned char VertexColor[3] = { 255, 128, 0 };
  const unsigned char PreselectionColor[3] = { 0, 0, 255 };
  const unsigned char SelectionColor[3] = { 255, 0, 0 };
  const unsigned char GroupSelectionColor[3] = { 0, 255, 255 };
};

#endif
