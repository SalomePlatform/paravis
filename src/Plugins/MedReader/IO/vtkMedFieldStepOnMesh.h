#ifndef __vtkMedFieldStepOnMesh_h_
#define __vtkMedFieldStepOnMesh_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkDataArray;
class vtkMedString;

class VTK_EXPORT vtkMedFieldStepOnMesh: public vtkObject
{
public:
	static vtkMedFieldStepOnMesh* New();
vtkTypeRevisionMacro(vtkMedFieldStepOnMesh, vtkObject)
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// The number of values of this field on this mesh
	vtkSetMacro(NumberOfValues, med_int);
	vtkGetMacro(NumberOfValues, med_int);

	// Description:
	// the Data that store the values read from file
	virtual void	SetData(vtkDataArray*);
	vtkGetObjectMacro(Data, vtkDataArray);

	// Description:
	// The number of quadrature points for this field, on this mesh and this cell type.
	vtkSetMacro(NumberOfQuadraturePoint, med_int);
	vtkGetMacro(NumberOfQuadraturePoint, med_int);

	// Description:
	// if the mesh is local or not.
	vtkSetMacro(Local, med_int);
	vtkGetMacro(Local, med_int);

	// Description:
	// The name of the mesh.
	vtkGetObjectMacro(MeshName, vtkMedString);

	// Description:
	// The name of the profile.
	vtkGetObjectMacro(ProfileName, vtkMedString);

	// Description:
	// The name of the Quadrature Definition.
	vtkGetObjectMacro(QuadratureDefinitionName, vtkMedString);

	// Description:
	// returns true if this field on mesh has been loaded once,
	// and we saved the profile and quadrature definition names
	vtkSetMacro(MetaInfo, int);
	vtkGetMacro(MetaInfo, int);

	// Description:
	// returns true if this field on mesh has a profile. This is detected by comparing the
	// number of values for the MED_COMPÄCT mode and the MED_GLOBAL mode.
	vtkSetMacro(HasProfile, int);
	vtkGetMacro(HasProfile, int);

	// Description:
	// returns true if the Data array is not NULL, and its size matches the NumberOfValues
	virtual int	IsLoaded();

protected:
	vtkMedFieldStepOnMesh();
	virtual ~vtkMedFieldStepOnMesh();

	med_int NumberOfQuadraturePoint;
	med_int Local;
	vtkMedString* MeshName;
	vtkMedString* ProfileName;
	vtkMedString* QuadratureDefinitionName;
	med_int NumberOfValues;
	vtkDataArray* Data;
	int MetaInfo;
	int HasProfile;

private:
	vtkMedFieldStepOnMesh(const vtkMedFieldStepOnMesh&); // Not implemented.
	void operator=(const vtkMedFieldStepOnMesh&); // Not implemented.
};

#endif //__vtkMedFieldStepOnMesh_h_
