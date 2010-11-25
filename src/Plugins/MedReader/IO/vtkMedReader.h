#ifndef __vtkMedReader_h_
#define __vtkMedReader_h_

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkMedSetGet.h"

class vtkMedFile;
class vtkMedDriver;
class vtkDataArraySelection;
class vtkMedFieldOverEntity;
class vtkMedFieldStep;
class vtkMedField;
class vtkMedMesh;
class vtkMedFamily;
class vtkMedQuadratureDefinition;
class vtkMedEntityArray;
class vtkMedFamilyOnEntity;
class vtkMedProfile;
class vtkMedFieldStepOnMesh;
//BTX
class ComputeStep;
//ETX

class vtkUnstructuredGrid;
class vtkUnsignedCharArray;
class vtkIdList;
class vtkDoubleArray;
class vtkFieldData;
class vtkInformationDataObjectKey;
class vtkMutableDirectedGraph;

class VTK_EXPORT vtkMedReader: public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkMedReader* New();
vtkTypeRevisionMacro(vtkMedReader, vtkMultiBlockDataSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the file name to read from
  virtual void SetFileName(const char*);
  vtkGetStringMacro(FileName)

  // Description:
  // Is the given file a MED file?
  virtual int CanReadFile(const char* fname);

  // Description:
  // Get the file extensions for this format.
  // Returns a string with a space separated list of extensions in
  // the format .extension
  virtual const char*
  GetFileExtensions()
  {
    return ".med";
  }

  // Description:
  // Return a descriptive name for the file format that might be useful in a GUI.
  virtual const char*
  GetDescriptiveName()
  {
    return "MED file (Modele d'Echange de Donnees)";
  }

  // Description:
  // Get the DataArraySelection objects to select point/cell/quadrature fields.
  // These objects are initialized during the RequestInformation call.
  vtkGetObjectMacro(PointFields, vtkDataArraySelection)
  vtkGetObjectMacro(CellFields, vtkDataArraySelection)
  vtkGetObjectMacro(QuadratureFields, vtkDataArraySelection)

  // Description:
  // use these methods to enable/disable point arrays
  virtual int GetPointFieldArrayStatus(const char* name);
  virtual void SetPointFieldArrayStatus(const char* name, int status);
  virtual int GetNumberOfPointFieldArrays();
  virtual const char*
  GetPointFieldArrayName(int);

  // Description:
  // use these methods to enable/disable cell arrays
  virtual int GetCellFieldArrayStatus(const char* name);
  virtual void SetCellFieldArrayStatus(const char* name, int status);
  virtual int GetNumberOfCellFieldArrays();
  virtual const char*
  GetCellFieldArrayName(int);

  // Description:
  // use these methods to enable/disable quadrature arrays
  virtual int GetQuadratureFieldArrayStatus(const char* name);
  virtual void SetQuadratureFieldArrayStatus(const char* name, int status);
  virtual int GetNumberOfQuadratureFieldArrays();
  virtual const char*
  GetQuadratureFieldArrayName(int);

  // Description:
  // use this method to enable/disable cell types
  // the key is encoded with the vtkMedUtilities::CellTypeKey method which returns a string
  // MED_ENTITE_MAILLAGE/MED_GEOMETRIE_ELEMENT
  virtual void SetCellTypeStatus(const char* key, int flag);
  virtual int GetCellTypeStatus(int type, int geometry);

  // Description:
  // use this method to enable/disable a family support
  // the key is formatted by the vtkMedUtilities::FamilyKey method which returns a string
  // MESH_NAME/OnPoint/FAMILY_NAME or MESH_NAME/OnCell/FAMILY_NAME
  virtual void SetFamilyStatus(const char* key, int flag);
  virtual int GetFamilyStatus(vtkMedMesh*, vtkMedFamily*);

  // Description:
  // the animation mode modify the way the reader answers time requests from the pipeline.
  // Default (0) : this is PhysicalTime if there are times, or Iterations if there are only iterations
  // PhysicalTime (1) : the reader aggregates all physical times available in the file.
  // for a given time t, the reader will return for each field the ComputeStep with the highest time inferior
  // to t, and with the highest iteration.
  // Iteration (2) : you need to also set the Time field. In this case, the reader will understand
  // time requests has a request to iterate over iterations for the given time.
  // ModeShape (3) : the reader will output a deformed shape of the input, the time is understood as the phase.
  // times of the given ModeShapeField are understood has displacement modes.
  // all other fields are loaded at the given PhysicalTime.
  //BTX
  enum
  {
    Default = 0, PhysicalTime = 1, Iteration = 2, ModeShape = 3
  };
  //ETX
  vtkSetMacro(AnimationMode, int)
  vtkGetMacro(AnimationMode, int)

  // Description:
  // in med files, the ComputeSteps are defined by a pair <iteration, physicalTime>.
  // the PhysicalTime ivar is used if the TimeMode is either Iteration or ModeShape.
  // it fixes the PhysicalTime when iterating over iterations, or when the times are used as modeshapes
  // for a given field
  vtkSetMacro(Time, double)
  vtkGetMacro(Time, double)

  // Description:
  // returns the available times. Use this to get the times when in iteration mode, or the frequencies when in ModeShape mode.
  virtual vtkDoubleArray*
  GetAvailableTimes();

  // Description:
  // Build the graph used to pass information to the client on the supports
  virtual void BuildSIL(vtkMutableDirectedGraph*, int onlySelected = 0);

  // Description:
  // Every time the SIL is updated a this will return a different value.
  virtual int GetSILUpdateStamp();

  // Description:
  // reset the selection arrays without having to rebuild the SIL.
  virtual void ClearSelections();

  // Description:
  // The CacheStrategy indicates to the reader if it should cache some of the arrays.
  //BTX
  enum
  {
    CacheNothing, CacheGeometry, CacheGeometryAndFields
  };
  //ETX
  vtkSetMacro(CacheStrategy, int)
  vtkGetMacro(CacheStrategy, int)

  // Description :
  // release arrays read from MED (point coordinates, profile ids, family ids)
  void  ClearMedSupports();

  // Description :
  // release arrays read from MED (fields)
  void  ClearMedFields();


  // Description:
  // when answering to time requests, this reader tries to find the good time step.
  // if the time step is present in the file, it returns it. Else, it returns the nearest inferior step.
  // But due to some rounding errors, the pipeline may
  // ask for a pipeline which is very near an existing one, but slightly inferior.
  // the reader use this value to return the superior step in this case.
  // this value is relative to the difference of the two successive steps.
  vtkSetMacro(TimePrecision, double)
  vtkGetMacro(TimePrecision, double)

  // Description :
  // This key contains the SIL describing the data selected by the user.
  static vtkInformationDataObjectKey* SELECTED_SIL();

protected:
  vtkMedReader();
  virtual ~vtkMedReader();

  // Description:
  // This is called by the superclass.
  virtual int RequestDataObject(vtkInformation*, vtkInformationVector**,
      vtkInformationVector*);

  // Description:
  // This is called by the superclass.
  virtual int RequestInformation(vtkInformation*, vtkInformationVector**,
      vtkInformationVector*);

  // Description:
  // This is called by the superclass.
  virtual int RequestData(vtkInformation*, vtkInformationVector**,
      vtkInformationVector*);

  virtual void SetMedDriver(vtkMedDriver*);
  vtkGetObjectMacro(MedDriver, vtkMedDriver)

  virtual void SetMedFile(vtkMedFile*);
  vtkGetObjectMacro(MedFile, vtkMedFile)

  // Description:
  // create a driver adapted to the given file.
  // the user has the responsibility to delete it.
  vtkMedDriver*
  NewMedDriver(const char*);

  // Description:
  // Gather all compute steps in the fields
  virtual void GatherComputeSteps();

  // Description:
  // Give the animation steps to the pipeline
  virtual void AdvertiseTime(vtkInformation*);

  // Description:
  // returns 1 if at least one the families of this mesh is selected, 0 otherwise.
  virtual int IsMeshSelected(int);

  // Description:
  // returns if the field is selected.
  virtual int IsFieldSelected(vtkMedField*);
  virtual int IsPointFieldSelected(vtkMedField*);
  virtual int IsCellFieldSelected(vtkMedField*);
  virtual int IsQuadratureFieldSelected(vtkMedField*);

  virtual void AddQuadratureSchemeDefinition(vtkInformation*,
      vtkMedQuadratureDefinition*);

  virtual void BuildUnstructuredGridForCellSupport(vtkMedFamilyOnEntity*,
      vtkUnstructuredGrid*);
  virtual void BuildUnstructuredGridForPointSupport(vtkMedFamilyOnEntity*,
      vtkUnstructuredGrid*);

  virtual bool KeepCell(vtkMedFamily*, vtkMedEntityArray*, vtkIdType);
  virtual bool KeepPoint(vtkMedFamily*, vtkMedMesh*, vtkIdType);
  virtual bool KeepPoint(vtkMedFamilyOnEntity*, vtkIdType);

  // Description:
  // this method is used internally to update all the arrays (float or double) for modal analysis.
  // The array values are following a sinusoidal curve between -PI and +PI.
  void ComputeModalFields(vtkFieldData* attributes);

  // Description:
  // This method is used to update the values of a given array for modal analysis.
  void ComputeModalField(vtkDataArray* attribute, vtkDataArray* newAttribute);
  //BTX
  enum
  {
    Initialize,
    StartRequest,
    AfterCreateMedSupports,
    EndBuildVTKSupports,
    EndRequest
  };
  //ETX
  virtual void ClearCaches(int when);

  virtual void CreateMedSupports();

  virtual void BuildVTKSupport(vtkMedFamilyOnEntity*);

  virtual void MapFieldsOnSupport(vtkMedFamilyOnEntity* foe);

  //BTX
  virtual vtkMultiBlockDataSet* GetParent(const vtkList<vtkstd::string>& path);
  //ETX

  virtual int IsFieldOnSupport(vtkMedField*, vtkMedFamilyOnEntity*);

  virtual void CreateVTKFieldOnSupport(vtkMedField*, vtkMedFamilyOnEntity*);

  virtual void FlagUsedPointsOnCellSupport(vtkMedFamilyOnEntity*,
      vtkUnsignedCharArray*);

  virtual int CanShallowCopyMed2VTK(vtkMedField*, vtkMedFamilyOnEntity*);

  virtual int IsCellFamilyOnEntityOnCellProfile(vtkMedFamilyOnEntity*,
      vtkMedProfile*, bool exact);

  virtual int IsCellFamilyOnEntityOnPointProfile(vtkMedFamilyOnEntity*,
      vtkMedProfile*, bool exact);

  virtual int IsPointFamilyOnEntityOnPointProfile(vtkMedFamilyOnEntity*,
      vtkMedProfile*, bool exact);

  virtual int IsFamilyOnEntityOnProfile(vtkMedFamilyOnEntity*, vtkMedProfile*,
      int fieldSupportType, bool exact);

  virtual void InitializeQuadratureOffsets(vtkMedField*, vtkMedFamilyOnEntity*);

  virtual void GetCellPoints(vtkMedMesh*, vtkMedEntityArray*, vtkIdType,
      vtkIdList*);

  virtual void SetVTKFieldOnSupport(vtkMedField*, vtkMedFamilyOnEntity*);

  // Description:
  // update the internal cache with the step associated to this field.
  virtual void SelectStep(vtkMedField*);

  // Description:
  // returns the ComputeStep asked by the time request, depending on the chosen animation mode
  vtkMedFieldStep*
  GetSelectedStep(vtkMedField*, vtkMedFieldOverEntity*, int timeIndex,
      int iterationIndex);

  // Description:
  // returns the index of the time step to use.
  // if the given time exists in the available times, returns its index.
  // If not, returns the index of the immediate inferior time. A small threshold is applied to get
  // superior time if the asked time is very near an existing time. (the TimePrecision is relative to the difference
  // between the 2 successive steps.
  virtual int GetTimeIndex(vtkMedField*, double);
  virtual int GetIterationIndex(vtkMedField*, int, double);
  virtual int GetLastIterationIndex(vtkMedField*, int);

  virtual void ChooseRealAnimationMode();

  virtual vtkMedProfile* GetProfile(vtkMedMesh*, vtkMedField*,
      vtkMedFieldOverEntity*, vtkMedFieldStep*, vtkMedFieldStepOnMesh*);

  virtual vtkMedQuadratureDefinition*
  GetQuadratureDefinition(vtkMedMesh*, vtkMedField*, vtkMedFieldOverEntity*,
      vtkMedFieldStep*, vtkMedFieldStepOnMesh*);

  virtual int GetQuadratureDefinitionKey(vtkMedMesh*, vtkMedField*,
      vtkMedFieldOverEntity*, vtkMedFieldStep*, vtkMedFieldStepOnMesh*);

  // Description :
  // Load the connectivity of this entities, and also those of the sub-entities in the case of non nodal connectivity.
  virtual void  LoadConnectivity(vtkMedMesh*, vtkMedEntityArray*);

  virtual void  InitializeCellGlobalIds();

  // Description :
  // returns 1 if any point/cell family of this mesh is selected.
  int HasMeshAnyCellSelectedFamily(vtkMedMesh* );
  int HasMeshAnyPointSelectedFamily(vtkMedMesh* );


  // Field selections
  vtkDataArraySelection* PointFields;
  vtkDataArraySelection* CellFields;
  vtkDataArraySelection* QuadratureFields;

  // Support selection
  vtkDataArraySelection* Entities;
  vtkDataArraySelection* Families;

  // name of the file to read from
  char* FileName;

  // time management
  int AnimationMode;
  double Time;
  double TimePrecision;
  vtkDoubleArray* AvailableTimes;

  // store the internal structure of the file
  vtkMedFile* MedFile;

  // store the internal structure of the file
  vtkMedDriver* MedDriver;

  int CacheStrategy;

  //BTX
  class vtkMedReaderInternal;
  vtkMedReaderInternal* Internal;
  //ETX

private:
  vtkMedReader(const vtkMedReader&); // Not implemented.
  void operator=(const vtkMedReader&); // Not implemented.
};

#endif //__vtkMedReader_h_
