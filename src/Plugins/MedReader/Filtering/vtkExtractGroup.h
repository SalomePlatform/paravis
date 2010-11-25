#ifndef vtkExtractGroup_h__
#define vtkExtractGroup_h__

#include "vtkMultiBlockDataSetAlgorithm.h"
class vtkMutableDirectedGraph;
class vtkInformation;
class vtkInformationVector;
class vtkDataArraySelection;
class vtkTimeStamp;

class vtkExtractGroup : public vtkMultiBlockDataSetAlgorithm
{
public : 
  static vtkExtractGroup* New();
  vtkTypeRevisionMacro(vtkExtractGroup, vtkMultiBlockDataSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // use this method to enable/disable cell types
  // the key is encoded with the vtkMedUtilities::CellTypeKey method which returns a string
  // MED_ENTITE_MAILLAGE/MED_GEOMETRIE_ELEMENT
  virtual void SetCellTypeStatus(const char* key, int flag);

  // Description:
  // use this method to enable/disable a family support
  // the key is formatted by the vtkMedUtilities::FamilyKey method which returns a string
  // MESH_NAME/OnPoint/FAMILY_NAME or MESH_NAME/OnCell/FAMILY_NAME
  virtual void SetFamilyStatus(const char* key, int flag);

  // Description:
  // Every time the SIL is updated a this will return a different value.
  virtual int GetSILUpdateStamp();

  // Description :
  // If set to 1, this filter will prune the empty parts in the output.
  // If not, then empty datasets will be kept
  vtkSetMacro(PruneOutput, int);
  vtkGetMacro(PruneOutput, int);

  int ModifyRequest(vtkInformation* request, int when);


protected :
  vtkExtractGroup();
  ~vtkExtractGroup();

  int RequestInformation(vtkInformation *request,
      vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  int RequestData(vtkInformation *request,
      vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  // Description :
  // returns 1 if this cell type is to be passed through
  int IsCellTypeSelected(const char*);

  // Description :
  // returns 1 if this family is to be passed through
  int IsFamilySelected(const char* meshName, const char* cellOrPoint, const char* familyName);

  // Description :
  // removes empty blocks from the vtkMultiBlockDataSet.
  void PruneEmptyBlocks(vtkMultiBlockDataSet* mb);

  // Description:
  // This SIL stores the structure of the mesh/groups/cell types that can be selected.
  virtual void  SetSIL(vtkMutableDirectedGraph*);
  vtkGetObjectMacro(SIL, vtkMutableDirectedGraph);

  virtual void BuildDefaultSIL(vtkMutableDirectedGraph*);

  vtkMutableDirectedGraph* SIL;

  // Support selection
  vtkDataArraySelection* Entities;
  vtkDataArraySelection* Families;

  int PruneOutput;

private :
  vtkExtractGroup(const vtkExtractGroup&);
  void operator=(const vtkExtractGroup&); // Not implemented.
};

#endif
