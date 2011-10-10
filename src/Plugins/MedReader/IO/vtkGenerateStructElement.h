#ifndef __vtkGenerateStructElement_h__
#define __vtkGenerateStructElement_h__

#include "vtkUnstructuredGridAlgorithm.h"

class vtkMedStructElement;
struct vtkGenerateStructElementCache;

class vtkGenerateStructElement : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkGenerateStructElement* New();
  vtkTypeRevisionMacro(vtkGenerateStructElement, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

protected :
  vtkGenerateStructElement();
  virtual ~vtkGenerateStructElement();

  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  //BTX
  virtual double  GetParameter1(const char*,
                           vtkIdType,
                           vtkGenerateStructElementCache&);
  //ETX

private :
  vtkGenerateStructElement(const vtkGenerateStructElement&);// Not implemented.
  void operator=(const vtkGenerateStructElement&); // Not implemented.

};

#endif //__vtkGenerateStructElement_h__
