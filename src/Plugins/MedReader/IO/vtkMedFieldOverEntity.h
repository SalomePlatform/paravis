#ifndef __vtkMedFieldOverEntity_h_
#define __vtkMedFieldOverEntity_h_

#include "vtkObject.h"
#include "vtkMed.h"

#include "vtkMedSetGet.h"

class vtkMedFieldStep;

class VTK_EXPORT vtkMedFieldOverEntity: public vtkObject
{
public:
  static vtkMedFieldOverEntity* New();
  vtkTypeRevisionMacro(vtkMedFieldOverEntity, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // the support of the cells : one of
  // MED_MAILLE, MED_FACE, MED_ARETE, MED_NOEUD, MED_NOEUD_MAILLE
  vtkSetMacro(MedType, med_entite_maillage);
  vtkGetMacro(MedType, med_entite_maillage);

  // Description:
  // The type of the cells : one of
  // MED_POINT1, MED_SEG2, MED_SEG3, MED_TRIA3, MED_QUAD4, MED_TRIA6, MED_QUAD8,
	// MED_TETRA4, MED_PYRA5, MED_PENTA6, MED_HEXA8, MED_TETRA10, MED_PYRA13,
	// MED_PENTA15, MED_HEXA20, MED_POLYGONE, MED_POLYEDRE
  vtkSetMacro(Geometry, med_geometrie_element);
  vtkGetMacro(Geometry, med_geometrie_element);

  // Description:
  // Set the number of steps of this field over these cells.
	vtkGetObjectVectorMacro(Step, vtkMedFieldStep);
	vtkSetObjectVectorMacro(Step, vtkMedFieldStep);

	// Description:
	// This ivar describes if this field is an ELNO field.
	vtkSetMacro(IsELNO, int);
	vtkGetMacro(IsELNO, int);

	virtual med_entite_maillage GetType();


protected:
  vtkMedFieldOverEntity();
  virtual ~vtkMedFieldOverEntity();

  med_entite_maillage	MedType;
  med_geometrie_element	Geometry;
  int IsELNO;
  //BTX
  vtkObjectVector<vtkMedFieldStep>* Step;
  //ETX

private:
  vtkMedFieldOverEntity(const vtkMedFieldOverEntity&); // Not implemented.
  void operator=(const vtkMedFieldOverEntity&); // Not implemented.
};

#endif //__vtkMedFieldOverEntity_h_
