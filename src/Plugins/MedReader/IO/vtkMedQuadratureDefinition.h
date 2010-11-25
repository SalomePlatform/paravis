#ifndef __vtkMedQuadratureDefinition_h_
#define __vtkMedQuadratureDefinition_h_

#include "vtkObject.h"
#include "vtkMed.h"

class vtkMedString;

class VTK_EXPORT vtkMedQuadratureDefinition: public vtkObject
{
public:
	static vtkMedQuadratureDefinition* New();
vtkTypeRevisionMacro(vtkMedQuadratureDefinition, vtkObject)
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// This is the name of this definition
	vtkGetObjectMacro(Name, vtkMedString);

	// Description:
	// This is the type of cell geometry this definition is for.
	vtkSetMacro(Geometry, med_geometrie_element);
	vtkGetMacro(Geometry, med_geometrie_element);

	// Description:
	// This is the number of quadrature points in this definition.
	vtkSetMacro(NumberOfQuadraturePoint, int);
	vtkGetMacro(NumberOfQuadraturePoint, int);

	// Description:
	// The index of this field in the med file
	vtkSetMacro(MedIndex, med_int);
	vtkGetMacro(MedIndex, med_int);

	// Description:
	// get the raw pointers to the internal arrays.
	// Those arrays are allocated in SecureResources, and cleared in ClearResources
	//BTX
	vtkGetMacro(Weights, double*);
	vtkGetMacro(PointLocalCoordinates, double*);
	vtkGetMacro(QuadraturePointLocalCoordinates, double*);
	vtkGetMacro(ShapeFunction, double*);
	//ETX

	virtual int GetSizeOfWeights();
	virtual int GetSizeOfPointLocalCoordinates();
	virtual int GetSizeOfQuadraturePointLocalCoordinates();
	virtual int GetSizeOfShapeFunction();

	virtual void SecureResources();
	virtual void ClearResources();

	virtual int IsLoaded();

	virtual void BuildShapeFunction();

	virtual void BuildPoint1();
	virtual void BuildSeg2();
	virtual void BuildSeg3();
	virtual void BuildTria3();
	virtual void BuildTria6();
	virtual void BuildTria7();
	virtual void BuildQuad4();
	virtual void BuildQuad8();
	virtual void BuildHexa8();
	virtual void BuildHexa20();
	virtual void BuildTetra4();
	virtual void BuildTetra10();
	virtual void BuildPenta6();
	virtual void BuildPenta15();
	virtual void BuildPyra5();
	virtual void BuildPyra13();

	virtual void	BuildELNO(med_geometrie_element geometry);
	virtual void	BuildCenter(med_geometrie_element geometry);

protected:
	vtkMedQuadratureDefinition();
	virtual ~vtkMedQuadratureDefinition();

	med_int MedIndex;
	med_geometrie_element Geometry;
	int NumberOfQuadraturePoint;

	double* Weights;
	double* PointLocalCoordinates;
	double* QuadraturePointLocalCoordinates;
	double* ShapeFunction;

	vtkMedString* Name;

private:
	vtkMedQuadratureDefinition(const vtkMedQuadratureDefinition&); // Not implemented.
	void operator=(const vtkMedQuadratureDefinition&); // Not implemented.

};

#endif //__vtkMedQuadratureDefinition_h_
