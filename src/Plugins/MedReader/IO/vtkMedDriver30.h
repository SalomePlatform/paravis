#ifndef __vtkMedDriver30_h_
#define __vtkMedDriver30_h_

#include "vtkObject.h"
#include "vtkMed.h"
#include "vtkMedDriver.h"

class vtkMedFile;
class vtkMedMesh;
class vtkMedField;
class vtkMedFamily;
class vtkMedRegularGrid;
class vtkMedCurvilinearGrid;
class vtkMedUnstructuredGrid;
class vtkMedLocalization;
class vtkMedProfile;
class vtkMedFieldOverEntity;
class vtkMedEntityArray;
class vtkMedFieldStep;
class vtkMedLink;

class VTK_EXPORT vtkMedDriver30: public vtkMedDriver
{
public:
  static vtkMedDriver30* New();
  vtkTypeRevisionMacro(vtkMedDriver30, vtkMedDriver)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // load all meta data associated with this file.
  virtual void ReadFileInformation(vtkMedFile*);

  // Description:
  // load all meta data associated with this mesh.
  virtual void ReadMeshInformation(vtkMedMesh*);

  // Description:
  // load all meta data associated with this field.
  virtual void ReadFieldInformation(vtkMedField*);

  // Description:
  // load all meta data associated with this regular (Cartesian or polar) grid.
  virtual void ReadRegularGridInformation(vtkMedRegularGrid*);

  // Description:
  // load all meta data associated with this standard grid.
  virtual void ReadCurvilinearGridInformation(vtkMedCurvilinearGrid*);

  // Description:
  // load all meta data associated with this unstructured grid.
  virtual void ReadUnstructuredGridInformation(vtkMedUnstructuredGrid*);

  // Description:
  // load all meta data associated with this family.
  virtual void ReadFamilyInformation(vtkMedMesh*, vtkMedFamily*);

  // Description:
  // load all meta data associated with this family.
  virtual void ReadProfileInformation(vtkMedProfile*);

  // Description:
  // load information on a field and a given cell type at a given step
  virtual void ReadFieldOverEntityInformation(vtkMedFieldOverEntity*);

  // Description:
  // load information related to the given quadrature scheme definition
  virtual void ReadLocalizationInformation(vtkMedLocalization*);

  // Description:
  // load all information associated witht this field step
  virtual void ReadFieldStepInformation(vtkMedFieldStep*);

  // Description:
  // load all information related to this interpolation function
  virtual void ReadInterpolationInformation(vtkMedInterpolation*);

  // Description:
  // load all information related to this field on this profile
  virtual void ReadFieldOnProfileInformation(vtkMedFieldOnProfile*);

  // Description:
  // load all information related to this field on this profile
  virtual void ReadLinkInformation(vtkMedLink*);

  // Description:
  // load the field on this profile
  virtual void LoadField(vtkMedFieldOnProfile*);

  // Description:
  // Load the indices of the profile
  virtual void LoadProfile(vtkMedProfile* profile);

  // Description:
  // Load the values of the given step
//  void Load(vtkMedFieldOnProfile*);

  virtual void LoadFamilyIds(vtkMedEntityArray*);

  virtual void LoadCoordinates(vtkMedGrid*);

  virtual void LoadPointGlobalIds(vtkMedGrid*);

  //virtual void LoadFamilyIds(vtkMedGrid*);

  virtual void LoadConnectivity(vtkMedEntityArray*);

  virtual void LoadCellGlobalIds(vtkMedEntityArray*);

  virtual void LoadRegularGridCoordinates(vtkMedRegularGrid*);

  virtual void LoadCurvilinearGridCoordinates(vtkMedCurvilinearGrid*);

protected:
  vtkMedDriver30();
  ~vtkMedDriver30();

  void ReadNumberOfEntity(vtkMedUnstructuredGrid*,
      med_entity_type, med_connectivity_mode);

private:
  vtkMedDriver30(const vtkMedDriver30&); // Not implemented.
  void operator=(const vtkMedDriver30&); // Not implemented.
};

#endif //__vtkMedDriver30_h_
