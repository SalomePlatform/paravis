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

#ifndef vtkCADRepresentation_h
#define vtkCADRepresentation_h

#include "CADRepresentationsModule.h" // for export macro

#include <vtkPVDataRepresentation.h>

#include <array>
#include <map>
#include <unordered_set>
#include <vector>

class vtkActor;
class vtkCADMapper;

enum SelectionType : unsigned char
{
  GeometrySolids = 0,
  GeometryVertices,
  GeometryEdges,
  GeometryFaces,
  SelectionTypeCount
};
class CADREPRESENTATIONS_EXPORT vtkCADRepresentation
  : public vtkPVDataRepresentation
{
public:
  static vtkCADRepresentation* New();
  vtkTypeMacro(vtkCADRepresentation, vtkPVDataRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  int ProcessViewRequest(vtkInformationRequestKey* request_type, vtkInformation* inInfo,
    vtkInformation* outInfo) override;

  /**
   * Set the pre-configured selection source for making the selection.
   * The vtkSelection is obtained internally and forwarded to the mapper
   */
  void SetSelectionConnection(vtkAlgorithmOutput* input);

  /**
   * Set the visibility of the rendered actor.
   */
  void SetVisibility(bool val) override;

  void Reset();
  void BeginSelect();
  void EndSelect();

  /**
   * Create a new selection group, and reset the selection.
   * Forwarded to the mapper.
   */
  void CreateGroup();

  /**
   * If in group mode, set the visibility of the given selection group.
   * Forwarded to the mapper.
   */
  void SetGroupVisibility(int groupIdx, bool visibility);

  /**
   * If in group mode, set the opacity of the given selection group.
   * Forwarded to the mapper.
   */
  void SetGroupOpacity(int groupIdx, unsigned char opacity);

  /**
   * Enable/disable the Group Mode. In this mode, you visualize
   * groups you saved (with the CreateGroup function). Only one
   * group can be displayed at a time.
   * Forwarded to the mapper.
   */
  void SetGroupModeEnabled(bool enabled);

  /**
   * Add a shape to the list of selected shapes that will
   * be modified by setOpacity / toggleColor functions
   * (for debugging purposes).
   */
  void AddShape(vtkIdType groupId, vtkIdType cellId);

  void SetArrayIdNames(const char* pointArray, const char* cellArray) override;
protected:
  vtkCADRepresentation();
  ~vtkCADRepresentation() = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  bool AddToView(vtkView* view) override;
  bool RemoveFromView(vtkView* view) override;

  bool IsInitialized = false;
  vtkNew<vtkActor> Actor;
  vtkNew<vtkCADMapper> Mapper;
  vtkNew<vtkPolyData> PolyData;

private:
  vtkCADRepresentation(const vtkCADRepresentation&) = delete;
  void operator=(const vtkCADRepresentation&) = delete;
};

#endif // vtkCADRepresentation_h
