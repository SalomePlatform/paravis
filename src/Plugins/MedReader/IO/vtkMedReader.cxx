#include "vtkMedReader.h"

#include "vtkMedFile.h"
#include "vtkMedFactory.h"
#include "vtkMedDriver.h"
#include "vtkMedUtilities.h"
#include "vtkMedFamily.h"
#include "vtkMedGroup.h"
#include "vtkMedMesh.h"
#include "vtkMedUnstructuredGrid.h"
#include "vtkMedEntityArray.h"
#include "vtkMedAttribute.h"
#include "vtkMedField.h"
#include "vtkMedString.h"
#include "vtkMedFieldStep.h"
#include "vtkMedFieldStepOnMesh.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedProfile.h"
#include "vtkMedIntArray.h"
#include "vtkMedQuadratureDefinition.h"
#include "vtkMedFamilyOnEntity.h"

#include "vtkObjectFactory.h"
#include "vtkDataArraySelection.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkStringArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkUnsignedCharArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkSmartPointer.h"
#include "vtkVariantArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkExecutive.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkCellType.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkConfigure.h"

#include "vtkSMDoubleVectorProperty.h"
#include "vtkInformationDataObjectKey.h"

vtkInformationKeyMacro(vtkMedReader, SELECTED_SIL, DataObject)
;

#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
using namespace std;

struct VTKField
{
  vtkSmartPointer<vtkDataArray> DataArray;
  vtkSmartPointer<vtkIdTypeArray> QuadratureIndexArray;
};

struct TimedFieldOverEntity
{
  vtkMedFieldOverEntity* Entity;
  vtkMedFieldStep* Step;
  vtkMedFieldStepOnMesh* StepOnMesh;
};

class ComputeStep: public pair<int, int>
{
};

struct TimedField: public pair<vtkMedField*, ComputeStep>
{
};

// list of quadrature offset index in the file
typedef med_int QuadratureKey;

bool operator<(const TimedFieldOverEntity& x, const TimedFieldOverEntity& y)
{
  if(x.Entity!=y.Entity)
    return x.Entity<y.Entity;

  if(x.Step!=y.Step)
    return x.Step<y.Step;

  return x.StepOnMesh<y.StepOnMesh;
}

class vtkMedReader::vtkMedReaderInternal
{
public:
  int NumberOfPieces;
  int CurrentPieceNumber;
  int GhostLevel;
  double UpdateTimeStep;
  vtkTimeStamp FileNameMTime;
  vtkTimeStamp MetaDataMTime;
  vtkTimeStamp GroupSelectionMTime;
  vtkTimeStamp FamilySelectionMTime;
  int SILUpdateStamp;
  int RealAnimationMode;
  vtkDataArraySelection* Families;

  // for each vtkMedField, store all the ordered time steps, and for each time store the ordered iterations
  map<vtkMedField*, vector<pair<double, vector<med_int> > > > StepCache;
  map<vtkMedField*, map<double, med_int> > TimeIds;

  // Store the vtkMutableDirectedGraph that represents links between family, groups and attributes, and cell types
  vtkMutableDirectedGraph* SIL;
  vtkMutableDirectedGraph* SelectedSIL;

  // this map is used to keep clean data sets in the cache, without any field.
  // for each support, store the vtkDataSet
  map<vtkMedFamilyOnEntity*, vtkSmartPointer<vtkDataSet> > DataSetCache;

  // this is the current dataset for the given support.
  map<vtkMedFamilyOnEntity*, vtkDataSet*> CurrentDataSet;

  // for each support, cache the VTK arrays that correspond to a given field at the given step.
  map<vtkMedFamilyOnEntity*, map<TimedField, VTKField> > FieldCache;
  map<vtkMedFamilyOnEntity*, map<TimedField, bool> > FieldMatchCache;

  // for each FamilyOnEntity, cache the TimedFieldOverEntity.
  map<TimedField, map<vtkMedFamilyOnEntity*, TimedFieldOverEntity> >
      FieldOnFamilyOnEntityCache;

  // This list keep tracks of all the selected supports
  set<vtkMedFamilyOnEntity*> UsedSupports;

  // this map keeps for each support, the quadrature offset array so that different fields on the same support can use
  // the same offset array, provided they use the same gauss points definitions (given by the index in the med file)
  map<vtkMedFamilyOnEntity*,
      map<QuadratureKey, vtkSmartPointer<vtkIdTypeArray> > >
      QuadratureOffsetCache;

  map<vtkMedFamilyOnEntity*, map<TimedField, QuadratureKey> > QuadOffsetKey;

  map<pair<vtkMedFamilyOnEntity*, vtkMedProfile*> , pair<bool, bool> >
      ProfileMatch;

  map<vtkMedField*, ComputeStep> CurrentStep;

  map<vtkMedFamilyOnEntity*, vtkstd::map<vtkIdType, vtkIdType> >
      Med2VTKPointIndex;

  map<vtkMedFamilyOnEntity*, vtkSmartPointer<vtkUnsignedCharArray> >
      UsedPointsOnCellSupportCache;

  vtkMedReaderInternal()
  {
    this->SIL=vtkMutableDirectedGraph::New();
    this->SelectedSIL=vtkMutableDirectedGraph::New();
    this->SILUpdateStamp=-1;
    this->RealAnimationMode=vtkMedReader::PhysicalTime;
    this->Families=vtkDataArraySelection::New();
  }
  ~vtkMedReaderInternal()
  {
    this->SIL->Delete();
    this->SelectedSIL->Delete();
    this->Families->Delete();
  }

  void ClearSupportCache(vtkMedFamilyOnEntity* foe)
  {
    this->Med2VTKPointIndex.erase(foe);
    this->QuadratureOffsetCache.erase(foe);
    this->FieldMatchCache.erase(foe);
    this->FieldCache.erase(foe);
    this->CurrentDataSet.erase(foe);
    this->DataSetCache.erase(foe);
  }

  vtkIdType GetChild(vtkIdType parent, const vtkStdString childName)
  {
    vtkStringArray* names=vtkStringArray::SafeDownCast(
        this->SIL->GetVertexData()->GetArray("Names"));
    if(names==NULL)
      return -1;
    vtkIdType nedges=this->SIL->GetOutDegree(parent);
    for(vtkIdType id=0; id<nedges; id++)
      {
      vtkOutEdgeType edge=this->SIL->GetOutEdge(parent, id);
      if(names->GetValue(edge.Target)==childName)
        return edge.Target;
      }
    return -1;
  }

  vtkIdType GetGroupId(const char* key)
  {
    vtkstd::string meshname, celltypename, groupname;
    vtkMedUtilities::SplitGroupKey(key, meshname, celltypename, groupname);
    vtkIdType root=GetChild(0, "SIL");
    if(root==-1)
      return -1;
    vtkIdType mesh=GetChild(root, meshname);
    if(mesh==-1)
      return -1;
    vtkIdType type=GetChild(mesh, celltypename);
    if(type==-1)
      return -1;
    return GetChild(type, groupname);

  }

};

vtkSetObjectImplementationMacro(vtkMedReader,MedDriver,vtkMedDriver)
vtkSetObjectImplementationMacro(vtkMedReader,MedFile,vtkMedFile)

vtkCxxRevisionMacro(vtkMedReader, "$Revision$")
vtkStandardNewMacro(vtkMedReader)

vtkMedReader::vtkMedReader()
{
  this->FileName=NULL;
  this->SetNumberOfInputPorts(0);
  this->PointFields=vtkDataArraySelection::New();
  this->CellFields=vtkDataArraySelection::New();
  this->QuadratureFields=vtkDataArraySelection::New();
  this->Entities=vtkDataArraySelection::New();
  this->Groups=vtkDataArraySelection::New();
  this->AnimationMode=Default;
  this->Time=0.0;
  this->MedFile=NULL;
  this->MedDriver=NULL;
  this->CacheStrategy=CacheGeometry;
  this->Internal=new vtkMedReaderInternal;
  this->TimePrecision=0.00001;
  this->AvailableTimes=vtkDoubleArray::New();
}

vtkMedReader::~vtkMedReader()
{
  this->SetMedDriver(NULL);
  this->SetFileName(NULL);
  this->PointFields->Delete();
  this->CellFields->Delete();
  this->QuadratureFields->Delete();
  this->Entities->Delete();
  this->Groups->Delete();
  this->SetMedFile(NULL);
  delete this->Internal;
  this->AvailableTimes->Delete();
}

int vtkMedReader::GetSILUpdateStamp()
{
  return this->Internal->SILUpdateStamp;
}

void vtkMedReader::SetFileName(const char* fname)
{
  int modified=0;
  if(fname==this->FileName)
    return;
  if(fname&&this->FileName&&!strcmp(fname, this->FileName))
    return;
  modified=1;
  if(this->FileName)
    delete[] this->FileName;
  if(fname)
    {
    size_t fnl=strlen(fname)+1;
    char* dst=new char[fnl];
    const char* src=fname;
    this->FileName=dst;
    do
      {
      *dst++=*src++;
      }
    while(--fnl);
    }
  else
    {
    this->FileName=0;
    }
  if(modified)
    {
    this->Modified();
    this->SetMedFile(NULL);
    this->SetMedDriver(NULL);
    this->Internal->FileNameMTime.Modified();
    }
}

int vtkMedReader::CanReadFile(const char* fname)
{
  // the factory give a driver only when it can read the file version,
  // or it returns a NULL pointer.
  vtkMedDriver* driver=NewMedDriver(fname);

  if(driver==NULL)
    return 0;

  driver->Delete();
  return 1;
}

int vtkMedReader::RequestInformation(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation* outInfo=outputVector->GetInformationObject(0);
  if(this->Internal->MetaDataMTime<=this->Internal->FileNameMTime)
    {
    this->ClearCaches(Initialize);

    vtkInformation* outInfo=outputVector->GetInformationObject(0);

    vtkMedDriver* driver=NewMedDriver(this->FileName);
    this->SetMedDriver(driver);
    // if the returned driver is NULL, this means that the file cannot be opened.
    if(driver)
      {
      driver->Delete();

      vtkMedFile* file=vtkMedFile::New();
      this->SetMedFile(file);
      file->Delete();

      driver->ReadMeta(file);
      this->InitializeCellGlobalIds();

      this->ClearSelections();

      this->BuildSIL(this->Internal->SIL);
      this->Internal->SILUpdateStamp++;

      this->GatherComputeSteps();

      this->Internal->MetaDataMTime.Modified();
      }
    else
      {
      vtkDebugMacro("the FileName not set or this med file is not readable by the linked med library");
      return 1;
      }
    }

  if(this->Internal->SelectedSIL->GetMTime()<=this->GetMTime())
    {
    this->BuildSIL(this->Internal->SelectedSIL, 1);
    }
  outInfo->Set(vtkMedReader::SELECTED_SIL(), this->Internal->SelectedSIL);
  vtkInformationKey* key=vtkMedReader::SELECTED_SIL();
  request->AppendUnique(vtkExecutive::KEYS_TO_COPY(), key);

  outInfo->Set(vtkDataObject::SIL(), this->Internal->SIL);
  this->AdvertiseTime(outInfo);
  return 1;
}

int vtkMedReader::RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  if(this->GetMedFile()==NULL)
    {
    vtkWarningMacro( << "FileName must be set and meta data loaded");
    return 0;
    }

  vtkInformation *info=outputVector->GetInformationObject(0);
  vtkMultiBlockDataSet *output=vtkMultiBlockDataSet::SafeDownCast(info->Get(
      vtkDataObject::DATA_OBJECT()));

  if(info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES()))
    {
    this->Internal->NumberOfPieces=info->Get(
        vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
    }
  else
    {
    this->Internal->NumberOfPieces=-1;
    }
  if(info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()))
    {
    this->Internal->CurrentPieceNumber=info->Get(
        vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
    }
  else
    {
    this->Internal->CurrentPieceNumber=-1;
    }

  if(info->Has(
      vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS()))
    {
    this->Internal->GhostLevel=info->Get(
        vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());
    }
  else
    {
    this->Internal->GhostLevel=0;
    }

  if(info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()))
    {
    this->Internal->UpdateTimeStep=info->Get(
        vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS())[0];
    }
  else
    {
    this->Internal->UpdateTimeStep=0;
    }

  output->Initialize();

  this->ChooseRealAnimationMode();

  vtkMedDriver::FileOpen open(this->MedDriver);

  // clear the dataset cache if the selected entities have been modified,
  // because the families to create may contain entities which should (or not) be there.
  //TODO : clear the caches that need clearing
  this->ClearCaches(StartRequest);

  // This call create the vtkMedSupports, but do not create the corresponding vtkDataSet;
  this->CreateMedSupports();
  this->ClearCaches(AfterCreateMedSupports);
  // This call creates the actual vtkDataSet that corresponds to each support
  int progress=0;
  int maxprogress=2*this->Internal->UsedSupports.size();
  for(set<vtkMedFamilyOnEntity*>::iterator it=
      this->Internal->UsedSupports.begin(); it
      !=this->Internal->UsedSupports.end(); it++)
    {
    ostringstream sstr;
    sstr<<"Support : "<<vtkMedUtilities::SimplifyName(
        (*it)->GetFamily()->GetName());
    this->SetProgressText(sstr.str().c_str());
    this->BuildVTKSupport(*it);
    this->UpdateProgress((float) progress/((float) maxprogress-1));
    progress++;
    }

  this->ClearCaches(EndBuildVTKSupports);

  // This call maps the fields to the supports
  for(set<vtkMedFamilyOnEntity*>::iterator it=
      this->Internal->UsedSupports.begin(); it
      !=this->Internal->UsedSupports.end(); it++)
    {
    ostringstream sstr;
    sstr<<"Loading fields on "<<vtkMedUtilities::SimplifyName(
        (*it)->GetFamily()->GetName());
    this->SetProgressText(sstr.str().c_str());
    this->MapFieldsOnSupport(*it);
    this->UpdateProgress((float) progress/((float) maxprogress-1));
    progress++;
    }

  if(this->Internal->RealAnimationMode==ModeShape)
    {
    for(map<vtkMedFamilyOnEntity*, vtkDataSet*>::iterator it=
        this->Internal->CurrentDataSet.begin(); it
        !=this->Internal->CurrentDataSet.end(); it++)
      {
      ComputeModalFields(it->second->GetPointData());
      ComputeModalFields(it->second->GetCellData());
      ComputeModalFields(it->second->GetFieldData());
      } // end for, iteration over all supports
    }
  // This call clean up caches (what is actually done depends of the CacheStrategy)
  this->ClearCaches(EndRequest);
  return 1;
}

void vtkMedReader::InitializeCellGlobalIds()
{
  vtkIdType totalCells=0;
  for(int m=0; m<this->MedFile->GetNumberOfMesh(); m++)
    {
    vtkMedGrid* grid=this->MedFile->GetMesh(m)->GetGrid();
    grid->InitializeCellGlobalIds();
    }
}

void vtkMedReader::ComputeModalFields(vtkFieldData* attributes)
{
  vtkstd::vector<vtkSmartPointer<vtkDataArray> > olddata;
  vtkstd::vector<int> attributeType;
  vtkDataSetAttributes* dsatt=vtkDataSetAttributes::SafeDownCast(attributes);
  int unmodifiedFieldIndex=0;
  while(attributes->GetNumberOfArrays()>unmodifiedFieldIndex)
    {
    vtkDataArray* array=attributes->GetArray(unmodifiedFieldIndex);
    // if the array is not float or double, it is not modified
    if(array->GetDataType()!=VTK_DOUBLE&&array->GetDataType()!=VTK_FLOAT)
      {
      unmodifiedFieldIndex++;
      }
    else
      {
      olddata.push_back(array);
      if(dsatt!=NULL)
        attributeType.push_back(dsatt->IsArrayAnAttribute(unmodifiedFieldIndex));
      else
        attributeType.push_back(-1);
      attributes->RemoveArray(array->GetName());
      }
    }

  for(int j=0; j<olddata.size(); j++)
    {
    vtkDataArray* attribute=olddata[j];

    vtkDataArray* newAttribute=attribute->NewInstance();

    this->ComputeModalField(attribute, newAttribute);

    attributes->AddArray(newAttribute);
    newAttribute->Delete();

    if(attributeType[j]>=0)
      dsatt->SetActiveAttribute(newAttribute->GetName(), attributeType[j]);

    } // for loop
}

void vtkMedReader::ComputeModalField(vtkDataArray* attribute,
    vtkDataArray* newAttribute)
{
  // Next code gives support to animation of frequency modes
  // Fields are animated using a sinus function which values, in the usual case,
  // vary between -PI an +PI

  vtkIdType num_tuples=attribute->GetNumberOfTuples();

  newAttribute->SetNumberOfComponents(attribute->GetNumberOfComponents());
#if (VTK_MAJOR_VERSION >= 5) and (VTK_MINOR_VERSION >= 7)
  for(int comp=0; comp<attribute->GetNumberOfComponents(); comp++)
    {
    newAttribute->SetComponentName(comp, attribute->GetComponentName(comp));
    }
#endif
  newAttribute->SetNumberOfTuples(num_tuples);
  newAttribute->SetName(attribute->GetName());
  if(attribute->HasInformation())
    {
    vtkInformation* oldinfo=attribute->GetInformation();
    vtkInformation* newinfo=newAttribute->GetInformation();
    newinfo->Copy(oldinfo);
    }

  double *newtuple=new double[attribute->GetNumberOfComponents()];
  vtkIdType i;
  for(i=0; i<num_tuples; i++)
    {
    double* x=attribute->GetTuple(i);

    double sinShift=sin(this->Internal->UpdateTimeStep);
    for(int comp=0; comp<attribute->GetNumberOfComponents(); comp++)
      {
      newtuple[comp]=x[comp]*sinShift;
      }
    newAttribute->SetTuple(i, newtuple);
    }
  delete[] newtuple;

  // override the array range to the -max / +max range.
  for(int comp=-1; comp<attribute->GetNumberOfComponents(); comp++)
    {
    double * range=attribute->GetRange(comp);
    double max, abs;
    max=(range[0]>0? range[0]: -range[0]);
    abs=(range[1]>0? range[1]: -range[1]);
    if(abs>max)
      max=abs;

    vtkInformation* info=newAttribute->GetInformation();
    if(comp==-1)
      {
      double r[2]={0, max};
      info->Set(vtkDataArray::L2_NORM_RANGE(), r, 2);
      }
    else
      {
      vtkInformationVector* infoVec;
      if(!info->Has(vtkDataArray::PER_COMPONENT()))
        {
        infoVec=vtkInformationVector::New();
        info->Set(vtkDataArray::PER_COMPONENT(), infoVec);
        infoVec->FastDelete();
        }
      else
        {
        infoVec=info->Get(vtkDataArray::PER_COMPONENT());
        }
      int vlen=infoVec->GetNumberOfInformationObjects();
      if(vlen<newAttribute->GetNumberOfComponents())
        {
        infoVec->SetNumberOfInformationObjects(
            newAttribute->GetNumberOfComponents());
        }
      double r[2]={-max, max};
      infoVec->GetInformationObject(comp)->Set(vtkDataArray::COMPONENT_RANGE(),
          r, 2);
      }
    }

}

struct compTimes
{
  bool operator()(pair<double, int> i, pair<double, int> j)
  {
    return (i.first<j.first);
  }
};

vtkDoubleArray* vtkMedReader::GetAvailableTimes()
{
  this->AvailableTimes->Initialize();
  this->AvailableTimes->SetNumberOfComponents(2);

  if(this->MedFile==NULL)
    return this->AvailableTimes;

  set<pair<double, int> > times;
  double auxd;
  int auxi;

  for(int fid=0; fid<this->MedFile->GetNumberOfField(); fid++)
    {
    vtkMedField* field=this->MedFile->GetField(fid);
    for(int tid=0; tid<this->Internal->StepCache[field].size(); tid++)
      {
      auxd=this->Internal->StepCache[field][tid].first;
      auxi
          =this->Internal->StepCache[field][tid].second[this->Internal->StepCache[field][tid].second.size()
              -1];
      times.insert(pair<double, int> (auxd, auxi));
      }
    }

  compTimes cT;
  vector<pair<double, int> > sortedtimes;
  for(set<pair<double, int> >::iterator it=times.begin(); it!=times.end(); it++)
    sortedtimes.push_back(*it);
  sort(sortedtimes.begin(), sortedtimes.end(), cT);

  double *st=new double[2];
  for(int tid=0; tid<sortedtimes.size(); tid++)
    {
    st[0]=sortedtimes[tid].first;
    st[1]=(double) sortedtimes[tid].second;
    this->AvailableTimes->InsertNextTupleValue(st);
    }

  return this->AvailableTimes;
}

void vtkMedReader::ChooseRealAnimationMode()
{
  if(this->AnimationMode!=Default)
    {
    this->Internal->RealAnimationMode=this->AnimationMode;
    return;
    }
  bool hasTime=false;
  bool hasIteration=false;
  for(int fid=0; fid<this->MedFile->GetNumberOfField(); fid++)
    {
    vtkMedField* field=this->MedFile->GetField(fid);
    vector<pair<double, vector<med_int> > >& steps=
        this->Internal->StepCache[field];
    if(steps.size()>1)
      {
      hasTime=true;
      break;
      }
    else if(steps[0].second.size()>1)
      {
      hasIteration=true;
      }
    }
  if(hasTime)
    {
    this->Internal->RealAnimationMode=PhysicalTime;
    return;
    }
  if(hasIteration)
    {
    this->Internal->RealAnimationMode=Iteration;
    return;
    }
  this->Internal->RealAnimationMode=PhysicalTime;
}

void vtkMedReader::SelectStep(vtkMedField* field)
{
  int timeid;
  int iterid;
  if(this->Internal->RealAnimationMode==PhysicalTime)
    {
    timeid=this->GetTimeIndex(field, this->Internal->UpdateTimeStep);
    iterid=this->GetLastIterationIndex(field, timeid);
    }
  else if(this->Internal->RealAnimationMode==Iteration)
    {
    timeid=this->GetTimeIndex(field, this->Time);
    iterid=this->GetIterationIndex(field, timeid,
        this->Internal->UpdateTimeStep);
    }
  else //mode shape
    {
    timeid=this->GetTimeIndex(field, this->Time);
    iterid=this->GetLastIterationIndex(field, timeid);
    }
  ComputeStep cs;
  pair<double, vector<med_int> >& cache=
      this->Internal->StepCache[field][timeid];
  cs.first=this->Internal->TimeIds[field][cache.first];
  cs.second=cache.second[iterid];
  this->Internal->CurrentStep[field]=cs;
}

vtkMedFieldStep* vtkMedReader::GetSelectedStep(vtkMedField* field,
    vtkMedFieldOverEntity* soe, int medtimeno, int mediterno)
{
  for(int stepid=0; stepid<soe->GetNumberOfStep(); stepid++)
    {
    vtkMedFieldStep* step=soe->GetStep(stepid);
    if(step->GetTimeId()==medtimeno&&step->GetIteration()==mediterno)
      return step;
    }
  return NULL;
}

int vtkMedReader::GetCellTypeStatus(int type, int geometry)
{
  if(type==MED_NOEUD)
    return 1;
  return this->Entities->GetArraySetting(vtkMedUtilities::CellTypeKey(
      (med_entite_maillage) type, (med_geometrie_element) geometry).c_str());
}

int vtkMedReader::GetFamilyStatus(vtkMedMesh* mesh, vtkMedFamily* family)
{
  if(!mesh||!family)
    return 0;

  if(this->Internal->GroupSelectionMTime>this->Internal->FamilySelectionMTime)
    {
    this->SelectFamiliesFromGroups();
    }

  return this->Internal->Families->GetArraySetting(vtkMedUtilities::FamilyKey(
      mesh->GetName()->GetString(), family->GetPointOrCell(),
      family->GetName()->GetString()).c_str());
}

int vtkMedReader::IsMeshSelected(int index)
{
  vtkMedMesh* mesh=this->MedFile->GetMesh(index);
  if(!mesh)
    return 0;

  for(int fam=0; fam<mesh->GetNumberOfFamily(); fam++)
    {
    if(this->GetFamilyStatus(mesh, mesh->GetFamily(fam))!=0)
      return 1;
    }
  return 0;
}

void vtkMedReader::GatherComputeSteps()
{
  this->Internal->StepCache.clear();
  this->Internal->TimeIds.clear();
  for(int fieldId=0; fieldId<this->GetMedFile()->GetNumberOfField(); fieldId++)
    {
    vtkMedField* field=this->GetMedFile()->GetField(fieldId);
    this->Internal->TimeIds[field]=map<double, med_int> ();
    set<pair<med_float, med_int> > stepset;
    // first loop to get all available compute steps
    for(int entityId=0; entityId<field->GetNumberOfFieldOverEntity(); entityId++)
      {
      vtkMedFieldOverEntity* foe=field->GetFieldOverEntity(entityId);
      for(int stepId=0; stepId<foe->GetNumberOfStep(); stepId++)
        {
        vtkMedFieldStep* step=foe->GetStep(stepId);
        stepset.insert(make_pair(step->GetTime(), step->GetIteration()));
        this->Internal->TimeIds[field][step->GetTime()]=step->GetTimeId();
        }
      }
    if(stepset.size()==0)
      continue;
    // second loop to verify that all steps are coherent for all entities and the stepId is coherent for all times
    for(int entityId=0; entityId<field->GetNumberOfFieldOverEntity(); entityId++)
      {
      vtkMedFieldOverEntity* foe=field->GetFieldOverEntity(entityId);
      if(stepset.size()!=foe->GetNumberOfStep())
        {
        vtkWarningMacro(
            "all entities do not share the same number of time steps or iterations for the field "
            << field->GetName()->GetString()
            << ", incoherent results may happen.");
        }
      else
        {
        for(int stepId=0; stepId<foe->GetNumberOfStep(); stepId++)
          {
          vtkMedFieldStep* step=foe->GetStep(stepId);
          pair<med_float, med_int> compute_step=make_pair(step->GetTime(),
              step->GetIteration());
          if(stepset.find(compute_step)==stepset.end())
            {
            vtkWarningMacro(
                "all entities do not share the same time steps or iterations for the field "
                << field->GetName()->GetString()
                << ", incoherent results may happen.");
            }
          if(this->Internal->TimeIds[field][step->GetTime()]!=step->GetTimeId())
            {
            vtkWarningMacro("the TimeStepNumber is not coherent for field "
                << field->GetName()->GetString()
                << "incoherent results may happen.");
            }
          }
        }
      }
    set<pair<med_float, med_int> >::iterator it=stepset.begin();
    deque<pair<med_float, med_int> > steps;
    while(it!=stepset.end())
      {
      steps.push_back(*it);
      it++;
      }
    sort(steps.begin(), steps.end());
    deque<pair<med_float, med_int> >::iterator dit=steps.begin();
    if(this->Internal->StepCache.find(field)==this->Internal->StepCache.end())
      this->Internal->StepCache[field]
          =vector<pair<double, vector<med_int> > > ();
    vector<pair<double, vector<med_int> > >& cache=
        this->Internal->StepCache[field];
    // third loop to fill the cache
    cache.push_back(make_pair(dit->first, vector<med_int> ()));
    int currentTimeIndex=0;
    while(dit!=steps.end())
      {
      med_float time=dit->first;
      med_int iter=dit->second;
      if(time!=cache[currentTimeIndex].first)
        {
        cache.push_back(make_pair(time, vector<med_int> ()));
        currentTimeIndex++;
        }
      cache[currentTimeIndex].second.push_back(iter);
      dit++;
      }
    }//fieldID
}

int vtkMedReader::IsFieldSelected(vtkMedField* field)
{
  return this->IsPointFieldSelected(field)||this->IsCellFieldSelected(field)
      ||this->IsQuadratureFieldSelected(field);
}

int vtkMedReader::IsPointFieldSelected(vtkMedField* field)
{
  return field->GetType()==vtkMedField::PointField
      &&this->GetPointFieldArrayStatus(vtkMedUtilities::SimplifyName(
          field->GetName()).c_str());
}

int vtkMedReader::IsCellFieldSelected(vtkMedField* field)
{
  return field->GetType()==vtkMedField::CellField
      &&this->GetCellFieldArrayStatus(vtkMedUtilities::SimplifyName(
          field->GetName()).c_str());
}

int vtkMedReader::IsQuadratureFieldSelected(vtkMedField* field)
{
  return field->GetType()==vtkMedField::QuadratureField
      &&this->GetQuadratureFieldArrayStatus(vtkMedUtilities::SimplifyName(
          field->GetName()).c_str());
}

// Description:
// Give the animation steps to the pipeline
void vtkMedReader::AdvertiseTime(vtkInformation* info)
{
  this->ChooseRealAnimationMode();
  if(this->Internal->RealAnimationMode==PhysicalTime)
    {
    // I advertise the union of all times available in all selected fields
    set<double> timesets;
    for(int fieldId=0; fieldId<this->GetMedFile()->GetNumberOfField(); fieldId++)
      {
      vtkMedField* field=this->GetMedFile()->GetField(fieldId);
      if(!this->IsFieldSelected(field))
        continue;
      vector<pair<double, vector<med_int> > >& cache=
          this->Internal->StepCache[field];
      for(int timeId=0; timeId<cache.size(); timeId++)
        {
        timesets.insert(cache[timeId].first);
        }
      }
    if(timesets.size()>0)
      {
      vector<double> times;
      set<double>::iterator it=timesets.begin();
      while(it!=timesets.end())
        {
        times.push_back(*it);
        it++;
        }
      sort(times.begin(), times.end());
      info->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &times[0],
          times.size());
      double timeRange[2];
      timeRange[0]=times[0];
      timeRange[1]=times[times.size()-1];
      info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), &timeRange[0],
          2);
      }
    else
      {
      info->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
      info->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
      }
    }
  else if(this->Internal->RealAnimationMode==Iteration)
    {
    // I advertise the union of all iterations available at the given Time for all selected fields.
    set<double> iterationsets;
    for(int fieldId=0; fieldId<this->GetMedFile()->GetNumberOfField(); fieldId++)
      {
      vtkMedField* field=this->GetMedFile()->GetField(fieldId);
      if(!this->IsFieldSelected(field))
        continue;
      vector<pair<double, vector<med_int> > >& cache=
          this->Internal->StepCache[field];
      int index=this->GetTimeIndex(field, this->GetTime());
      if(index<0||index>=cache.size())
        continue;
      vector<med_int>& iterations=cache[index].second;
      for(int itid=0; itid<iterations.size(); itid++)
        iterationsets.insert(iterations[itid]);
      }
    if(iterationsets.size()>0)
      {
      vector<double> iterations;
      set<double>::iterator it=iterationsets.begin();
      while(it!=iterationsets.end())
        {
        iterations.push_back(*it);
        it++;
        }
      sort(iterations.begin(), iterations.end());
      info->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &iterations[0],
          iterations.size());
      double timeRange[2];
      timeRange[0]=iterations[0];
      timeRange[1]=iterations[iterations.size()-1];
      info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), &timeRange[0],
          2);
      }
    else
      {
      info->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
      info->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
      }
    }
  else
    {
    double range[2]={-vtkMath::RadiansFromDegrees(180.0),
        vtkMath::RadiansFromDegrees(180.0)};
    info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), range, 2);
    info->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    }
}

int vtkMedReader::GetTimeIndex(vtkMedField* field, double time)
{
  if(this->Internal->StepCache.find(field)==this->Internal->StepCache.end())
    return 0;

  vector<pair<double, vector<med_int> > >& cache=
      this->Internal->StepCache[field];
  int timeid;
  for(timeid=0; timeid<cache.size(); timeid++)
    {
    if(cache[timeid].first>=time)
      break;
    }

  if(timeid==cache.size())
    {
    return timeid-1;
    }

  if(timeid==0)
    {
    return 0;
    }

  double foundtime=cache[timeid].first;
  if(foundtime==time)
    {
    return timeid;
    }

  // test if the asked time if very near an existing time, else return the inferior step
  double deltatime=foundtime-cache[timeid-1].first;
  if(deltatime<=0)
    {
    return timeid;
    }

  if((foundtime-time)/deltatime<this->TimePrecision)
    {
    return timeid;
    }
  else
    {
    return timeid-1;
    }
}

med_int vtkMedReader::GetLastIterationIndex(vtkMedField* field, int timeid)
{
  if(this->Internal->StepCache.find(field)==this->Internal->StepCache.end())
    return 0;

  return this->Internal->StepCache[field][timeid].second.size()-1;
}

int vtkMedReader::GetIterationIndex(vtkMedField* field, int timeid,
    double iteration)
{
  if(this->Internal->StepCache.find(field)==this->Internal->StepCache.end())
    return 0;

  vector<med_int>& iterations=this->Internal->StepCache[field][timeid].second;
  int itid;
  for(itid=0; itid<iterations.size(); itid++)
    {
    if(iterations[itid]>=iteration)
      break;
    }
  if(itid==0)
    return 0;

  if(itid==iterations.size())
    return itid-1;

  if(iterations[itid]-iteration<this->TimePrecision)
    return itid;

  return itid-1;
}

int vtkMedReader::RequestDataObject(vtkInformation* request,
    vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation *info=outputVector->GetInformationObject(0);
  if(vtkMultiBlockDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT())))
    {
    // The output is already created
    return 1;
    }
  else
    {
    vtkMultiBlockDataSet* output=vtkMultiBlockDataSet::New();
    this->GetExecutive()->SetOutputData(0, output);
    output->Delete();
    this->GetOutputPortInformation(0)->Set(vtkDataObject::DATA_EXTENT_TYPE(),
        output->GetExtentType());
    }
  return 1;
}

void vtkMedReader::ClearSelections()
{
  for(int index=0; index<this->MedFile->GetNumberOfField(); index++)
    {
    vtkMedField* field=this->MedFile->GetField(index);
    if(field->GetType()==vtkMedField::PointField)
      {
      this->PointFields->AddArray(vtkMedUtilities::SimplifyName(
          field->GetName()->GetString()).c_str());
      }
    }

  for(int index=0; index<this->MedFile->GetNumberOfField(); index++)
    {
    vtkMedField* field=this->MedFile->GetField(index);
    if(field->GetType()==vtkMedField::CellField)
      {
      this->CellFields->AddArray(vtkMedUtilities::SimplifyName(
          field->GetName()->GetString()).c_str());
      }
    }

  for(int index=0; index<this->MedFile->GetNumberOfField(); index++)
    {
    vtkMedField* field=this->MedFile->GetField(index);
    if(field->GetType()==vtkMedField::QuadratureField)
      {
      this->QuadratureFields->AddArray(vtkMedUtilities::SimplifyName(
          field->GetName()->GetString()).c_str());
      }
    }

  for(int meshIndex=0; meshIndex<this->MedFile->GetNumberOfMesh(); meshIndex++)
    {
    vtkMedMesh* mesh=this->MedFile->GetMesh(meshIndex);
    for(int famIndex=0; famIndex<mesh->GetNumberOfFamily(); famIndex++)
      {
      vtkMedFamily* fam=mesh->GetFamily(famIndex);
      string name=vtkMedUtilities::FamilyKey(mesh->GetName()->GetString(),
          fam->GetPointOrCell(), fam->GetName()->GetString());
      int state=-1;
      if(!this->Internal->Families->ArrayExists(name.c_str()))
        {
        state=(mesh->GetFamily(famIndex)->GetPointOrCell()
            ==vtkMedUtilities::OnPoint? 0: 1);
        }
      this->Internal->Families->AddArray(name.c_str());
      if(state==0)
        this->Internal->Families->DisableArray(name.c_str());
      int ng=fam->GetNumberOfGroup();
      for(int gindex=0; gindex<ng; gindex++)
        {
        vtkMedGroup* group=fam->GetGroup(gindex);
        string gname=vtkMedUtilities::GroupKey(mesh->GetName()->GetString(),
            fam->GetPointOrCell(), group->GetName()->GetString());
        this->Groups->AddArray(gname.c_str());
        if(state==0)
          {
          this->Groups->DisableArray(gname.c_str());
          }
        }
      }
    }
  this->Internal->GroupSelectionMTime.Modified();

  for(int meshIndex=0; meshIndex<this->MedFile->GetNumberOfMesh(); meshIndex++)
    {
    vtkMedUnstructuredGrid* grid=vtkMedUnstructuredGrid::SafeDownCast(
        this->MedFile->GetMesh(meshIndex)->GetGrid());
    if(grid)
      {
      for(int entityIndex=0; entityIndex<grid->GetNumberOfEntityArray(); entityIndex++)
        {
        vtkMedEntityArray* array=grid->GetEntityArray(entityIndex);
        string name=vtkMedUtilities::CellTypeKey(array->GetType(),
            array->GetGeometry());
        this->Entities->AddArray(name.c_str());
        }
      }
    }
  this->Modified();
}

void vtkMedReader::SelectFamiliesFromGroups()
{
  this->Internal->Families->DisableAllArrays();

  for(int meshIndex=0; meshIndex<this->MedFile->GetNumberOfMesh(); meshIndex++)
    {
    vtkMedMesh* mesh=this->MedFile->GetMesh(meshIndex);
    for(int famIndex=0; famIndex<mesh->GetNumberOfFamily(); famIndex++)
      {
      vtkMedFamily* fam=mesh->GetFamily(famIndex);
      string name=vtkMedUtilities::FamilyKey(mesh->GetName()->GetString(),
          fam->GetPointOrCell(), fam->GetName()->GetString());

      int ng=fam->GetNumberOfGroup();
      if(ng==0)
        {
        string gname=vtkMedUtilities::GroupKey(mesh->GetName()->GetString(),
            fam->GetPointOrCell(), vtkMedUtilities::NoGroupName);
        int state=this->Groups->GetArraySetting(gname.c_str());
        if(state)
          {
          this->SetFamilyStatus(name.c_str(), 1);
          }
        }
      for(int gindex=0; gindex<ng; gindex++)
        {
        vtkMedGroup* group=fam->GetGroup(gindex);
        string gname=vtkMedUtilities::GroupKey(mesh->GetName()->GetString(),
            fam->GetPointOrCell(), group->GetName()->GetString());
        int state=this->Groups->GetArraySetting(gname.c_str());
        if(state)
          {
          this->SetFamilyStatus(name.c_str(), 1);
          }
        }
      }
    }
  this->Internal->FamilySelectionMTime.Modified();
}

vtkMedDriver*
vtkMedReader::NewMedDriver(const char* fname)
{
  int major, minor, release;
  vtkMedDriver* driver=vtkMedDriver::New();
  driver->SetFileName(fname);
  bool canRead=driver->CanReadFile();
  if(!canRead)
    {
    driver->Delete();
    return NULL;
    }
  driver->ReadFileVersion(&major, &minor, &release);
  driver->Delete();
  vtkMedFactory* factory=vtkMedFactory::New();
  driver=factory->NewMedDriver(major, minor, release);
  factory->Delete();
  if(driver)
    driver->SetFileName(fname);
  return driver;
}

int vtkMedReader::GetNumberOfPointFieldArrays()
{
  return this->PointFields->GetNumberOfArrays();
}

const char*
vtkMedReader::GetPointFieldArrayName(int index)
{
  return this->PointFields->GetArrayName(index);
}

int vtkMedReader::GetPointFieldArrayStatus(const char* name)
{
  return this->PointFields->GetArraySetting(name);
}

void vtkMedReader::SetPointFieldArrayStatus(const char* name, int status)
{
  if(this->PointFields->ArrayExists(name)&&this->PointFields->GetArraySetting(
      name)==status)
    return;

  if(status)
    {
    this->PointFields->EnableArray(name);
    }
  else
    {
    this->PointFields->DisableArray(name);
    }
  this->Modified();
}

int vtkMedReader::GetNumberOfCellFieldArrays()
{
  return this->CellFields->GetNumberOfArrays();
}

const char*
vtkMedReader::GetCellFieldArrayName(int index)
{
  return this->CellFields->GetArrayName(index);
}

int vtkMedReader::GetCellFieldArrayStatus(const char* name)
{
  return this->CellFields->GetArraySetting(name);
}

void vtkMedReader::SetCellFieldArrayStatus(const char* name, int status)
{
  if(this->CellFields->ArrayExists(name)&&this->CellFields->GetArraySetting(
      name)==status)
    return;

  if(status)
    {
    this->CellFields->EnableArray(name);
    }
  else
    {
    this->CellFields->DisableArray(name);
    }
  this->Modified();
}

int vtkMedReader::GetNumberOfQuadratureFieldArrays()
{
  return this->QuadratureFields->GetNumberOfArrays();
}

const char*
vtkMedReader::GetQuadratureFieldArrayName(int index)
{
  return this->QuadratureFields->GetArrayName(index);
}

int vtkMedReader::GetQuadratureFieldArrayStatus(const char* name)
{
  return this->QuadratureFields->GetArraySetting(name);
}

void vtkMedReader::SetQuadratureFieldArrayStatus(const char* name, int status)
{
  if(this->QuadratureFields->ArrayExists(name)
      &&this->QuadratureFields->GetArraySetting(name)==status)
    return;
  if(status)
    {
    this->QuadratureFields->EnableArray(name);
    }
  else
    {
    this->QuadratureFields->DisableArray(name);
    }
  this->Modified();
}

void vtkMedReader::SetCellTypeStatus(const char* name, int status)
{
  if(this->Entities->ArrayExists(name)&&this->Entities->GetArraySetting(name)
      ==status)
    return;
  if(status)
    {
    this->Entities->EnableArray(name);
    }
  else
    {
    this->Entities->DisableArray(name);
    }
  this->Modified();
}

void vtkMedReader::SetFamilyStatus(const char* name, int status)
{
  if(this->Internal->Families->ArrayExists(name)
      &&this->Internal->Families->GetArraySetting(name)==status)
    return;
  if(status)
    {
    this->Internal->Families->EnableArray(name);
    }
  else
    {
    this->Internal->Families->DisableArray(name);
    }
}

void vtkMedReader::SetGroupStatus(const char* name, int status)
{
  if(this->Groups->ArrayExists(name)&&this->Groups->GetArraySetting(name)
      ==status)
    return;
  if(status)
    {
    this->Groups->EnableArray(name);
    }
  else
    {
    this->Groups->DisableArray(name);
    }
  this->Modified();
  this->Internal->GroupSelectionMTime.Modified();
}

void vtkMedReader::AddQuadratureSchemeDefinition(vtkInformation* info,
    vtkMedQuadratureDefinition* meddef)
{
  if(info==NULL||meddef==NULL)
    return;

  vtkInformationQuadratureSchemeDefinitionVectorKey *key=
      vtkQuadratureSchemeDefinition::DICTIONARY();

  vtkQuadratureSchemeDefinition* def=vtkQuadratureSchemeDefinition::New();
  int cellType=vtkMedUtilities::GetVTKCellType(meddef->GetGeometry());
  def->Initialize(cellType, vtkMedUtilities::GetNumberOfPoint(
      meddef->GetGeometry()), meddef->GetNumberOfQuadraturePoint(),
      meddef->GetShapeFunction(), meddef->GetWeights());
  key->Set(info, def, cellType);
  def->Delete();

}

void vtkMedReader::FlagUsedPointsOnCellSupport(vtkMedFamilyOnEntity* foe,
    vtkUnsignedCharArray* flag)
{
  vtkMedMesh* mesh=foe->GetMesh();
  flag->SetNumberOfComponents(1);
  flag->SetNumberOfTuples(mesh->GetGrid()->GetNumberOfPoints());

  for(vtkIdType index=0; index<mesh->GetGrid()->GetNumberOfPoints(); index++)
    {
    flag->SetValue(index, 0);
    }

  vtkMedFamily* family=foe->GetFamily();
  vtkMedEntityArray* array=foe->GetEntityArray();

  this->MedDriver->LoadFamilyIds(mesh, array);

  this->LoadConnectivity(mesh, array);

  vtkMedIntArray* conn=array->GetConnectivityArray();

  vtkIdList* pts=vtkIdList::New();
  for(vtkIdType index=0; index<array->GetNumberOfEntity(); index++)
    {
    if(!this->KeepCell(family, array, index))
      continue;

    this->GetCellPoints(mesh, array, index, pts);
    for(int pt=0; pt<pts->GetNumberOfIds(); pt++)
      {
      vtkIdType dest=pts->GetId(pt);
      flag->SetValue(dest, 1);
      }
    }
  pts->Delete();
}

void vtkMedReader::LoadConnectivity(vtkMedMesh* mesh, vtkMedEntityArray* array)
{
  this->MedDriver->LoadConnectivity(mesh, array);
  if(array->GetConnectivity()==MED_NOD||vtkMedUtilities::GetDimension(
      array->GetGeometry())<2)
    return;

  med_entite_maillage subtype=vtkMedUtilities::GetSubType(array->GetType());
  vtkMedUnstructuredGrid* grid=vtkMedUnstructuredGrid::SafeDownCast(
      mesh->GetGrid());
  if(grid==NULL)
    {
    vtkErrorMacro("DESC connectivity is only supported for unstructured grids.");
    return;
    }
  for(int index=0; index<vtkMedUtilities::GetNumberOfSubEntity(
      array->GetGeometry()); index++)
    {
    med_geometrie_element geom=vtkMedUtilities::GetSubGeometry(
        array->GetGeometry(), index);
    vtkMedEntityArray* subarray=grid->GetEntityArray(subtype, geom);
    if(subarray==NULL)
      {
      vtkErrorMacro("DESC connectivity used, but sub types do not exist in file.");
      return;
      }
    this->LoadConnectivity(mesh, subarray);
    }
}

void vtkMedReader::BuildUnstructuredGridForCellSupport(
    vtkMedFamilyOnEntity* foe, vtkUnstructuredGrid* grid)
{
  vtkMedMesh* mesh=foe->GetMesh();
  vtkMedUnstructuredGrid* medgrid=vtkMedUnstructuredGrid::SafeDownCast(
      mesh->GetGrid());

  if(!medgrid)
    {
    //TODO : structured grid support
    vtkWarningMacro ("Structured grids not supported")
    return;
    }

  // first, mark all the nodes that must be copied in the new vtkUnstructuredGrid
  vtkUnsignedCharArray* nodeFlag=vtkUnsignedCharArray::New();
  this->FlagUsedPointsOnCellSupport(foe, nodeFlag);

  // now we copy all the flagged nodes in the grid, shallow copy if possible
  bool shallowCopyPoints=true;
  bool restrictIndices=false;

  if(medgrid->GetDimension()!=3)
    {
    shallowCopyPoints=false;
    }

  for(vtkIdType index=0; index<nodeFlag->GetNumberOfTuples(); index++)
    {
    if(nodeFlag->GetValue(index)==0)
      {
      restrictIndices=true;
      shallowCopyPoints=false;
      break;
      }
    }

  this->MedDriver->LoadCoordinates(mesh);

  vtkIdType numberOfPoints;
  vtkPoints* points=vtkPoints::New(medgrid->GetCoordinates()->GetDataType());
  grid->SetPoints(points);
  points->Delete();

  vtkIdTypeArray* pointGlobalIds=vtkIdTypeArray::New();
  pointGlobalIds->SetName("MED_POINT_ID");
  pointGlobalIds->SetNumberOfComponents(1);
  grid->GetPointData()->SetGlobalIds(pointGlobalIds);
  pointGlobalIds->Delete();

  vtkIdTypeArray* cellGlobalIds=vtkIdTypeArray::New();
  cellGlobalIds->SetName("MED_CELL_ID");
  cellGlobalIds->SetNumberOfComponents(1);
  grid->GetCellData()->SetGlobalIds(cellGlobalIds);
  cellGlobalIds->Delete();

  if(shallowCopyPoints)
    {
    grid->GetPoints()->SetDataType(medgrid->GetCoordinates()->GetDataType());
    grid->GetPoints()->SetData(medgrid->GetCoordinates());
    // add global ids
    numberOfPoints=grid->GetNumberOfPoints();
    pointGlobalIds->SetNumberOfTuples(numberOfPoints);
    vtkIdType* ptr=pointGlobalIds->GetPointer(0);
    for(int pid=0; pid<numberOfPoints; pid++)
      ptr[pid]=pid+1;
    }
  else
    {
    if(restrictIndices)
      {
      this->Internal->Med2VTKPointIndex[foe]
          =vtkstd::map<vtkIdType, vtkIdType>();
      }
    vtkstd::map<vtkIdType, vtkIdType> tmp;
    vtkstd::map<vtkIdType, vtkIdType>& med2VTKIndex=
        (restrictIndices? this->Internal->Med2VTKPointIndex[foe]: tmp);
    vtkIdType currentIndex=0;
    for(vtkIdType index=0; index<medgrid->GetNumberOfPoints(); index++)
      {
      if(nodeFlag->GetValue(index)==0)
        {
        continue;
        }
      if(restrictIndices)
        med2VTKIndex[index]=currentIndex;

      double coord[3]={0.0, 0.0, 0.0};
      double * tuple=medgrid->GetCoordinates()->GetTuple(index);
      for(int dim=0; dim<medgrid->GetDimension()&&dim<3; dim++)
        {
        coord[dim]=tuple[dim];
        }
      grid->GetPoints()->InsertPoint(currentIndex, coord);
      pointGlobalIds->InsertNextValue(index+1);
      currentIndex++;
      }
    grid->GetPoints()->Squeeze();
    pointGlobalIds->Squeeze();
    numberOfPoints=currentIndex;
    }
  nodeFlag->Delete();

  vtkUnsignedCharArray *cellTypes=vtkUnsignedCharArray::New();
  vtkIdTypeArray *cellLocations=vtkIdTypeArray::New();
  vtkCellArray *cells=vtkCellArray::New();
  grid->SetCells(cellTypes, cellLocations, cells);
  cellTypes->Delete();
  cellLocations->Delete();
  cells->Delete();
  vtkIdType location=0;

  vtkMedFamily* family=foe->GetFamily();
  vtkMedEntityArray* array=foe->GetEntityArray();
  vtkMedIntArray* conn=array->GetConnectivityArray();

  unsigned char vtkType = vtkMedUtilities::GetVTKCellType(array->GetGeometry());
  vtkIdType ncell=0;
  vtkIdList* pts=vtkIdList::New();
  vtkIdType intialGlobalId=array->GetInitialGlobalId();
  for(vtkIdType index=0; index<array->GetNumberOfEntity(); index++)
    {
    if(!this->KeepCell(family, array, index))
      continue;

    this->GetCellPoints(mesh, array, index, pts);

    cellGlobalIds->InsertNextValue(intialGlobalId+index);
    cellTypes->InsertNextValue(vtkType);
    cellLocations->InsertNextValue(location);
    cells->InsertNextCell(pts->GetNumberOfIds());
    vtkstd::map<vtkIdType, vtkIdType> tmp;
    vtkstd::map<vtkIdType, vtkIdType>& med2VTKIndex=
        (restrictIndices? this->Internal->Med2VTKPointIndex[foe]: tmp);
    for(vtkIdType node=0; node<pts->GetNumberOfIds(); node++)
      {
      vtkIdType ptid=pts->GetId(node);
      if(restrictIndices)
        {
        ptid=med2VTKIndex[ptid];
        }
      cells->InsertCellPoint(ptid);
      }
    location+=pts->GetNumberOfIds()+1;
    ncell++;
    }
  pts->Delete();

  cellTypes->Squeeze();
  cellLocations->Squeeze();
  cells->Squeeze();
}

void vtkMedReader::BuildUnstructuredGridForPointSupport(
    vtkMedFamilyOnEntity* foe, vtkUnstructuredGrid* grid)
{
  vtkMedMesh* mesh=foe->GetMesh();
  vtkMedUnstructuredGrid* medgrid=vtkMedUnstructuredGrid::SafeDownCast(
      mesh->GetGrid());
  if(!medgrid)
    {
    vtkWarningMacro("Structured data sets are not supported by this reader");
    return;
    }

  this->MedDriver->LoadCoordinates(mesh);

  vtkIdType npts=mesh->GetGrid()->GetNumberOfPoints();

  bool shallowCopy=true;
  if(foe->GetAllPoints()==-1)
    {
    if(medgrid->GetDimension()!=3)
      {
      shallowCopy=false;
      }
    else
      {
      for(int index=0; index<npts; index++)
        {
        if(this->KeepPoint(foe, index)==0)
          {
          shallowCopy=false;
          break;
          }
        }
      }
    foe->SetAllPoints(shallowCopy);
    }
  else
    {
    shallowCopy=foe->GetAllPoints();
    }

  vtkIdType numberOfPoints;
  vtkPoints* points=vtkPoints::New(medgrid->GetCoordinates()->GetDataType());
  grid->SetPoints(points);
  points->Delete();

  vtkIdTypeArray* pointGlobalIds=vtkIdTypeArray::New();
  pointGlobalIds->SetName("MED_POINT_ID");
  pointGlobalIds->SetNumberOfComponents(1);
  grid->GetPointData()->SetGlobalIds(pointGlobalIds);
  pointGlobalIds->Delete();

  if(shallowCopy)
    {
    grid->GetPoints()->SetDataType(medgrid->GetCoordinates()->GetDataType());
    grid->GetPoints()->SetData(medgrid->GetCoordinates());
    numberOfPoints=npts;

    pointGlobalIds->SetNumberOfTuples(numberOfPoints);
    vtkIdType* ptr=pointGlobalIds->GetPointer(0);
    for(int pid=0; pid<numberOfPoints; pid++)
      ptr[pid]=pid+1;
    }
  else
    {
    vtkIdType currentIndex=0;
    this->Internal->Med2VTKPointIndex[foe]=vtkstd::map<vtkIdType, vtkIdType>();
    vtkstd::map<vtkIdType, vtkIdType>& med2VTKIndex=
        this->Internal->Med2VTKPointIndex[foe];
    vtkMedFamily* family=foe->GetFamily();
    for(vtkIdType index=0; index<medgrid->GetNumberOfPoints(); index++)
      {
      if(!this->KeepPoint(family, mesh, index))
        {
        continue;
        }
      med2VTKIndex[index]=currentIndex;

      double coord[3]={0.0, 0.0, 0.0};
      double * tuple=medgrid->GetCoordinates()->GetTuple(index);
      for(int dim=0; dim<medgrid->GetDimension()&&dim<3; dim++)
        {
        coord[dim]=tuple[dim];
        }
      grid->GetPoints()->InsertPoint(currentIndex, coord);
      pointGlobalIds->InsertNextValue(index+1);
      currentIndex++;
      }
    grid->GetPoints()->Squeeze();
    pointGlobalIds->Squeeze();
    numberOfPoints=currentIndex;
    }

  // now create the VTK_VERTEX cells
  for(vtkIdType id=0; id<numberOfPoints; id++)
    {
    grid->InsertNextCell(VTK_VERTEX, 1, &id);
    }
  grid->Squeeze();

  // in this particular case, the global ids of the cells is the same as the global ids of the points.
  grid->GetCellData()->SetGlobalIds(grid->GetPointData()->GetGlobalIds());
}

bool vtkMedReader::KeepCell(vtkMedFamily* family, vtkMedEntityArray* array,
    vtkIdType index)
{
  med_int famid=(array->GetFamilyIds()!=NULL? array->GetFamilyIds()->GetValue(
      index): 0);
  return famid==family->GetId();
}

bool vtkMedReader::KeepPoint(vtkMedFamily* family, vtkMedMesh* mesh,
    vtkIdType index)
{
  med_int famid=
      (mesh->GetPointFamilyIds()!=NULL? mesh->GetPointFamilyIds()->GetValue(
          index): 0);
  return famid==family->GetId();
}

bool vtkMedReader::KeepPoint(vtkMedFamilyOnEntity* foe, vtkIdType index)
{
  if(foe->GetPointOrCell()!=vtkMedUtilities::OnPoint)
    {
    vtkWarningMacro("use this method only for node support");
    return false;
    }

  vtkMedMesh* mesh=foe->GetMesh();

  med_int famid=
      (mesh->GetPointFamilyIds()!=NULL? mesh->GetPointFamilyIds()->GetValue(
          index): 0);
  if(famid==foe->GetFamily()->GetId())
    return true;

  return false;
}

vtkMultiBlockDataSet*
vtkMedReader::GetParent(const vtkList<vtkstd::string>& path)
{
  vtkMultiBlockDataSet* output=this->GetOutput();
  vtkMultiBlockDataSet* parent=output;
  vtkList<vtkstd::string>::const_iterator it=path.begin();
  while(it!=path.end())
    {
    vtkstd::string parentName=*it;
    it++;
    bool found=false;
    for(int blockId=0; blockId<parent->GetNumberOfBlocks(); blockId++)
      {
      vtkInformation* metaData=parent->GetMetaData(blockId);
      const char* blockName=NULL;
      if(metaData->Has(vtkCompositeDataSet::NAME()))
        {
        const char* blockName=metaData->Get(vtkCompositeDataSet::NAME());
        if(parentName==blockName&&vtkMultiBlockDataSet::SafeDownCast(
            parent->GetBlock(blockId))!=NULL)
          {
          parent=vtkMultiBlockDataSet::SafeDownCast(parent->GetBlock(blockId));
          found=true;
          break;
          }
        }
      }
    if(!found)
      {
      // If I am here, it means that I did not find any block with the good name, create one
      int nb=parent->GetNumberOfBlocks();
      vtkMultiBlockDataSet* block=vtkMultiBlockDataSet::New();
      parent->SetBlock(nb, block);
      block->Delete();
      parent->GetMetaData(nb)->Set(vtkCompositeDataSet::NAME(),
          parentName.c_str());
      parent=block;
      }
    }
  return parent;
}

void vtkMedReader::CreateMedSupports()
{
  this->Internal->UsedSupports.clear();
  for(int meshIndex=0; meshIndex<this->MedFile->GetNumberOfMesh(); meshIndex++)
    {
    vtkMedMesh* mesh=this->MedFile->GetMesh(meshIndex);

    // create the node centered supports
    this->MedDriver->LoadPointFamilyIds(mesh);

    for(int famIndex=0; famIndex<mesh->GetNumberOfPointFamilyOnEntity(); famIndex++)
      {
      vtkMedFamilyOnEntity* foe=mesh->GetPointFamilyOnEntity(famIndex);
      vtkMedFamily* family=foe->GetFamily();
      if(this->GetFamilyStatus(mesh, family)==0)
        continue;
      this->Internal->UsedSupports.insert(foe);
      }

    // create the cell-centered supports
    vtkMedUnstructuredGrid* ugrid=vtkMedUnstructuredGrid::SafeDownCast(
        mesh->GetGrid());

    if(ugrid)
      {
      for(int entityIndex=0; entityIndex<ugrid->GetNumberOfEntityArray(); entityIndex++)
        {
        vtkMedEntityArray* array=ugrid->GetEntityArray(entityIndex);
        if(this->GetCellTypeStatus(array->GetType(), array->GetGeometry())==0)
          {
          continue;
          }
        this->MedDriver->LoadFamilyIds(mesh, array);

        for(int foeIndex=0; foeIndex<array->GetNumberOfFamilyOnEntity(); foeIndex++)
          {
          vtkMedFamilyOnEntity* foe=array->GetFamilyOnEntity(foeIndex);
          vtkMedFamily* family=foe->GetFamily();
          if(this->GetFamilyStatus(mesh, family)==0)
            continue;

          this->Internal->UsedSupports.insert(foe);
          }
        }
      }
    else
      {
      vtkWarningMacro("Structured grids are not supported by this reader.");
      //TODO : deal with structured grids
      }
    }
}

void vtkMedReader::BuildVTKSupport(vtkMedFamilyOnEntity* foe)
{
  vtkMedMesh* mesh=foe->GetMesh();
  vtkMedGrid* grid=mesh->GetGrid();
  vtkMedUnstructuredGrid* ugrid=vtkMedUnstructuredGrid::SafeDownCast(grid);
  if(ugrid==NULL)
    {
    //TODO
    vtkWarningMacro("Structured grids are not supported by this reader.");
    return;
    }
  vtkList<std::string> path;
  string meshName=vtkMedUtilities::SimplifyName(mesh->GetName());
  path.push_back(meshName);
  std::string finalName;

  if(foe->GetPointOrCell()==vtkMedUtilities::OnPoint)
    {
    path.push_back(vtkMedUtilities::OnPointName);
    finalName=vtkMedUtilities::SimplifyName(foe->GetFamily()->GetName());
    }
  else
    {
    path.push_back(vtkMedUtilities::OnCellName);
    path.push_back(vtkMedUtilities::SimplifyName(foe->GetFamily()->GetName()));
    vtkMedEntityArray* array=foe->GetEntityArray();
    finalName=vtkMedUtilities::CellTypeKey(array->GetType(),
        array->GetGeometry());
    }

  vtkMultiBlockDataSet* root=this->GetParent(path);
  vtkDataSet* cachedDataSet;
  if(this->Internal->DataSetCache.find(foe)!=this->Internal->DataSetCache.end())
    {
    cachedDataSet=this->Internal->DataSetCache[foe];
    }
  else
    {
    ostringstream progressBarTxt;
    vtkList<std::string>::iterator pathit=path.begin();
    while(pathit!=path.end())
      {
      progressBarTxt<<*pathit<<" ";
      pathit++;
      }
    progressBarTxt<<finalName;
    SetProgressText(progressBarTxt.str().c_str());
    // load the node coordinates
    vtkUnstructuredGrid* grid=vtkUnstructuredGrid::New();
    this->Internal->DataSetCache[foe]=grid;
    grid->Delete();
    cachedDataSet=grid;

    if(foe->GetPointOrCell()==vtkMedUtilities::OnPoint)
      {
      this->BuildUnstructuredGridForPointSupport(foe, grid);
      }
    else
      {
      this->BuildUnstructuredGridForCellSupport(foe, grid);
      }
    }// else on cache

  int nb=root->GetNumberOfBlocks();

  vtkDataSet* realDataSet=cachedDataSet->NewInstance();
  root->SetBlock(nb, realDataSet);
  realDataSet->Delete();

  root->GetMetaData(nb)->Set(vtkCompositeDataSet::NAME(), finalName.c_str());
  realDataSet->ShallowCopy(cachedDataSet);

  this->Internal->DataSetCache[foe]=cachedDataSet;
  this->Internal->CurrentDataSet[foe]=realDataSet;
}

void vtkMedReader::MapFieldsOnSupport(vtkMedFamilyOnEntity* foe)
{
  // now loop over all fields to map it to the created grids
  for(int fieldId=0; fieldId<this->GetMedFile()->GetNumberOfField(); fieldId++)
    {
    vtkMedField* field=this->GetMedFile()->GetField(fieldId);

    if(this->IsFieldSelected(field))
      {
      this->SelectStep(field);
      bool cached=false;
      ComputeStep cs=this->Internal->CurrentStep[field];
      TimedField timedField;
      timedField.first=field;
      timedField.second=cs;
      if(this->Internal->FieldCache.find(foe)!=this->Internal->FieldCache.end())
        {
        map<TimedField, VTKField>& fieldCache=this->Internal->FieldCache[foe];
        if(fieldCache.find(timedField)!=fieldCache.end())
          {
          cached=true;
          }
        }

      if(!cached&&!this->IsFieldOnSupport(field, foe))
        continue;

      if(!cached)
        {
        this->CreateVTKFieldOnSupport(field, foe);
        }
      this->SetVTKFieldOnSupport(field, foe);
      }
    }
}

void vtkMedReader::SetVTKFieldOnSupport(vtkMedField* field,
    vtkMedFamilyOnEntity* foe)
{
  ComputeStep cs=this->Internal->CurrentStep[field];
  TimedField timedField;
  timedField.first=field;
  timedField.second=cs;
  vtkDataSet* ds=this->Internal->CurrentDataSet[foe];
  map<TimedField, VTKField>& timedFieldCache=this->Internal->FieldCache[foe];
  VTKField& vtkfield=timedFieldCache[timedField];
  vtkfield.DataArray->SetName(
      vtkMedUtilities::SimplifyName(field->GetName()).c_str());
  vtkfield.DataArray->Squeeze();
  if(vtkfield.QuadratureIndexArray!=NULL)
    vtkfield.QuadratureIndexArray->Squeeze();

  if(foe->GetPointOrCell()==vtkMedUtilities::OnCell)
    {
    if(field->GetType()==vtkMedField::PointField)
      {
      if(vtkfield.DataArray->GetNumberOfTuples()!=ds->GetNumberOfPoints())
        {
        vtkDebugMacro("the data array " << vtkfield.DataArray->GetName() << " do not have the good number of tuples");
        return;
        }
      ds->GetPointData()->AddArray(vtkfield.DataArray);
      switch(vtkfield.DataArray->GetNumberOfComponents())
      {
        case 1:
          ds->GetPointData()->SetActiveScalars(vtkfield.DataArray->GetName());
          break;
        case 3:
          ds->GetPointData()->SetActiveVectors(vtkfield.DataArray->GetName());
          break;
      }
      // if the data set is only composed of VTK_VERTEX cells,
      // and no field called with the same name exist on cells,
      // map this field to cells too
      if(foe->GetVertexOnly()==1&&ds->GetCellData()->GetArray(
          vtkfield.DataArray->GetName())==NULL)
        {
        ds->GetCellData()->AddArray(vtkfield.DataArray);
        switch(vtkfield.DataArray->GetNumberOfComponents())
        {
          case 1:
            ds->GetCellData()->SetActiveScalars(vtkfield.DataArray->GetName());
            break;
          case 3:
            ds->GetCellData()->SetActiveVectors(vtkfield.DataArray->GetName());
            break;
        }
        }
      }
    if(field->GetType()==vtkMedField::CellField)
      {
      if(vtkfield.DataArray->GetNumberOfTuples()!=ds->GetNumberOfCells())
        {
        vtkDebugMacro("the data array " << vtkfield.DataArray->GetName() << " do not have the good number of tuples");
        return;
        }
      ds->GetCellData()->AddArray(vtkfield.DataArray);
      switch(vtkfield.DataArray->GetNumberOfComponents())
      {
        case 1:
          ds->GetCellData()->SetActiveScalars(vtkfield.DataArray->GetName());
          break;
        case 3:
          ds->GetCellData()->SetActiveVectors(vtkfield.DataArray->GetName());
          break;
      }
      // if the data set is only composed of VTK_VERTEX cells,
      // and no field called with the same name exist on points,
      // map this field to points too
      if(foe->GetVertexOnly()==1&&ds->GetPointData()->GetArray(
          vtkfield.DataArray->GetName())==NULL)
        {
        ds->GetPointData()->AddArray(vtkfield.DataArray);
        switch(vtkfield.DataArray->GetNumberOfComponents())
        {
          case 1:
            ds->GetPointData()->SetActiveScalars(vtkfield.DataArray->GetName());
            break;
          case 3:
            ds->GetPointData()->SetActiveVectors(vtkfield.DataArray->GetName());
            break;
        }
        }
      }
    if(field->GetType()==vtkMedField::QuadratureField)
      {
      vtkIdType ncells=ds->GetNumberOfCells();
      vtkIdType nid=vtkfield.QuadratureIndexArray->GetNumberOfTuples();
      if(nid!=ncells)
        {
        vtkDebugMacro(
            "There should be as many quadrature index values as there are cells");
        return;
        }
      else
        {
        ds->GetFieldData()->AddArray(vtkfield.DataArray);
        ds->GetCellData()->AddArray(vtkfield.QuadratureIndexArray);
        vtkfield.DataArray->GetInformation()->Set(
            vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME(),
            vtkfield.QuadratureIndexArray->GetName());
        vtkfield.QuadratureIndexArray->GetInformation()->Set(
            vtkAbstractArray::GUI_HIDE(), 1);

        }
      }
    }//support OnCell
  else
    {//support OnPoint
    if(vtkfield.DataArray->GetNumberOfTuples()!=ds->GetNumberOfPoints())
      {
      vtkDebugMacro("the data array " << vtkfield.DataArray->GetName() << " do not have the good number of tuples");
      return;
      }
    ds->GetPointData()->AddArray(vtkfield.DataArray);
    switch(vtkfield.DataArray->GetNumberOfComponents())
    {
      case 1:
        ds->GetPointData()->SetActiveScalars(vtkfield.DataArray->GetName());
        break;
      case 3:
        ds->GetPointData()->SetActiveVectors(vtkfield.DataArray->GetName());
        break;
    }
    // all the VTK_VERTEX created cells have the same order than the points,
    // I can safely map the point array to the cells in this particular case.
    ds->GetCellData()->AddArray(vtkfield.DataArray);
    switch(vtkfield.DataArray->GetNumberOfComponents())
    {
      case 1:
        ds->GetCellData()->SetActiveScalars(vtkfield.DataArray->GetName());
        break;
      case 3:
        ds->GetCellData()->SetActiveVectors(vtkfield.DataArray->GetName());
        break;
    }
    }
}

int vtkMedReader::IsFieldOnSupport(vtkMedField* field,
    vtkMedFamilyOnEntity* foe)
{
  // cell fields cannot match point supports
  if((foe->GetPointOrCell()==vtkMedUtilities::OnPoint&&field->GetType()
      !=vtkMedField::PointField))
    return 0;

  vtkMedMesh* mesh=foe->GetMesh();

  ComputeStep cs=this->Internal->CurrentStep[field];
  TimedField timedField;
  timedField.first=field;
  timedField.second=cs;

  if(this->Internal->FieldMatchCache.find(foe)
      ==this->Internal->FieldMatchCache.end())
    this->Internal->FieldMatchCache[foe]=map<TimedField, bool> ();
  map<TimedField, bool>& fieldMatchCache=this->Internal->FieldMatchCache[foe];
  if(fieldMatchCache.find(timedField)!=fieldMatchCache.end())
    return fieldMatchCache[timedField];

  bool found=false;
  vtkMedFieldOverEntity* fieldOverEntity=NULL;
  vtkMedFieldStep* step=NULL;
  vtkMedFieldStepOnMesh* stepOnMesh=NULL;

  for(int entityId=0; entityId<field->GetNumberOfFieldOverEntity(); entityId++)
    {
    fieldOverEntity=field->GetFieldOverEntity(entityId);

    if(field->GetType()!=vtkMedField::PointField
        &&(foe->GetEntityArray()->GetGeometry()!=fieldOverEntity->GetGeometry()))
      continue;

    if(!this->GetCellTypeStatus(fieldOverEntity->GetType(),
        fieldOverEntity->GetGeometry()))
      continue;

    step=this->GetSelectedStep(field, fieldOverEntity, timedField.second.first,
        timedField.second.second);
    if(step==NULL)
      continue;

    stepOnMesh=step->GetStepOnMesh(mesh);
    if(stepOnMesh==NULL)
      continue;

    found=true;
    break;
    }// entityId

  if(found==false)
    {
    fieldMatchCache[timedField]=false;
    return 0;
    }

  TimedFieldOverEntity tfoe;
  tfoe.Entity=fieldOverEntity;
  tfoe.Step=step;
  tfoe.StepOnMesh=stepOnMesh;

  if(this->Internal->FieldOnFamilyOnEntityCache.find(timedField)
      ==this->Internal->FieldOnFamilyOnEntityCache.end())
    this->Internal->FieldOnFamilyOnEntityCache[timedField]=map<
        vtkMedFamilyOnEntity*, TimedFieldOverEntity> ();
  this->Internal->FieldOnFamilyOnEntityCache[timedField][foe]=tfoe;

  if(stepOnMesh->GetHasProfile()==1)
    {

    vtkMedProfile* profile=this->GetProfile(mesh, field, tfoe.Entity,
        tfoe.Step, tfoe.StepOnMesh);

    if(!this->IsFamilyOnEntityOnProfile(foe, profile, field->GetType(), false))
      {
      fieldMatchCache[timedField]=false;
      return 0;
      }
    }
  fieldMatchCache[timedField]=true;
  return 1;
}

int vtkMedReader::IsFamilyOnEntityOnProfile(vtkMedFamilyOnEntity* foe,
    vtkMedProfile* profile, int fieldSupportType, bool exact)
{
  int nodeOrCellSupport=foe->GetPointOrCell();
  if(fieldSupportType!=vtkMedField::PointField&&nodeOrCellSupport
      ==vtkMedUtilities::OnPoint)
    return false;

  if(fieldSupportType!=vtkMedField::PointField&&nodeOrCellSupport
      ==vtkMedUtilities::OnCell)
    return this->IsCellFamilyOnEntityOnCellProfile(foe, profile, exact);

  if(fieldSupportType==vtkMedField::PointField&&nodeOrCellSupport
      ==vtkMedUtilities::OnPoint)
    return this->IsPointFamilyOnEntityOnPointProfile(foe, profile, exact);

  if(fieldSupportType==vtkMedField::PointField&&nodeOrCellSupport
      ==vtkMedUtilities::OnCell)
    return this->IsCellFamilyOnEntityOnPointProfile(foe, profile, exact);
}

int vtkMedReader::IsPointFamilyOnEntityOnPointProfile(
    vtkMedFamilyOnEntity* foe, vtkMedProfile* profile, bool exact)
{
  pair<vtkMedFamilyOnEntity*, vtkMedProfile*> key=make_pair(foe, profile);

  if(this->Internal->ProfileMatch.find(key)!=this->Internal->ProfileMatch.end())
    {
    if(exact)
      return this->Internal->ProfileMatch[key].first;
    else
      return this->Internal->ProfileMatch[key].second;
    }

  this->MedDriver->Load(profile);
  set<med_int> fids;
  vtkMedIntArray* famIds=foe->GetMesh()->GetPointFamilyIds();
  vtkMedIntArray* pIds=profile->GetIds();
  med_int famId=(foe->GetFamily()? foe->GetFamily()->GetId(): 0);
  bool match=true;
  bool exactmatch=true;
  vtkIdType pindex=0;

  for(vtkIdType index=0; index<famIds->GetNumberOfTuples()&&match&&pindex
      <pIds->GetNumberOfTuples(); index++)
    {
    bool test=false;
    med_int fid=famIds->GetValue(index);
    med_int pid=pIds->GetValue(pindex)-1;
    if(fid==famId)
      {
      if(pid!=index)
        {
        match=false;
        exactmatch=false;
        }
      else
        // on the profile and found the family --> forward on the profile
        pindex++;
      }
    else
      {
      if(pid==index)
        {// we are on the profile and did not find the family --> no exact match, but the the profile might be larger than the family.
        exactmatch=false;
        pindex++;
        }
      else if(index>pid)
        {
        match=false;
        exactmatch=false;
        }
      }
    }

  this->Internal->ProfileMatch[key]=make_pair(exactmatch, match);
  if(exact)
    return exactmatch;
  return match;
}

int vtkMedReader::IsCellFamilyOnEntityOnPointProfile(vtkMedFamilyOnEntity* foe,
    vtkMedProfile* profile, bool exact)
{
  pair<vtkMedFamilyOnEntity*, vtkMedProfile*> key=make_pair(foe, profile);

  if(this->Internal->ProfileMatch.find(key)!=this->Internal->ProfileMatch.end())
    {
    if(exact)
      return this->Internal->ProfileMatch[key].first;
    else
      return this->Internal->ProfileMatch[key].second;
    }

  this->MedDriver->Load(profile);
  vtkUnsignedCharArray* flag;
  if(this->Internal->UsedPointsOnCellSupportCache.find(foe)
      ==this->Internal->UsedPointsOnCellSupportCache.end())
    {
    flag=vtkUnsignedCharArray::New();
    this->Internal->UsedPointsOnCellSupportCache[foe]=flag;
    this->FlagUsedPointsOnCellSupport(foe, flag);
    }

  flag=this->Internal->UsedPointsOnCellSupportCache[foe];

  bool match=true;
  bool exactmatch=true;
  if(profile==NULL)
    {
    for(vtkIdType index=0; index<flag->GetNumberOfTuples(); index++)
      {
      if(flag->GetValue(index)==0)
        {
        match=false;
        exactmatch=false;
        break;
        }
      }
    this->Internal->ProfileMatch[key]=make_pair(exactmatch, match);
    return match;
    }

  vtkMedIntArray* pids=profile->GetIds();
  vtkIdType pIndex=0;
  for(vtkIdType index=0; index<flag->GetNumberOfTuples()&&match&&pIndex
      <pids->GetNumberOfTuples(); index++)
    {
    med_int pid=pids->GetValue(pIndex)-1;
    if(index<pid) // not on the profile
      {
      if(flag->GetValue(index)==1)
        {
        match=false;
        exactmatch=false;
        }
      }
    else if(index==pid)
      {
      pIndex++;
      if(flag->GetValue(index)==0)
        {
        exactmatch=false;
        }
      }
    else
      {
      exactmatch=false;
      match=false;
      }
    }
  this->Internal->ProfileMatch[key]=make_pair(exactmatch, match);
  if(exact)
    return exactmatch;
  return match;
}

int vtkMedReader::IsCellFamilyOnEntityOnCellProfile(vtkMedFamilyOnEntity* foe,
    vtkMedProfile* profile, bool exact)
{
  pair<vtkMedFamilyOnEntity*, vtkMedProfile*> key=make_pair(foe, profile);

  if(this->Internal->ProfileMatch.find(key)!=this->Internal->ProfileMatch.end())
    {
    if(exact)
      return this->Internal->ProfileMatch[key].first;
    else
      return this->Internal->ProfileMatch[key].second;
    }

  this->MedDriver->Load(profile);
  vtkMedIntArray* pids=profile->GetIds();

  med_int famId=(foe->GetFamily()? foe->GetFamily()->GetId(): 0);

  bool match=true;
  bool exactmatch=true;
  if(pids!=NULL)
    {
    vtkIdType pindex=0;
    vtkMedEntityArray* array=foe->GetEntityArray();
    vtkMedIntArray* famIds=array->GetFamilyIds();
    for(int index=0; index<famIds->GetNumberOfTuples()&&match&&pindex
        <pids->GetNumberOfTuples(); index++)
      {
      med_int pid=pids->GetValue(pindex)-1;
      med_int fid=famIds->GetValue(index);
      if(famId==fid)
        {
        if(index==pid)
          pindex++;
        else
          {
          match=false;
          exactmatch=false;
          }
        }
      else
        {
        if(index==pid)
          {
          exactmatch=false;
          pindex++;
          }
        else if(index>pid) // this should not happen : we passed by the pid.
          {
          exactmatch=false;
          match=false;
          }
        }
      }
    }

  this->Internal->ProfileMatch[key]=make_pair(exactmatch, match);
  if(exact)
    return exactmatch;
  return match;
}

int vtkMedReader::CanShallowCopyMed2VTK(vtkMedField* field,
    vtkMedFamilyOnEntity* foe)
{
  if(field->GetNumberOfFieldOverEntity()!=1)
    return 0;

  vtkMedMesh* mesh=foe->GetMesh();

  ComputeStep cs=this->Internal->CurrentStep[field];
  TimedField timedField;
  timedField.first=field;
  timedField.second=cs;

  TimedFieldOverEntity & ref=
      this->Internal->FieldOnFamilyOnEntityCache[timedField][foe];
  if(foe->GetPointOrCell()==vtkMedUtilities::OnPoint)
    {
    // first case when I can shallow copy is if all the families are here
    this->MedDriver->LoadPointFamilyIds(mesh);
    if(mesh->GetNumberOfPointFamilyOnEntity()<=1
        &&ref.StepOnMesh->GetHasProfile()==0)
      return true;

    // second case is if all the families match exactly the profile
    if(ref.StepOnMesh->GetHasProfile()==0)
      return false;

    vtkMedProfile* profile=this->GetProfile(mesh, field, ref.Entity, ref.Step,
        ref.StepOnMesh);

    return this->IsPointFamilyOnEntityOnPointProfile(foe, profile, true);
    }// support OnPoint
  else
    {
    if(field->GetType()!=vtkMedField::PointField)
      {//cell field on cell support
      vtkMedEntityArray* array=foe->GetEntityArray();
      this->MedDriver->LoadFamilyIds(mesh, array);

      if(array->GetNumberOfFamilyOnEntity()<=1&&ref.StepOnMesh->GetHasProfile()
          ==0)
        {
        return true;
        }
      // then I have to test if the profile exactly matches the families
      if(ref.StepOnMesh->GetHasProfile()!=1)
        return false;

      vtkMedProfile* profile=this->GetProfile(mesh, field, ref.Entity,
          ref.Step, ref.StepOnMesh);

      return this->IsCellFamilyOnEntityOnCellProfile(foe, profile, true);
      }
    else
      {//point field on cell support
      this->MedDriver->LoadPointFamilyIds(mesh);

      // first case : all the points of the mesh have to be loaded,
      // and there is no profile
      if(mesh->GetNumberOfCellFamily()<=1&&ref.StepOnMesh->GetHasProfile()==0)
        {
        return true;
        }
      // then I have to test if the profile exactly matches the families
      vtkMedProfile* profile=this->GetProfile(mesh, field, ref.Entity,
          ref.Step, ref.StepOnMesh);

      return this->IsCellFamilyOnEntityOnPointProfile(foe, profile, true);
      }
    }// support OnCell
}

vtkMedProfile*
vtkMedReader::GetProfile(vtkMedMesh* mesh, vtkMedField* field,
    vtkMedFieldOverEntity* foe, vtkMedFieldStep* step,
    vtkMedFieldStepOnMesh* som)
{
  if(som->GetHasProfile()==0)
    return NULL;

  if(som->GetMetaInfo()==0)
    this->MedDriver->Load(mesh, field, foe, step, som);

  return this->MedFile->GetProfile(som->GetProfileName());
}

vtkMedQuadratureDefinition*
vtkMedReader::GetQuadratureDefinition(vtkMedMesh* mesh, vtkMedField* field,
    vtkMedFieldOverEntity* foe, vtkMedFieldStep* step,
    vtkMedFieldStepOnMesh* som)
{
  if(field->GetType()!=vtkMedField::QuadratureField)
    return NULL;

  if(som->GetMetaInfo()==0)
    this->MedDriver->Load(mesh, field, foe, step, som);

  return this->MedFile->GetQuadratureDefinition(
      som->GetQuadratureDefinitionName());
}

int vtkMedReader::GetQuadratureDefinitionKey(vtkMedMesh* mesh,
    vtkMedField* field, vtkMedFieldOverEntity* foe, vtkMedFieldStep* step,
    vtkMedFieldStepOnMesh* som)
{
  vtkMedQuadratureDefinition* def=this->GetQuadratureDefinition(mesh, field,
      foe, step, som);
  if(def)
    return def->GetMedIndex();
  if(som->GetNumberOfQuadraturePoint()==1)//1 quadrature point at the cell center
    return this->MedFile->GetNumberOfQuadratureDefinition()+1
        +foe->GetGeometry();
  return -foe->GetGeometry();//ELNO
}

void vtkMedReader::InitializeQuadratureOffsets(vtkMedField* field,
    vtkMedFamilyOnEntity* foe)
{
  ComputeStep cs=this->Internal->CurrentStep[field];
  TimedField timedField;
  timedField.first=field;
  timedField.second=cs;
  vtkMedMesh *mesh=foe->GetMesh();

  // then I compute the quadrature key if needed, and try to find the quadrature offsets.
  if(this->Internal->QuadratureOffsetCache.find(foe)
      ==this->Internal->QuadratureOffsetCache.end())
    this->Internal->QuadratureOffsetCache[foe]=map<QuadratureKey,
        vtkSmartPointer<vtkIdTypeArray> > ();

  map<QuadratureKey, vtkSmartPointer<vtkIdTypeArray> >& quadOffsets=
      this->Internal->QuadratureOffsetCache[foe];
  ;
  map<vtkMedFamilyOnEntity*, TimedFieldOverEntity>& refs=
      this->Internal->FieldOnFamilyOnEntityCache[timedField];

  TimedFieldOverEntity& ref=refs[foe];
  vtkMedFieldStepOnMesh* som=ref.Step->GetStepOnMesh(mesh);
  QuadratureKey quadkey=this->GetQuadratureDefinitionKey(mesh, field,
      ref.Entity, ref.Step, ref.StepOnMesh);

  if(this->Internal->QuadOffsetKey.find(foe)
      ==this->Internal->QuadOffsetKey.end())
    this->Internal->QuadOffsetKey[foe]=map<TimedField, QuadratureKey> ();
  this->Internal->QuadOffsetKey[foe][timedField]=quadkey;
  if(quadOffsets.find(quadkey)!=quadOffsets.end())
    {// the quadrature offset array has already been created
    return;
    }

  vtkIdTypeArray* qoffsets=vtkIdTypeArray::New();
  quadOffsets[quadkey]=qoffsets;
  qoffsets->Delete();

  string name=field->GetName()->GetString();
  ostringstream sstr;
  int type=0;
  if(name.find("ELNO")!=string::npos)
    {
    sstr<<"ELNO_Offset";
    type=1;
    }
  else if(name.find("ELGA")!=string::npos)
    {
    sstr<<"ELGA_Offset";
    type=2;
    }
  else
    {
    sstr<<"QuadraturePointOffset";
    }
  int nelga=0;
  int nelno=0;
  for(map<QuadratureKey, vtkSmartPointer<vtkIdTypeArray> >::iterator it=
      quadOffsets.begin(); it!=quadOffsets.end(); it++)
    {
    vtkIdTypeArray* qoff=it->second;
    if(qoff!=NULL&&qoff->GetName()!=NULL)
      {
      string name=qoff->GetName();
      if(name.find("ELNO")!=string::npos)
        nelno++;
      else if(name.find("ELGA")!=string::npos)
        nelga++;
      }
    }
  if(type==1&&nelno>1)
    sstr<<nelno-1;
  if(type==2&&nelga>1)
    sstr<<nelga-1;

  qoffsets->SetName(sstr.str().c_str());

  vtkSmartPointer<vtkMedQuadratureDefinition> def=
      this->GetQuadratureDefinition(mesh, field, ref.Entity, ref.Step,
          ref.StepOnMesh);
  if(def!=NULL)
    {
    this->MedDriver->Load(def);
    }
  else if(ref.StepOnMesh->GetNumberOfQuadraturePoint()==1)
    {// cell-centered fields can be seen as quadrature fields with 1 quadrature point at the center
    vtkMedQuadratureDefinition* center=vtkMedQuadratureDefinition::New();
    def=center;
    center->Delete();
    center->BuildCenter(ref.Entity->GetGeometry());
    }
  else
    {// ELNO fields have no vtkMedQuadratureDefinition, I need to create a dummy one
    vtkMedQuadratureDefinition* elno=vtkMedQuadratureDefinition::New();
    def=elno;
    elno->Delete();
    elno->BuildELNO(ref.Entity->GetGeometry());
    }
  this->AddQuadratureSchemeDefinition(qoffsets->GetInformation(), def);
}

void vtkMedReader::CreateVTKFieldOnSupport(vtkMedField* field,
    vtkMedFamilyOnEntity* foe)
{
  ComputeStep cs=this->Internal->CurrentStep[field];
  TimedField timedField;
  timedField.first=field;
  timedField.second=cs;

  vtkMedMesh* mesh=foe->GetMesh();
  vtkMedUnstructuredGrid* ugrid=vtkMedUnstructuredGrid::SafeDownCast(
      mesh->GetGrid());
  if(!ugrid)
    {
    // TODO deal with structured data sets
    return;
    }

  if(this->Internal->FieldCache.find(foe)==this->Internal->FieldCache.end())
    this->Internal->FieldCache[foe]=map<TimedField, VTKField> ();

  this->Internal->FieldCache[foe][timedField]=VTKField();
  VTKField& vtkfield=this->Internal->FieldCache[foe][timedField];

  vtkDataArray* data=vtkMedUtilities::NewArray(field->GetDataType());
  vtkfield.DataArray=data;
  data->Delete();
  data->SetNumberOfComponents(field->GetNumberOfComponent());
#if VTK_MAJOR_VERSION >= 5 and VTK_MINOR_VERSION >= 7
  for(int comp=0; comp<field->GetNumberOfComponent(); comp++)
    {
    data->SetComponentName(comp, vtkMedUtilities::SimplifyName(
        field->GetComponentName(comp)).c_str());
    }
#endif

  med_int fid=foe->GetFamily()->GetId();

  map<vtkMedFamilyOnEntity*, TimedFieldOverEntity>& refs=
      this->Internal->FieldOnFamilyOnEntityCache[timedField];

  TimedFieldOverEntity& ref=refs[foe];
  this->MedDriver->Load(foe->GetMesh(), field, ref.Entity, ref.Step,
      ref.StepOnMesh);

  bool createOffsets=false;
  if(field->GetType()==vtkMedField::QuadratureField)
    {
    this->InitializeQuadratureOffsets(field, foe);
    QuadratureKey& quadKey=this->Internal->QuadOffsetKey[foe][timedField];
    vtkfield.QuadratureIndexArray
        =this->Internal->QuadratureOffsetCache[foe][quadKey];
    vtkDataSet* ds=this->Internal->CurrentDataSet[foe];
    if(vtkfield.QuadratureIndexArray->GetNumberOfTuples()
        !=ds->GetNumberOfCells())
      {
      vtkfield.QuadratureIndexArray->SetNumberOfTuples(0);
      createOffsets=true;
      }
    }

  if(this->CanShallowCopyMed2VTK(field, foe))
    {
    TimedFieldOverEntity & ref=refs[foe];
    vtkMedFieldOverEntity* entity=ref.Entity;
    vtkMedFieldStep* step=ref.Step;
    vtkMedFieldStepOnMesh* stepOnMesh=ref.StepOnMesh;
    // first, build the quadrature offset array if needed
    if(createOffsets)
      {
      vtkIdType noffsets;
      int nquad=stepOnMesh->GetNumberOfQuadraturePoint();

      noffsets=stepOnMesh->GetData()->GetNumberOfTuples()/nquad;

      vtkDataSet* ds=this->Internal->CurrentDataSet[foe];
      if(noffsets!=ds->GetNumberOfCells())
        {
        vtkErrorMacro(
            "number of quadrature offsets (" << noffsets << ") must match the number of cells (" << ds->GetNumberOfCells() << ")!");
        }

      vtkfield.QuadratureIndexArray->SetNumberOfTuples(noffsets);
      for(vtkIdType id=0; id<noffsets; id++)
        {
        vtkfield.QuadratureIndexArray->SetValue(id, nquad*id);
        }
      }

    vtkfield.DataArray=stepOnMesh->GetData();
    return;
    }

  if(foe->GetPointOrCell()==vtkMedUtilities::OnCell&&field->GetType()
      !=vtkMedField::PointField)
    {
    TimedFieldOverEntity & ref=refs[foe];
    vtkMedFieldOverEntity* entity=ref.Entity;
    vtkMedFieldStep* step=ref.Step;
    vtkMedFieldStepOnMesh* stepOnMesh=ref.StepOnMesh;

    vtkMedIntArray* famIdArray=foe->GetEntityArray()->GetFamilyIds();
    med_int* famIds=(famIdArray==NULL? NULL: famIdArray->GetPointer(0));
    int nquad=1;
    if(field->GetType()==vtkMedField::QuadratureField)
      {
      nquad=stepOnMesh->GetNumberOfQuadraturePoint();
      }
    vtkMedIntArray* profids=NULL;
    if(stepOnMesh->GetHasProfile())
      {
      vtkMedProfile* profile=this->GetProfile(mesh, field, entity, step,
          stepOnMesh);
      if(profile)
        {
        profids=profile->GetIds();
        }
      }
    vtkIdType maxIndex=stepOnMesh->GetData()->GetNumberOfTuples();
    maxIndex/=nquad;

    vtkIdType destIndex=0;
    vtkIdType quadIndex=0;
    for(vtkIdType id=0; id<maxIndex; id++)
      {
      vtkIdType realIndex=(profids!=NULL? profids->GetValue(id)-1: id);
      med_int famId=(famIds!=NULL? famIds[realIndex]: 0);
      if(famId!=fid)
        continue;

      if(field->GetType()==vtkMedField::QuadratureField)
        {
        for(int q=0; q<nquad; q++)
          {
          vtkfield.DataArray->InsertNextTuple(nquad*realIndex+q,
              stepOnMesh->GetData());
          }
        if(createOffsets)
          {
          vtkfield.QuadratureIndexArray->InsertNextValue(quadIndex);
          quadIndex+=nquad;
          }
        }
      else
        {
        vtkfield.DataArray->InsertNextTuple(id, stepOnMesh->GetData());
        }
      }//copy all tuples
    vtkfield.DataArray->Squeeze();
    return;
    }

  if(foe->GetPointOrCell()==vtkMedUtilities::OnCell&&field->GetType()
      ==vtkMedField::PointField)
    {// point field on cell support

    bool useMed2VTKIndex=(this->Internal->Med2VTKPointIndex.find(foe)
        !=this->Internal->Med2VTKPointIndex.end());
    map<vtkIdType, vtkIdType> tmp;
    const map<vtkIdType, vtkIdType>& med2VTKIndex=
        (useMed2VTKIndex? this->Internal->Med2VTKPointIndex[foe]: tmp);

    TimedFieldOverEntity & ref=refs[foe];
    vtkMedFieldOverEntity* entity=ref.Entity;
    vtkMedFieldStep* step=ref.Step;
    vtkMedFieldStepOnMesh* stepOnMesh=ref.StepOnMesh;

    vtkMedIntArray* profids=NULL;
    if(stepOnMesh->GetHasProfile())
      {
      vtkMedProfile* profile=this->GetProfile(mesh, field, ref.Entity,
          ref.Step, ref.StepOnMesh);

      this->MedDriver->Load(profile);

      profids=profile->GetIds();
      }//has profile

    vtkIdType maxId=stepOnMesh->GetData()->GetNumberOfTuples();

    for(vtkIdType id=0; id<maxId; id++)
      {
      // if I have a profile, then I should insert the value at the position given in the profile.
      vtkIdType destIndex;
      if(profids!=NULL)
        {
        destIndex=profids->GetValue(id)-1; // -1 because med indices start at 1
        }
      else
        {
        destIndex=id;
        }

      // if I use the med2VTKIndex, then the index to insert this value is given by the map.
      if(useMed2VTKIndex)
        {
	map<vtkIdType, vtkIdType>::const_iterator itr = med2VTKIndex.find(destIndex);
        if(itr==med2VTKIndex.end())
          {
          // this point is not used by this familyOnEntity, skip it.
          continue;
          }
        else
          {
          destIndex=(*itr).second;
          }
        }
      data->InsertTuple(destIndex, id, stepOnMesh->GetData());
      }
    data->Squeeze();
    return;
    }// point field on cell support

  if(foe->GetPointOrCell()==vtkMedUtilities::OnPoint&&field->GetType()
      ==vtkMedField::PointField)
    {//support OnPoint

    this->MedDriver->LoadPointFamilyIds(mesh);

    bool useMed2VTKIndex=(this->Internal->Med2VTKPointIndex.find(foe)
        !=this->Internal->Med2VTKPointIndex.end());
    map<vtkIdType, vtkIdType> tmp;
    const map<vtkIdType, vtkIdType>& med2VTKIndex=
        (useMed2VTKIndex? this->Internal->Med2VTKPointIndex[foe]: tmp);

    vtkMedIntArray* famIds=mesh->GetPointFamilyIds();

    TimedFieldOverEntity & ref=refs[foe];
    vtkMedFieldOverEntity* entity=ref.Entity;
    vtkMedFieldStep* step=ref.Step;
    vtkMedFieldStepOnMesh* stepOnMesh=ref.StepOnMesh;
    med_int fid=foe->GetFamily()->GetId();

    if(stepOnMesh->GetMetaInfo()==0&&stepOnMesh->GetHasProfile()==1)
      {
      this->MedDriver->Load(mesh, field, ref.Entity, ref.Step, ref.StepOnMesh);
      }
    vtkMedIntArray* profids=NULL;
    if(stepOnMesh->GetHasProfile())
      {
      vtkMedProfile* profile=this->GetProfile(mesh, field, entity, step,
          stepOnMesh);

      this->MedDriver->Load(profile);
      vtkIdType profIndex=0;
      profids=profile->GetIds();
      }//has profile

    vtkIdType maxId=stepOnMesh->GetData()->GetNumberOfTuples();
    if(profids!=NULL)
      maxId=(profids->GetNumberOfTuples()<maxId? profids->GetNumberOfTuples()
          : maxId);

    vtkIdType destIndex=0;
    for(vtkIdType id=0; id<maxId; id++)
      {
      vtkIdType realIndex=id;
      if(profids!=NULL)
        {
        realIndex=profids->GetValue(id)-1; // -1 because med indices start at 1
        }
      med_int famId=(famIds!=NULL? famIds->GetValue(realIndex): 0);
      if(famId!=fid)
        continue;

      data->InsertNextTuple(stepOnMesh->GetData()->GetTuple(realIndex));
      }
    data->Squeeze();
    return;
    }// support on point
}

void vtkMedReader::GetCellPoints(vtkMedMesh* mesh, vtkMedEntityArray* array,
    vtkIdType index, vtkIdList* ids)
{
  ids->Initialize();
  if(array->GetGeometry()==MED_POLYEDRE)
    {
    vtkMedIntArray* conn=array->GetConnectivityArray();
    vtkMedIntArray* index0=array->GetIndex0();
    vtkMedIntArray* index1=array->GetIndex1();
    med_int start=index0->GetValue(index)-1;
    med_int end=index0->GetValue(index+1)-1;
    // the use of a set loses the order, but VTK do not support this order anyway.
    set<med_int> pts;
    if(array->GetConnectivity()==MED_NOD)
      {
      for(int ff=start; ff<end; ff++)
        {
        med_int fstart=index1->GetValue(ff)-1;
        med_int fend=index1->GetValue(ff+1)-1;
        for(int pt=fstart; pt<fend; pt++)
          {
          pts.insert(conn->GetValue(pt)-1);
          }
        }
      }
    else // MED_DESC
      {
      vtkMedUnstructuredGrid* grid=vtkMedUnstructuredGrid::SafeDownCast(
          mesh->GetGrid());
      if(!grid)
        {
        vtkErrorMacro(
            "MED_DESC connectivity is only supported on unstructured grids");
        return;
        }
      vtkIdList* subIds=vtkIdList::New();
      for(int ff=start; ff<end; ff++)
        {
        med_int fid=conn->GetValue(ff)-1;
        med_geometrie_element geometry=
            (med_geometrie_element) index1->GetValue(ff);
        vtkMedEntityArray* subarray=grid->GetEntityArray(MED_FACE, geometry);
        this->GetCellPoints(mesh, subarray, fid, subIds);
        for(int id=0; id<subIds->GetNumberOfIds(); id++)
          pts.insert(subIds->GetId(id)-1);
        }
      subIds->Delete();
      }
    for(set<med_int>::iterator it=pts.begin(); it!=pts.end(); it++)
      {
      ids->InsertNextId(*it);
      }
    }
  else if(array->GetGeometry()==MED_POLYGONE)
    {
    vtkMedIntArray* conn=array->GetConnectivityArray();
    vtkMedIntArray* nids=array->GetIndex0();
    med_int start=nids->GetValue(index)-1;
    med_int end=nids->GetValue(index+1)-1;
    if(array->GetConnectivity()==MED_NOD)
      {
      for(int pt=start; pt<end; pt++)
        {
        ids->InsertNextId(conn->GetValue(pt)-1);
        }
      }
    else // MED_DESC
      {
      //set<med_int> pts;
      vtkIdList* subpts=vtkIdList::New();
      vtkMedUnstructuredGrid* grid=vtkMedUnstructuredGrid::SafeDownCast(
          mesh->GetGrid());
      if(!grid)
        {
        vtkErrorMacro(
            "MED_DESC connectivity is only supported on unstructured grids");
        return;
        }
      for(int sub=start; sub<end; sub++)
        {
        med_int subid=conn->GetValue(sub)-1;
        med_geometrie_element subgeo=MED_SEG2;
        med_entite_maillage subtype=MED_ARETE;
        vtkMedEntityArray* subarray=grid->GetEntityArray(subtype, subgeo);
        this->GetCellPoints(mesh, subarray, subid, subpts);
        // for each edge, I only add the first vertex to the VTK list,
        //the second vertex should be equal to the first of the next edge.
        ids->InsertNextId(subpts->GetId(0));
        }
      subpts->Delete();
      }
    }
  else if(array->GetConnectivity()==MED_NOD||vtkMedUtilities::GetDimension(
      array->GetGeometry())<2)
    {
    int npts=vtkMedUtilities::GetNumberOfPoint(array->GetGeometry());
    vtkMedIntArray* conn=array->GetConnectivityArray();
    for(int i=0; i<npts; i++)
      {
      ids->InsertNextId(conn->GetValue(npts*index+i)-1);
      }
    }
  else
    {
    vtkIdList* subpts=vtkIdList::New();
    int nsub=vtkMedUtilities::GetNumberOfSubEntity(array->GetGeometry());
    vtkMedUnstructuredGrid* grid=vtkMedUnstructuredGrid::SafeDownCast(
        mesh->GetGrid());
    if(!grid)
      {
      vtkErrorMacro(
          "MED_DESC connectivity is only supported on unstructured grids");
      return;
      }
    vtkMedIntArray* conn=array->GetConnectivityArray();
    ids->SetNumberOfIds(vtkMedUtilities::GetNumberOfPoint(array->GetGeometry()));
    for(int sub=0; sub<nsub; sub++)
      {
      med_int subid=conn->GetValue(nsub*index+sub)-1;
      med_geometrie_element subgeo=vtkMedUtilities::GetSubGeometry(
          array->GetGeometry(), sub);
      med_entite_maillage subtype=vtkMedUtilities::GetSubType(array->GetType());
      vtkMedEntityArray* subarray=grid->GetEntityArray(subtype, subgeo);
      this->GetCellPoints(mesh, subarray, subid, subpts);
      vtkMedUtilities::ProjectConnectivity(array->GetGeometry(), ids, subpts,
          sub);
      }
    subpts->Delete();
    }
}

int vtkMedReader::HasMeshAnyCellSelectedFamily(vtkMedMesh* mesh)
{
  int nfam=mesh->GetNumberOfFamily();
  for(int famid=0; famid<nfam; famid++)
    {
    vtkMedFamily* fam=mesh->GetFamily(famid);
    if(fam->GetPointOrCell()!=vtkMedUtilities::OnCell||!this->GetFamilyStatus(
        mesh, fam))
      continue;
    return true;
    }
  return false;
}

int vtkMedReader::HasMeshAnyPointSelectedFamily(vtkMedMesh* mesh)
{
  int nfam=mesh->GetNumberOfFamily();
  for(int famid=0; famid<nfam; famid++)
    {
    vtkMedFamily* fam=mesh->GetFamily(famid);
    if(fam->GetPointOrCell()!=vtkMedUtilities::OnPoint||!this->GetFamilyStatus(
        mesh, fam))
      continue;
    return true;
    }
  return false;
}

void vtkMedReader::BuildSIL(vtkMutableDirectedGraph* sil, int onlySelected)
{
  if(sil==NULL)
    return;

  sil->Initialize();

  vtkSmartPointer<vtkVariantArray> childEdge=
      vtkSmartPointer<vtkVariantArray>::New();
  childEdge->InsertNextValue(0);

  vtkSmartPointer<vtkVariantArray> crossEdge=
      vtkSmartPointer<vtkVariantArray>::New();
  crossEdge->InsertNextValue(1);

  // CrossEdge is an edge linking hierarchies.
  vtkUnsignedCharArray* crossEdgesArray=vtkUnsignedCharArray::New();
  crossEdgesArray->SetName("CrossEdges");
  sil->GetEdgeData()->AddArray(crossEdgesArray);
  crossEdgesArray->Delete();
  vtkstd::deque<vtkstd::string> names;

  // Now build the hierarchy.
  vtkIdType rootId=sil->AddVertex();
  names.push_back("SIL");

  // Add a global entry to encode global names for the families
  vtkIdType globalFamilyRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Families");

  // Add a global entry to encode global names for the families
  vtkIdType globalGroupRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Groups");

  // Add the groups subtree
  vtkIdType groupsRoot=sil->AddChild(rootId, childEdge);
  names.push_back("GroupTree");

  // Add the attributes subtree
  vtkIdType attributesRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Attributes");

  // Add a global entry to encode names for the cell types
  vtkIdType globalEntityRoot=sil->AddChild(rootId, childEdge);
  names.push_back("CellTypes");

  // Add the cell types subtree
  vtkIdType entityTypesRoot=sil->AddChild(rootId, childEdge);
  names.push_back("CellTypeTree");

  // this is the map that keep added cell types
  map<Entity, vtkIdType> entityMap;
  map<med_entite_maillage, vtkIdType> typeMap;

  // this map will keep all added attributes
  map<vtkstd::string, vtkstd::map<med_int, vtkIdType> > attributeMap;
  map<vtkstd::string, vtkIdType> descMap;

  int numMeshes=this->GetMedFile()->GetNumberOfMesh();
  for(int meshIndex=0; meshIndex<numMeshes; meshIndex++)
    {
    vtkMedMesh* mesh=this->GetMedFile()->GetMesh(meshIndex);

    // add all the cell types, only valid for unstructured grids
    vtkMedUnstructuredGrid* ugrid=vtkMedUnstructuredGrid::SafeDownCast(
        mesh->GetGrid());
    if(ugrid!=NULL)
      {
      for(int entityIndex=0; entityIndex<ugrid->GetNumberOfEntityArray(); entityIndex++)
        {
        vtkMedEntityArray* array=ugrid->GetEntityArray(entityIndex);
        med_entite_maillage type=array->GetType();
        med_geometrie_element geometry=array->GetGeometry();

        if(onlySelected&&!this->GetCellTypeStatus(type, geometry))
          continue;

        Entity entity=make_pair(type, geometry);
        if(entityMap.find(entity)==entityMap.end())
          {
          vtkIdType entityGlobalId=sil->AddChild(globalEntityRoot, childEdge);
          names.push_back(vtkMedUtilities::CellTypeKey(type, geometry));

          vtkIdType typeId;
          if(typeMap.find(type)==typeMap.end())
            {
            typeId=sil->AddChild(entityTypesRoot, childEdge);
            names.push_back(vtkMedUtilities::TypeName(type));
            typeMap[type]=typeId;
            }
          else
            {
            typeId=typeMap[type];
            }
          vtkIdType entityId=sil->AddChild(typeId, childEdge);
          names.push_back(vtkMedUtilities::GeometryName(geometry));

          sil->AddEdge(entityId, entityGlobalId, crossEdge);

          entityMap[entity]=entityId;
          }
        }
      }
    bool hasCellFamily=true;
    bool hasPointFamily=true;
    if(onlySelected)
      {
      hasCellFamily=this->HasMeshAnyCellSelectedFamily(mesh);
      hasPointFamily=this->HasMeshAnyPointSelectedFamily(mesh);

      if(!hasCellFamily&&!hasPointFamily)
        continue;
      }

    vtkIdType meshGroup=sil->AddChild(groupsRoot, childEdge);
    names.push_back(vtkMedUtilities::SimplifyName(mesh->GetName()));

    // add the two OnPoint and OnCell entries, for groups and for families
    vtkIdType meshCellGroups=-1;
    if(!onlySelected||hasCellFamily)
      {
      meshCellGroups=sil->AddChild(meshGroup, childEdge);
      names.push_back(vtkMedUtilities::OnCellName);
      //sil->AddEdge(cellSupportRoot, meshCellGroups, crossEdge);
      }

    vtkIdType meshPointGroups=-1;
    if(!onlySelected||hasPointFamily)
      {
      meshPointGroups=sil->AddChild(meshGroup, childEdge);
      names.push_back(vtkMedUtilities::OnPointName);
      //sil->AddEdge(pointSupportRoot, meshPointGroups, crossEdge);
      }
    // this maps will keep all added groups on nodes/cells
    map<vtkMedGroup*, vtkIdType> nodeGroupMap;
    map<vtkMedGroup*, vtkIdType> cellGroupMap;

    // add all families
    for(int famIndex=0; famIndex<mesh->GetNumberOfFamily(); famIndex++)
      {
      vtkMedFamily* family=mesh->GetFamily(famIndex);
      // family with Id 0 is both on cell and on points, so add it to both
      map<vtkMedGroup*, vtkIdType> & groupMap=(family->GetPointOrCell()
          ==vtkMedUtilities::OnPoint? nodeGroupMap: cellGroupMap);
      vtkIdType groupRootId=
          (family->GetPointOrCell()==vtkMedUtilities::OnPoint? meshPointGroups
              : meshCellGroups);

      if(onlySelected&&!this->GetFamilyStatus(mesh, family))
        continue;

      vtkIdType familyGlobalId=sil->AddChild(globalFamilyRoot, childEdge);
      names.push_back(vtkMedUtilities::FamilyKey(mesh->GetName()->GetString(),
          family->GetPointOrCell(), family->GetName()->GetString()));

      // if this family has no group, create a dummy NoGroup group entry
      if(family->GetNumberOfGroup()==0)
        {
        vtkIdType groupGlobalId;
        if(groupMap.find(NULL)==groupMap.end())
          {
          vtkIdType groupLocalId;
          groupLocalId=sil->AddChild(groupRootId, childEdge);
          names.push_back(vtkMedUtilities::NoGroupName);

          groupGlobalId=sil->AddChild(globalGroupRoot, childEdge);
          names.push_back(vtkMedUtilities::GroupKey(
              mesh->GetName()->GetString(), family->GetPointOrCell(),
              vtkMedUtilities::NoGroupName));

          sil->AddEdge(groupLocalId, groupGlobalId, crossEdge);

          groupMap[NULL]=groupGlobalId;
          }
        else
          {
          groupGlobalId=groupMap[NULL];
          }
        // add a cross link from this group to this family
        sil->AddEdge(groupGlobalId, familyGlobalId, crossEdge);
        }
      // add all the groups of this family
      for(vtkIdType groupIndex=0; groupIndex<family->GetNumberOfGroup(); groupIndex++)
        {
        vtkMedGroup* group=family->GetGroup(groupIndex);
        vtkIdType groupGlobalId;
        if(groupMap.find(group)==groupMap.end())
          {
          vtkIdType groupLocalId;
          groupLocalId=sil->AddChild(groupRootId, childEdge);
          names.push_back(vtkMedUtilities::SimplifyName(group->GetName()));

          groupGlobalId=sil->AddChild(globalGroupRoot, childEdge);
          names.push_back(vtkMedUtilities::GroupKey(
              mesh->GetName()->GetString(), family->GetPointOrCell(),
              group->GetName()->GetString()));
          groupMap[group]=groupGlobalId;

          sil->AddEdge(groupLocalId, groupGlobalId, crossEdge);
          }
        else
          {
          groupGlobalId=groupMap[group];
          }
        // add a cross link from this group to this family
        sil->AddEdge(groupGlobalId, familyGlobalId, crossEdge);
        }//groupIndex

      // add all the attributes of this family
      for(int attrIndex=0; attrIndex<family->GetNumberOfAttribute(); attrIndex++)
        {
        vtkMedAttribute* attribute=family->GetAttribute(attrIndex);
        vtkstd::string desc=vtkMedUtilities::SimplifyName(
            attribute->GetDescription());
        vtkIdType attributeId;
        vtkIdType descId;
        if(descMap.find(desc)==descMap.end())
          {
          attributeMap[desc]=map<med_int, vtkIdType> ();
          descId=sil->AddChild(attributesRoot, childEdge);
          names.push_back(desc);
          descMap[desc]=descId;
          }
        else
          {
          descId=descMap[desc];
          }
        map<med_int, vtkIdType>& attIds=attributeMap[desc];
        if(attIds.find(attribute->GetValue())==attIds.end())
          {
          std::ostringstream sstr;
          sstr<<attribute->GetValue();
          attributeId=sil->AddChild(descId, childEdge);
          names.push_back(sstr.str());
          attIds[attribute->GetValue()]=attributeId;
          }
        else
          {
          attributeId=attIds[attribute->GetValue()];
          }
        // add a cross link from this attribute to this family
        sil->AddEdge(attributeId, familyGlobalId, crossEdge);
        }
      }//famIndex
    }//meshIndex

  // This array is used to assign names to nodes.
  vtkStringArray* namesArray=vtkStringArray::New();
  namesArray->SetName("Names");
  namesArray->SetNumberOfTuples(sil->GetNumberOfVertices());
  sil->GetVertexData()->AddArray(namesArray);
  namesArray->Delete();
  vtkstd::deque<vtkstd::string>::iterator iter;
  vtkIdType cc;
  for(cc=0, iter=names.begin(); iter!=names.end(); ++iter, ++cc)
    {
    namesArray->SetValue(cc, (*iter).c_str());
    }
}

void vtkMedReader::ClearMedSupports()
{
  this->Internal->DataSetCache.clear();
  this->Internal->Med2VTKPointIndex.clear();

  vtkMedFile* file=this->GetMedFile();
  int numMeshes=file->GetNumberOfMesh();
  for(int meshIndex=0; meshIndex<numMeshes; meshIndex++)
    {
    vtkMedMesh* mesh=this->GetMedFile()->GetMesh(meshIndex);
    vtkMedUnstructuredGrid* ugrid=vtkMedUnstructuredGrid::SafeDownCast(
        mesh->GetGrid());
    if(ugrid!=NULL)
      {
      for(int entityIndex=0; entityIndex<ugrid->GetNumberOfEntityArray(); entityIndex++)
        {
        vtkMedEntityArray* array=ugrid->GetEntityArray(entityIndex);
        if(array->GetConnectivityArray()!=NULL)
          array->GetConnectivityArray()->Initialize();
        if(array->GetGlobalIds()!=NULL)
          array->GetGlobalIds()->Initialize();
        if(array->GetIndex0()!=NULL)
          array->GetIndex0()->Initialize();
        if(array->GetIndex1()!=NULL)
          array->GetIndex1()->Initialize();
        if(array->GetFamilyIds()!=NULL)
          array->GetFamilyIds()->Initialize();
        }
      }
    if(mesh->GetPointFamilyIds()!=NULL)
      mesh->GetPointFamilyIds()->Initialize();
    if(mesh->GetPointGlobalIds()!=NULL)
      mesh->GetPointGlobalIds()->Initialize();
    }

  int numProf=file->GetNumberOfProfile();
  for(int prof=0; prof<numProf; prof++)
    {
    vtkMedProfile* profile=file->GetProfile(prof);
    if(profile->GetIds()!=NULL)
      profile->GetIds()->Initialize();
    }
}

void vtkMedReader::ClearMedFields()
{
  this->Internal->FieldCache.clear();
  this->Internal->QuadOffsetKey.clear();
  this->Internal->QuadratureOffsetCache.clear();

  vtkMedFile* file=this->GetMedFile();
  int numFields=file->GetNumberOfField();
  for(int ff=0; ff<numFields; ff++)
    {
    vtkMedField* field=file->GetField(ff);
    int nfoe=field->GetNumberOfFieldOverEntity();
    for(int foeid=0; foeid<nfoe; foeid++)
      {
      vtkMedFieldOverEntity* foe=field->GetFieldOverEntity(foeid);
      for(int ss=0; ss<foe->GetNumberOfStep(); ss++)
        {
        vtkMedFieldStep * fs=foe->GetStep(ss);
        int nsom=fs->GetNumberOfStepOnMesh();
        for(int somid=0; somid<nsom; somid++)
          {
          vtkMedFieldStepOnMesh* som=fs->GetStepOnMesh(somid);
          if(som->GetData()!=NULL)
            som->GetData()->Initialize();
          }
        }
      }
    }
}

void vtkMedReader::ClearCaches(int when)
{
  switch(when)
  {
    case Initialize:
      this->Internal->CurrentDataSet.clear();
      this->Internal->DataSetCache.clear();
      this->Internal->FieldCache.clear();
      this->Internal->StepCache.clear();
      this->Internal->UsedSupports.clear();
      this->Internal->ProfileMatch.clear();
      this->Internal->FieldOnFamilyOnEntityCache.clear();
      this->Internal->FieldMatchCache.clear();
      this->Internal->ProfileMatch.clear();
      this->Internal->QuadratureOffsetCache.clear();
      this->Internal->QuadOffsetKey.clear();
      this->Internal->Med2VTKPointIndex.clear();
      this->Internal->CurrentStep.clear();
      break;
    case StartRequest:
      this->Internal->CurrentDataSet.clear();
      this->Internal->UsedSupports.clear();
      if(this->CacheStrategy==CacheNothing)
        {
        this->ClearMedSupports();
        this->ClearMedFields();
        }
      else if(this->CacheStrategy==CacheGeometry)
        {
        this->ClearMedFields();
        }
      break;
    case AfterCreateMedSupports:
      // clear the unused supports and associated cached datasets and fields
      break;
    case EndBuildVTKSupports:
      //TODO : depending on the strategy, clear some caches
      break;
    case EndRequest:
      //TODO : depending on the strategy, clear some caches
      if(this->CacheStrategy==CacheNothing)
        {
        this->ClearMedSupports();
        this->ClearMedFields();
        }
      else if(this->CacheStrategy==CacheGeometry)
        {
        this->ClearMedSupports();
        }
      break;
  }
}

void vtkMedReader::PrintSelf(ostream& os, vtkIndent indent)
{
  PRINT_STRING(os, indent, FileName);
  PRINT_IVAR(os, indent, AnimationMode);
  PRINT_IVAR(os, indent, Time);
  PRINT_OBJECT(os, indent, PointFields);
  PRINT_OBJECT(os, indent, CellFields);
  PRINT_OBJECT(os, indent, QuadratureFields);
  PRINT_OBJECT(os, indent, Groups);
  PRINT_OBJECT(os, indent, Entities);
  PRINT_OBJECT(os, indent, MedFile);
  PRINT_OBJECT(os, indent, MedDriver);
  PRINT_IVAR(os, indent, CacheStrategy);
  this->Superclass::PrintSelf(os, indent);
}
