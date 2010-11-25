#ifndef __vtkMedField_h_
#define __vtkMedField_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedFieldOverEntity;
class vtkMedString;

class VTK_EXPORT vtkMedField: public vtkObject
{
public:
	static vtkMedField* New();
	vtkTypeRevisionMacro(vtkMedField, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// The number of component of this field
	virtual void	SetNumberOfComponent(int);
	vtkGetMacro(NumberOfComponent, int);

	// Description:
	// The number of component of this field
	vtkSetMacro(DataType, med_type_champ);
	vtkGetMacro(DataType, med_type_champ);

	// Description:
	// The name of this field
	vtkGetObjectMacro(Name, vtkMedString);

	// Description:
	// The units of each component of this field
	vtkGetObjectVectorMacro(Unit, vtkMedString);
	vtkSetObjectVectorMacro(Unit, vtkMedString);

	// Description:
	// The name of each component of this field
	vtkGetObjectVectorMacro(ComponentName, vtkMedString);
	vtkSetObjectVectorMacro(ComponentName, vtkMedString);

	// Description:
	// add a cell type as support to this field
	vtkGetObjectVectorMacro(FieldOverEntity, vtkMedFieldOverEntity);
	vtkSetObjectVectorMacro(FieldOverEntity, vtkMedFieldOverEntity);

	// Description:
	// returns if the field is on point, cell or quadrature point
	//BTX
	enum{PointField, CellField, QuadratureField};
	vtkSetMacro(Type, int);
	vtkGetMacro(Type, int);
	virtual void	ComputeFieldSupportType();

	// Description:
	// The index of this field in the med file
	vtkSetMacro(MedIndex, med_int);
	vtkGetMacro(MedIndex, med_int);

	// Description:
	// returns the vtkMedFieldOverEntity for the given Type and Geometry;
	virtual vtkMedFieldOverEntity* GetFieldOverEntity(med_entite_maillage, med_geometrie_element);

protected:
	vtkMedField();
	virtual ~vtkMedField();

	int NumberOfComponent;
	med_type_champ DataType;
	med_int MedIndex;
	vtkMedString* Name;
	int Type;
  //BTX
	vtkObjectVector<vtkMedFieldOverEntity>* FieldOverEntity;
	vtkObjectVector<vtkMedString>* Unit;
	vtkObjectVector<vtkMedString>* ComponentName;
  //ETX

private:
	vtkMedField(const vtkMedField&); // Not implemented.
	void operator=(const vtkMedField&); // Not implemented.

};

#endif //__vtkMedField_h_
