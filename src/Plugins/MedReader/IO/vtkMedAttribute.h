#ifndef __vtkMedAttribute_h_
#define __vtkMedAttribute_h_

#include "vtkObject.h"
#include "vtkMed.h"
class vtkMedString;

class VTK_EXPORT vtkMedAttribute : public vtkObject
{
public :
  static vtkMedAttribute* New();
  vtkTypeRevisionMacro(vtkMedAttribute, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // the id of this attribute
  vtkSetMacro(Id, med_int);
  vtkGetMacro(Id, med_int);

  // Description:
  // the value of this attribute
  vtkSetMacro(Value, med_int);
  vtkGetMacro(Value, med_int);

  // Description:
  // the description of this attribute
  vtkGetObjectMacro(Description, vtkMedString);

protected:
	vtkMedAttribute();
  virtual ~vtkMedAttribute();

  med_int Id;
  med_int Value;
  vtkMedString* Description;

private:
	vtkMedAttribute(const vtkMedAttribute&); // Not implemented.
  void operator=(const vtkMedAttribute&); // Not implemented.

};

#endif //__vtkMedAttribute_h_
