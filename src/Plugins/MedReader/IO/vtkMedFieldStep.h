#ifndef __vtkMedFieldStep_h_
#define __vtkMedFieldStep_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedFieldStepOnMesh;
class vtkMedString;
class vtkMedMesh;
class vtkDataArray;

class VTK_EXPORT vtkMedFieldStep: public vtkObject
{
public:
	static vtkMedFieldStep* New();
vtkTypeRevisionMacro(vtkMedFieldStep, vtkObject)
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// The physical time of this step
	vtkSetMacro(Time, med_float);
	vtkGetMacro(Time, med_float);

	// Description:
	// The id of the physical time of this step
	vtkSetMacro(TimeId, med_int);
	vtkGetMacro(TimeId, med_int);

	// Description:
	// The iteration of this step
	vtkSetMacro(Iteration, med_int);
	vtkGetMacro(Iteration, med_int);

	// Description:
	// The physical time of this step
	vtkGetObjectMacro(TimeUnit, vtkMedString);

	// Description:
	// the number of mesh this field is associated with.
	vtkGetObjectVectorMacro(StepOnMesh, vtkMedFieldStepOnMesh);
	vtkSetObjectVectorMacro(StepOnMesh, vtkMedFieldStepOnMesh);
	virtual vtkMedFieldStepOnMesh* GetStepOnMesh(vtkMedMesh*);

protected:
	vtkMedFieldStep();
	virtual ~vtkMedFieldStep();

	med_float Time;
	med_int TimeId;
	med_int Iteration;
	vtkMedString* TimeUnit;
	//BTX
	vtkObjectVector<vtkMedFieldStepOnMesh>* StepOnMesh;
	//ETX


private:
	vtkMedFieldStep(const vtkMedFieldStep&); // Not implemented.
	void operator=(const vtkMedFieldStep&); // Not implemented.
};

#endif //__vtkMedFieldStep_h_
