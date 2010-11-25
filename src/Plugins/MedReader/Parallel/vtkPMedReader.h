#ifndef __vtkPMedReader_h_
#define __vtkPMedReader_h_

#include "vtkMedReader.h"

class VTK_EXPORT vtkPMedReader: public vtkMedReader
{
public:
  static vtkPMedReader* New();
  vtkTypeRevisionMacro(vtkPMedReader, vtkMedReader)
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkPMedReader();
  virtual ~vtkPMedReader();

  int RequestData(vtkInformation *,
      vtkInformationVector **,
      vtkInformationVector *);

  int RequestInformation(vtkInformation *,
      vtkInformationVector **,
      vtkInformationVector *);

private:
  vtkPMedReader(const vtkPMedReader&); // Not implemented.
  void operator=(const vtkPMedReader&); // Not implemented.
};

#endif //__vtkPMedReader_h_
