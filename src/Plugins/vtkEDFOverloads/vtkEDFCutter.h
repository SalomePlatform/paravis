#ifndef __vtkEDFCutter_h__
#define __vtkEDFCutter_h__

#include "vtkCutter.h"

class VTK_EXPORT vtkEDFCutter : public vtkCutter
{
public :
  static vtkEDFCutter* New();
  vtkTypeRevisionMacro(vtkEDFCutter, vtkCutter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // these ivars
  // control the name given to the field in which the ids are written into.  If
  // set to NULL, then vtkOriginalCellIds or vtkOriginalPointIds (the default)
  // is used, respectively.
  vtkSetStringMacro(OriginalCellIdsName);
  virtual const char *GetOriginalCellIdsName() {
    return (  this->OriginalCellIdsName
            ? this->OriginalCellIdsName : "vtkOriginalCellIds");
  }

protected :
  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);

  virtual void  AssembleOutputTriangles(vtkPolyData* inpd,
                                        vtkPolyData* outpd);

  char* OriginalCellIdsName;

  vtkEDFCutter();
  ~vtkEDFCutter();

private:
  vtkEDFCutter(const vtkEDFCutter&);  // Not implemented.
  void operator=(const vtkEDFCutter&);  // Not implemented.
};

#endif //__vtkEDFCutter_h__
