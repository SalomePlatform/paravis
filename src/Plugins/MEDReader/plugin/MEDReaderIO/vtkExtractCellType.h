#ifndef vtkExtractCellType_h
#define vtkExtractCellType_h

#include "MEDReaderIOModule.h" // for export macro
#include "vtkDataSetAlgorithm.h"
#include "vtkExtractSelection.h"
#include "vtkNew.h"

class vtkDataArraySelection;
class vtkIdTypeArray;

class MEDREADERIO_EXPORT vtkExtractCellType : public vtkExtractSelection
{
public:
  static vtkExtractCellType* New();
  vtkTypeMacro(vtkExtractCellType, vtkExtractSelection);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual int GetNumberOfGeoTypesArrays();
  const char *GetGeoTypesArrayName(int index);
  int GetGeoTypesArrayStatus(const char *name);
  virtual void SetGeoTypesStatus(const char *name, int status);
  
  vtkSetMacro(InsideOut, bool);

protected:
  vtkExtractCellType();
  ~vtkExtractCellType();

  int RequestInformation(vtkInformation* req, vtkInformationVector** inInfo, vtkInformationVector* outInfo) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  void AppendToGeoTypes(vtkDataSet* dataset);
  void SelectIds(vtkDataSet* dataset, vtkIdTypeArray* selArr);

private:
  vtkExtractCellType(const vtkExtractCellType&) = delete;
  void operator=(const vtkExtractCellType&) = delete;
  
  vtkNew<vtkDataArraySelection> FieldSelection;
  int InsideOut = false;
};

#endif
