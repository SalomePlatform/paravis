#include "vtkMedDriver.h"

#include "vtkObjectFactory.h"
#include "vtkStringArray.h"
#include "vtkDataArray.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"

#include "vtkMedFile.h"
#include "vtkMedCartesianGrid.h"
#include "vtkMedPolarGrid.h"
#include "vtkMedStandardGrid.h"
#include "vtkMedUnstructuredGrid.h"
#include "vtkMedField.h"
#include "vtkMedMesh.h"
#include "vtkMedFamily.h"
#include "vtkMedAttribute.h"
#include "vtkMedUtilities.h"
#include "vtkMedEntityArray.h"
#include "vtkMedQuadratureDefinition.h"
#include "vtkMedProfile.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedFieldStepOnMesh.h"
#include "vtkMedFieldStep.h"
#include "vtkMedGroup.h"
#include "vtkMedString.h"
#include "vtkMedIntArray.h"

vtkCxxRevisionMacro(vtkMedDriver, "$Revision$")
vtkStandardNewMacro(vtkMedDriver)

vtkMedDriver::vtkMedDriver()
{
  this->FileName = NULL;
  this->OpenLevel = 0;
  this->FileId = -1;
}

vtkMedDriver::~vtkMedDriver()
{
  if(this->OpenLevel > 0)
    {
    vtkWarningMacro("The file " << this->FileName << " has not be closed before destructor.")
    this->OpenLevel = 1;
    this->Close();
    }
  this->SetFileName(NULL);
}

int vtkMedDriver::Open()
{
  int res = 0;
  if(this->FileName == NULL)
    {
    vtkDebugMacro("Error : FileName has not been set ");
    return -1;
    }

  if(this->OpenLevel <= 0)
    {

    med_err conforme = MEDformatConforme(this->FileName);
    if(conforme < 0)
      {
      vtkDebugMacro("The file " << this->FileName << "is not a HDF5 file, aborting.");
      return -1;
      }

    conforme = MEDversionConforme(this->FileName);
    if(conforme < 0)
      {
      vtkDebugMacro("The file " << this->FileName << "has not been written with the same version as the one currently used to read it, this may lead to errors. Please use the medimport tool.");
      }

    this->FileId = MEDouvrir(this->FileName, MED_LECTURE);
    if(this->FileId < 0)
      {
      vtkDebugMacro("Error : unable to open file " << this->FileName);
      res = -2;
      }
    this->OpenLevel = 0;
    }
  this->OpenLevel++;
  return res;
}

void vtkMedDriver::Close()
{
  this->OpenLevel--;
  if(this->OpenLevel == 0)
    {
    if(MEDfermer(this->FileId) < 0)
      {
      vtkErrorMacro("Error: unable to close the current file.");
      }
    this->FileId = -1;
    }
}

bool vtkMedDriver::CanReadFile()
{
  bool canRead = (this->Open() >= 0);
  this->Close();
  return canRead;
}

void vtkMedDriver::ReadFileVersion(int* major, int* minor, int* release)
{
  FileOpen open(this);

  med_int amajor, aminor, arelease;
  if(MEDversionLire(this->FileId, &amajor, &aminor, &arelease) < 0)
    {
    vtkErrorMacro("Impossible to read the version of this file " << this->FileName);
    return;
    }
  *major = amajor;
  *minor = aminor;
  *release = arelease;
}

// Description:
// load all meta data associated with this cartesian grid.
void vtkMedDriver::ReadMeta(vtkMedMesh* mesh, vtkMedRegularGrid* grid)
{
  FileOpen open(this);

  med_table medaxis[3] = { MED_COOR_IND1, MED_COOR_IND2, MED_COOR_IND3 };

  char* meshName = mesh->GetName()->GetString();

  // Loop over each axis
  med_int* size = new med_int[grid->GetDimension()];
  for(int axis = 0; axis < grid->GetDimension(); axis++)
    {

    size[axis] = MEDnEntMaa(FileId, meshName, medaxis[axis], MED_NOEUD,
        MED_NONE, MED_NOD);
    if(size[axis] < 0)
      {
      vtkErrorMacro("Error during indices reading.");
      }
    }
  grid->SetSize(size);

  char* names = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];
  char* units = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];

  for(int axis = 0; axis < grid->GetDimension(); axis++)
    {
    memset(names, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);
    memset(units, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);
    vtkDataArray* coords = vtkMedUtilities::NewCoordArray();
    grid->SetCoordinates(axis, coords);
    coords->Delete();
    // Read for the current axis the Indices
    if(MEDindicesCoordLire(this->FileId, meshName, grid->GetDimension(),
        static_cast<med_float*> (coords->GetVoidPointer(0)), size[axis], axis
            + 1, names, units) < 0)
      {
      vtkErrorMacro("Error during coordinates loading.");
      }
    else
      {
      char name[MED_TAILLE_PNOM + 1];
      char unit[MED_TAILLE_PNOM + 1];
      for(int comp = 0; comp < grid->GetDimension(); comp++)
        {
        memset(unit, '\0', MED_TAILLE_PNOM + 1);
        memset(name, '\0', MED_TAILLE_PNOM + 1);
        memcpy(unit, units + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM
            * sizeof(char));
        memcpy(name, names + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM
            * sizeof(char));
        grid->GetComponentName(comp)->SetString(name);
        grid->GetUnit(comp)->SetString(unit);
        }
      }
    }

  delete[] names;
  delete[] units;
}

// Description:
// load all meta data associated with this standard grid.
void vtkMedDriver::ReadMeta(vtkMedMesh* mesh, vtkMedStandardGrid* grid)
{
  FileOpen open(this);
  char* meshName = mesh->GetName()->GetString();

  med_int* nelem = new med_int[grid->GetDimension()];
  if(MEDstructureCoordLire(this->FileId, meshName, grid->GetDimension(), nelem)
      < 0)
    {
    vtkErrorMacro("Error during grid size loading.");
    }

  grid->SetSize(nelem);
  delete[] nelem;

  med_int nnodes;
  if((nnodes = MEDnEntMaa(FileId, meshName, MED_COOR, MED_NOEUD, MED_NONE,
      MED_NOD)) < 0)
    {
    vtkErrorMacro("Error while reading the number of nodes of a standard grid.");
    }

  grid->SetNumberOfPoints(nnodes);

  vtkDataArray* coords = vtkMedUtilities::NewCoordArray();
  grid->SetCoordinates(coords);
  coords->Delete();

  coords->SetNumberOfComponents(grid->GetDimension());
  coords->SetNumberOfTuples(grid->GetNumberOfPoints());
  med_repere type_rep;

  char* names = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];
  memset(names, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);
  char* units = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];
  memset(units, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);

  if(MEDcoordLire(this->FileId, meshName, grid->GetDimension(),
      static_cast<med_float*> (coords->GetVoidPointer(0)), MED_FULL_INTERLACE,
      MED_ALL, NULL, 0, &type_rep, names, units) < 0)
    {
    vtkErrorMacro("Error while loading Coordinates for mesh " << meshName);
    }

  char name[MED_TAILLE_PNOM + 1];
  char unit[MED_TAILLE_PNOM + 1];
  for(int comp = 0; comp < grid->GetDimension(); comp++)
    {
    memset(unit, '\0', MED_TAILLE_PNOM + 1);
    memset(name, '\0', MED_TAILLE_PNOM + 1);
    memcpy(unit, units + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM * sizeof(char));
    memcpy(name, names + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM * sizeof(char));
    grid->GetComponentName(comp)->SetString(name);
    grid->GetUnit(comp)->SetString(unit);
    }

  grid->SetCoordinateSystem(type_rep);

  delete[] names;
  delete[] units;
}

// Description:
// load all meta data associated with this unstructured grid.
void vtkMedDriver::ReadMeta(vtkMedMesh* mesh, vtkMedUnstructuredGrid* grid)
{
  FileOpen open(this);

  char *meshName = mesh->GetName()->GetString();
  med_entite_maillage type;
  med_geometrie_element geometry;
  med_connectivite connectivity;

  // first check if we have points
  vtkIdType npoints = MEDnEntMaa(FileId, meshName, MED_COOR, MED_NOEUD,
      MED_NONE, MED_NOD);

  if(npoints > 0)
    {
    grid->SetNumberOfPoints(npoints);
    }
  else
    {
    vtkWarningMacro("the unstructured grid has no points!")
    return;
    }

  // this triggers the creation of undefined families
  this->LoadPointFamilyIds(mesh);

  // load all cell types
  for(int entityTypeIndex = 0; entityTypeIndex
      < vtkMedUtilities::NumberOfEntityType; entityTypeIndex++)
    {
    type = vtkMedUtilities::EntityType[entityTypeIndex];
    // skip the nodes when reading the cell info
    if(type == MED_NOEUD)
      continue;
    for(int geometryIndex = 0; geometryIndex
        < vtkMedUtilities::NumberOfCellGeometry; geometryIndex++)
      {
      geometry = vtkMedUtilities::CellGeometry[geometryIndex];
      // skip the dummy MED_NONE geometry
      if(geometry == MED_NONE)
        continue;
      for(int connectivityIndex = 0; connectivityIndex
          < vtkMedUtilities::NumberOfConnectivity; connectivityIndex++)
        {
        connectivity = vtkMedUtilities::Connectivity[connectivityIndex];
        vtkIdType ncell = MEDnEntMaa(FileId, meshName, MED_CONN, type,
            geometry, connectivity);

        if(ncell > 0)
          {
          vtkMedEntityArray* array = vtkMedEntityArray::New();
          array->SetNumberOfEntity(ncell);
          array->SetGeometry(geometry);
          array->SetType(type);
          array->SetConnectivity(connectivity);
          grid->AppendEntityArray(array);
          array->Delete();
          // this triggers the creation of undefined families
          this->LoadFamilyIds(mesh, array);
          }
        }
      }
    }
}

void vtkMedDriver::ReadMeta(vtkMedMesh* mesh, vtkMedFamily* family)
{
  FileOpen open(this);

  med_int familyid;
  med_int nattr;
  med_int ngroup;
  med_int nrealattr;
  med_int nrealgroup;
  med_int* attrIds = NULL;
  med_int* attrValues = NULL;
  char* groupNames = NULL;
  char* attributeDescriptions = NULL;
  char* meshName = mesh->GetName()->GetString();

  ngroup = MEDnGroupe(FileId, meshName, family->GetMedIndex());
  nattr = MEDnAttribut(FileId, meshName, family->GetMedIndex());

  if(ngroup > 0)
    {
    groupNames = new char[ngroup * MED_TAILLE_LNOM + 1];
    memset(groupNames, '\0', ngroup * MED_TAILLE_LNOM + 1);
    }

  if(nattr > 0)
    {
    attrIds = new med_int[nattr];
    attrValues = new med_int[nattr];
    attributeDescriptions = new char[nattr * MED_TAILLE_DESC + 1];
    memset(attributeDescriptions, '\0', nattr * MED_TAILLE_DESC + 1);
    }

  // Read meta
  if(MEDfamInfo(FileId, meshName, family->GetMedIndex(),
      family->GetName()->GetWritePointer(), &familyid, attrIds, attrValues,
      attributeDescriptions, &nrealattr, groupNames, &nrealgroup) < 0)
    {
    vtkErrorMacro("vtkMedDriver::ReadMeta(vtkMedFamily* family) cannot read family informations.");
    }
  else
    {
    family->SetId(familyid);
    family->AllocateNumberOfAttribute(nrealattr);
    if(familyid < 0)
      family->SetPointOrCell(vtkMedUtilities::OnCell);
    else
      family->SetPointOrCell(vtkMedUtilities::OnPoint);

    for(int index = 0; index < nrealattr; index++)
      {
      char desc[MED_TAILLE_DESC + 1];
      memset(desc, '\0', MED_TAILLE_DESC + 1);
      memcpy(desc, attributeDescriptions + index * MED_TAILLE_DESC,
          MED_TAILLE_DESC * sizeof(char));
      vtkMedAttribute* attr = family->GetAttribute(index);
      attr->SetId(attrIds[index]);
      attr->SetValue(attrValues[index]);
      attr->GetDescription()->SetString(desc);
      }

    family->AllocateNumberOfGroup(nrealgroup);
    for(int index = 0; index < nrealgroup; index++)
      {
      char realGroupName[MED_TAILLE_LNOM + 1];
      memset(realGroupName, '\0', MED_TAILLE_LNOM + 1);
      memcpy(realGroupName, groupNames + index * MED_TAILLE_LNOM,
          MED_TAILLE_LNOM * sizeof(char));
      vtkMedGroup* group = mesh->GetGroup(family->GetPointOrCell(),
          realGroupName);
      bool found = (group != NULL);
      if(group == NULL)
        {
        group = family->GetGroup(index);
        }
      group->GetName()->SetString(realGroupName);
      group->SetPointOrCell(family->GetPointOrCell());
      family->SetGroup(index, group);
      if(!found)
        {
        if(family->GetPointOrCell() == vtkMedUtilities::OnCell)
          mesh->AppendCellGroup(group);
        else
          mesh->AppendPointGroup(group);
        }
      }
    }

  delete[] attrIds;
  delete[] attrValues;
  delete[] groupNames;
  delete[] attributeDescriptions;
}

void vtkMedDriver::ReadMeta(vtkMedFile* file)
{
  FileOpen open(this);

  MEDfichEntete(this->FileId, MED_FICH_DES,
      file->GetDescription()->GetWritePointer());

  // Get Meshes informations
  int nmesh = MEDnMaa(this->FileId);
  file->AllocateNumberOfMesh(nmesh);
  for(int i = 0; i < nmesh; i++)
    {
    vtkMedMesh* mesh = file->GetMesh(i);
    mesh->SetMedIndex(i + 1);
    this->ReadMeta(mesh);
    }

  // Get Fields informations
  int nfields = MEDnChamp(this->FileId, 0);
  file->AllocateNumberOfField(nfields);
  for(int i = 0; i < nfields; i++)
    {
    vtkMedField* field = file->GetField(i);
    field->SetMedIndex(i + 1);
    this->ReadMeta(field);
    }

  // Get Quadrature Points informations
  int ngauss = MEDnGauss(this->FileId);
  file->AllocateNumberOfQuadratureDefinition(ngauss);
  for(int i = 0; i < ngauss; i++)
    {
    vtkMedQuadratureDefinition* def = file->GetQuadratureDefinition(i);
    def->SetMedIndex(i + 1);
    this->ReadMeta(def);
    }

  int nprof = MEDnProfil(FileId);
  file->AllocateNumberOfProfile(nprof);
  for(int i = 0; i < nprof; i++)
    {
    vtkMedProfile* profile = file->GetProfile(i);
    profile->SetMedIndex(i + 1);
    this->ReadMeta(profile);
    }
}

void vtkMedDriver::ReadMeta(vtkMedProfile* profile)
{
  FileOpen open(this);

  char profileName[MED_TAILLE_NOM + 1];
  profileName[MED_TAILLE_NOM] = '\0';
  med_int nelem;

  if(MEDprofilInfo(FileId, profile->GetMedIndex(),
      profile->GetName()->GetWritePointer(), &nelem) < 0)
    {
    vtkErrorMacro("cannot read information on profile" << profile->GetMedIndex());
    }
  profile->SetNumberOfElement(nelem);
}

void vtkMedDriver::ReadMeta(vtkMedField* field)
{
  FileOpen open(this);

  int ncomp = MEDnChamp(FileId, field->GetMedIndex());
  if(ncomp < 0)
    {
    field->SetNumberOfComponent(-1);
    vtkErrorMacro("cannot read the number of component of field " << field->GetMedIndex())
    return;
    }

  char* units = new char[MED_TAILLE_PNOM * ncomp + 1];
  char* componentNames = new char[MED_TAILLE_PNOM * ncomp + 1];
  memset(units, '\0', MED_TAILLE_PNOM * ncomp + 1);
  memset(componentNames, '\0', MED_TAILLE_PNOM * ncomp + 1);

  med_type_champ dataType;
  if(MEDchampInfo(FileId, field->GetMedIndex(),
      field->GetName()->GetWritePointer(), &dataType, componentNames, units,
      ncomp) < 0)
    {
    vtkErrorMacro("cannot read the informations on field " << field->GetMedIndex())
    return;
    }

  field->SetNumberOfComponent(ncomp);
  field->SetDataType(dataType);
  for(int comp = 0; comp < ncomp; comp++)
    {
    char unit[MED_TAILLE_PNOM + 1];
    memset(unit, '\0', MED_TAILLE_PNOM + 1);
    memcpy(unit, units + MED_TAILLE_PNOM * comp, MED_TAILLE_PNOM * sizeof(char));
    field->GetUnit(comp)->SetString(unit);

    char compName[MED_TAILLE_PNOM + 1];
    memset(compName, '\0', MED_TAILLE_PNOM + 1);
    memcpy(compName, componentNames + MED_TAILLE_PNOM * comp, MED_TAILLE_PNOM
        * sizeof(char));
    field->GetComponentName(comp)->SetString(compName);
    }

  med_geometrie_element geometry;
  med_entite_maillage type;

  for(int entityTypeIndex = 0; entityTypeIndex
      < vtkMedUtilities::NumberOfEntityType; entityTypeIndex++)
    {
    type = vtkMedUtilities::EntityType[entityTypeIndex];
    if(type != MED_NOEUD)
      {
      for(int cellGeometryIndex = 0; cellGeometryIndex
          < vtkMedUtilities::NumberOfCellGeometry; cellGeometryIndex++)
        {
        geometry = vtkMedUtilities::CellGeometry[cellGeometryIndex];
        int nstep = MEDnPasdetemps(FileId, field->GetName()->GetString(), type,
            geometry);
        if(nstep > 0)
          {
          bool elno = (type == MED_NOEUD_MAILLE);// || (type == MED_MAILLE && geometry == MED_NONE) );
          vtkMedFieldOverEntity* fieldOverEntity = vtkMedFieldOverEntity::New();
          field->AppendFieldOverEntity(fieldOverEntity);
          fieldOverEntity->Delete();
          fieldOverEntity->SetMedType(type);
          fieldOverEntity->SetGeometry(geometry);
          fieldOverEntity->AllocateNumberOfStep(nstep);
          fieldOverEntity->SetIsELNO(elno);

          this->ReadMeta(field, fieldOverEntity);
          }//nstep>0
        }// cellTypeIndex
      }
    else // (type != MED_NOEUD)
      {
      int nstep = MEDnPasdetemps(FileId, field->GetName()->GetString(),
          MED_NOEUD, MED_NONE);
      if(nstep > 0)
        {
        vtkMedFieldOverEntity* fieldOverEntity = vtkMedFieldOverEntity::New();
        field->AppendFieldOverEntity(fieldOverEntity);
        fieldOverEntity->Delete();
        fieldOverEntity->SetMedType(MED_NOEUD);
        fieldOverEntity->SetGeometry(MED_NONE);
        fieldOverEntity->AllocateNumberOfStep(nstep);

        this->ReadMeta(field, fieldOverEntity);
        }//nstep>0
      }//else (type != MED_NOEUD)
    }//entityTypeIndex

  field->ComputeFieldSupportType();

  delete[] units;
  delete[] componentNames;
}

void vtkMedDriver::ReadMeta(vtkMedField* field,
    vtkMedFieldOverEntity* fieldOverEntity)
{
  FileOpen open(this);

  char* fieldName = field->GetName()->GetString();

  for(int stepIndex = 0; stepIndex < fieldOverEntity->GetNumberOfStep(); stepIndex++)
    {
    vtkMedFieldStep* step = fieldOverEntity->GetStep(stepIndex);

    med_int ngp;
    med_float time;
    med_booleen local;
    med_int numo;
    med_int numdt;
    med_int nref;
    char meshName[MED_TAILLE_NOM + 1];
    meshName[MED_TAILLE_NOM] = '\0';

    if(MEDpasdetempsInfo(FileId, fieldName, fieldOverEntity->GetMedType(),
        fieldOverEntity->GetGeometry(), stepIndex + 1, &ngp, &numdt, &numo,
        step->GetTimeUnit()->GetWritePointer(), &time, meshName, &local, &nref)
        < 0)
      {
      vtkErrorMacro("While retrieving time step " << stepIndex+1
          << " for field " << field->GetName() )
      }
    else
      {
      if(vtkMath::IsNan(time))
        time = 0.0;
      step->SetTime(time);
      step->SetTimeId(numdt);
      step->SetIteration(numo);
      if(nref == 0)
        {
        step->AllocateNumberOfStepOnMesh(1);
        vtkMedFieldStepOnMesh* stepOnMesh = step->GetStepOnMesh(0);
        stepOnMesh->SetLocal(local);
        stepOnMesh->SetNumberOfQuadraturePoint(ngp);
        stepOnMesh->GetMeshName()->SetString(meshName);

        med_int nval = MEDnVal(this->FileId, fieldName,
            fieldOverEntity->GetMedType(), fieldOverEntity->GetGeometry(),
            numdt, numo, meshName, MED_COMPACT);
        if(nval < 0)
          {
          vtkErrorMacro("MEDnVal");
          }
        else
          {
          stepOnMesh->SetNumberOfValues(nval);
          }
        med_int nvalglobal = MEDnVal(this->FileId, fieldName,
            fieldOverEntity->GetMedType(), fieldOverEntity->GetGeometry(),
            numdt, numo, meshName, MED_GLOBAL);
        if(nvalglobal < 0)
          {
          vtkErrorMacro("MEDnVal");
          }
        else
          {
          stepOnMesh->SetHasProfile(nval != nvalglobal);
          }
        }
      else
        {
        step->AllocateNumberOfStepOnMesh(nref);
        for(int stepOnMeshId = 0; stepOnMeshId < nref; stepOnMeshId++)
          {
          vtkMedFieldStepOnMesh* stepOnMesh = step->GetStepOnMesh(stepOnMeshId);

          if(MEDchampRefInfo(this->FileId, fieldName,
              fieldOverEntity->GetMedType(), fieldOverEntity->GetGeometry(),
              stepOnMeshId + 1, numdt, numo,
              stepOnMesh->GetMeshName()->GetWritePointer(), &local, &ngp) < 0)
            {
            vtkErrorMacro("MEDchampRefInfo");
            }
          else
            {
            stepOnMesh->SetNumberOfQuadraturePoint(ngp);
            stepOnMesh->SetLocal(local);
            }
          char* meshName = stepOnMesh->GetMeshName()->GetString();
          med_int nval = MEDnVal(this->FileId, fieldName,
              fieldOverEntity->GetMedType(), fieldOverEntity->GetGeometry(),
              numdt, numo, meshName, MED_COMPACT);
          if(nval < 0)
            {
            vtkErrorMacro("MEDnVal");
            }
          else
            {
            stepOnMesh->SetNumberOfValues(nval);
            }
          med_int nvalglobal = MEDnVal(this->FileId, fieldName,
              fieldOverEntity->GetMedType(), fieldOverEntity->GetGeometry(),
              numdt, numo, meshName, MED_GLOBAL);
          if(nvalglobal < 0)
            {
            vtkErrorMacro("MEDnVal");
            }
          else
            {
            stepOnMesh->SetHasProfile(nval != nvalglobal);
            }
          }//stepOnMeshId
        }
      }//MEDpasdetempsInfo
    }//stepIndex

}

void vtkMedDriver::ReadMeta(vtkMedMesh* mesh)
{
  FileOpen open(this);

  med_int dimension;
  med_maillage type;

  if(MEDmaaInfo(this->FileId, mesh->GetMedIndex(),
      mesh->GetName()->GetWritePointer(), &dimension, &type,
      mesh->GetDescription()->GetWritePointer()))
    {
    vtkErrorMacro("Error reading mesh");
    }

  char* meshName = mesh->GetName()->GetString();

  if(MEDunvLire(this->FileId, meshName,
      mesh->GetUniversalName()->GetWritePointer()) < 0)
    {
    meshName, mesh->GetUniversalName()->SetString(NULL);
    }

  // read the meta data of all families.
  // writing the family 0 is optional, but I need it, so add it if it is not here.
  int nfam = MEDnFam(FileId, meshName);
  mesh->AllocateNumberOfFamily(nfam);
  vtkMedFamily* familyZero = NULL;
  for(int index = 0; index < nfam; index++)
    {
    vtkMedFamily* family = mesh->GetFamily(index);
    family->SetMedIndex(index + 1);
    this->ReadMeta(mesh, family);
    if(family->GetId() == 0)
      familyZero = family;
    }

  if(familyZero == NULL)
    {
    familyZero = vtkMedFamily::New();
    familyZero->SetId(0);
    familyZero->GetName()->SetString("FAMILY_ZERO");
    mesh->AppendFamily(familyZero);
    familyZero->Delete();
    }

  vtkMedFamily* familyZeroOnPoint = vtkMedFamily::New();
  mesh->AppendFamily(familyZeroOnPoint);
  familyZeroOnPoint->Delete();
  familyZeroOnPoint->ShallowCopy(familyZero);
  familyZeroOnPoint->SetPointOrCell(vtkMedUtilities::OnPoint);

  // Load meta regarding the grid type
  if(type == MED_STRUCTURE)
    {
    // Do it for structured data
    med_type_grille mtg;
    if(MEDnatureGrilleLire(FileId, meshName, &mtg) < 0)
      {
      vtkErrorMacro("Error during structured grid meta loading.");
      }
    vtkMedCartesianGrid* cartesian;
    vtkMedPolarGrid* polar;
    vtkMedStandardGrid* standard;

    switch(mtg)
      {
      case MED_GRILLE_CARTESIENNE:
        cartesian = vtkMedCartesianGrid::New();
        cartesian->SetDimension(dimension);
        mesh->SetGrid(cartesian);
        cartesian->Delete();
        this->ReadMeta(mesh, cartesian);
        break;
      case MED_GRILLE_POLAIRE:
        polar = vtkMedPolarGrid::New();
        polar->SetDimension(dimension);
        mesh->SetGrid(polar);
        polar->Delete();
        this->ReadMeta(mesh, polar);
        break;
      case MED_GRILLE_STANDARD:
        standard = vtkMedStandardGrid::New();
        standard->SetDimension(dimension);
        mesh->SetGrid(standard);
        standard->Delete();
        this->ReadMeta(mesh, standard);
        break;
      }
    }
  else //(mesh->GetType() == MED_STRUCTURE)
    {
    vtkMedUnstructuredGrid* grid = vtkMedUnstructuredGrid::New();
    mesh->SetGrid(grid);
    grid->Delete();
    grid->SetDimension(dimension);
    this->ReadMeta(mesh, grid);
    }

}

void vtkMedDriver::ReadMeta(vtkMedQuadratureDefinition* def)
{
  FileOpen open(this);

  char entityName[MED_TAILLE_NOM + 1];
  entityName[MED_TAILLE_NOM] = '\0';
  med_int ngp;
  med_geometrie_element type_geo;

  if(MEDgaussInfo(this->FileId, def->GetMedIndex(),
      def->GetName()->GetWritePointer(), &type_geo, &ngp) < 0)
    {
    vtkErrorMacro("Reading information on quadrature points definition : " << def->GetMedIndex());
    }

  def->SetNumberOfQuadraturePoint((int) ngp);
  def->SetGeometry(type_geo);
}

void vtkMedDriver::Load(vtkMedQuadratureDefinition* def)
{
  FileOpen open(this);

  if(def->IsLoaded())
    return;

  def->SecureResources();

  med_float* nodeLocalCoord = def->GetPointLocalCoordinates();
  med_float* qpLocalCoord = def->GetQuadraturePointLocalCoordinates();
  med_float* weights = def->GetWeights();

  if(MEDgaussLire(FileId, nodeLocalCoord, qpLocalCoord, weights,
      MED_FULL_INTERLACE, def->GetName()->GetString()) < 0)
    {
    vtkErrorMacro("Reading Quadrature Points values on : " << def->GetName());
    }

  def->BuildShapeFunction();

  //def->PrintSelf(cout, vtkIndent());
}

void vtkMedDriver::LoadFamilyIds(vtkMedMesh* mesh, vtkMedEntityArray* array)
{
  if(array->IsFamilyIdsLoaded())
    return;

  FileOpen open(this);

  vtkMedIntArray* famIds = vtkMedIntArray::New();
  array->SetFamilyIds(famIds);
  famIds->Delete();

  famIds->SetNumberOfTuples(array->GetNumberOfEntity());

  if(MEDfamLire(this->FileId, mesh->GetName()->GetString(), famIds->GetPointer(
      0), array->GetNumberOfEntity(), array->GetType(), array->GetGeometry())
      < 0)
    {
    vtkDebugMacro("Error loading the family ids of entity " << array->GetType() << " " << array->GetGeometry() << " on mesh " << mesh->GetName());
    array->SetFamilyIds(NULL);
    }

  array->ComputeFamilies(mesh);
}

void vtkMedDriver::LoadPointGlobalIds(vtkMedMesh* mesh)
{
  if(mesh->IsPointGlobalIdsLoaded())
    return;

  FileOpen open(this);

  vtkMedIntArray* globalIds = vtkMedIntArray::New();
  mesh->SetPointGlobalIds(globalIds);
  globalIds->Delete();

  globalIds->SetNumberOfTuples(mesh->GetGrid()->GetNumberOfPoints());
  if(MEDnumLire(this->FileId, (char *) mesh->GetName()->GetString(),
      globalIds->GetPointer(0), mesh->GetGrid()->GetNumberOfPoints(),
      MED_NOEUD, MED_NONE) < 0)
    {
    mesh->SetPointGlobalIds(NULL);
    }

  mesh->ComputePointFamilies();
}

void vtkMedDriver::LoadPointFamilyIds(vtkMedMesh* mesh)
{
  if(mesh->IsPointFamilyIdsLoaded())
    return;

  FileOpen open(this);

  vtkMedIntArray* famIds = vtkMedIntArray::New();
  mesh->SetPointFamilyIds(famIds);
  famIds->Delete();

  vtkIdType npts = mesh->GetGrid()->GetNumberOfPoints();

  famIds->SetNumberOfTuples(npts);
  if(MEDfamLire(this->FileId, mesh->GetName()->GetString(), famIds->GetPointer(
      0), npts, MED_NOEUD, MED_NONE) < 0)
    {
    vtkDebugMacro("Error loading the family ids on nodes  of mesh " << mesh->GetName());
    mesh->SetPointFamilyIds(NULL);
    }

  mesh->ComputePointFamilies();
}

void vtkMedDriver::LoadCoordinates(vtkMedMesh* mesh)
{
  vtkMedUnstructuredGrid* grid = vtkMedUnstructuredGrid::SafeDownCast(
      mesh->GetGrid());
  if(grid == NULL)
    {
    //TODO : deal with structured grids
    return;
    }

  if(grid->IsCoordinatesLoaded())
    return;

  FileOpen open(this);

  vtkDataArray* coords = vtkMedUtilities::NewCoordArray();
  grid->SetCoordinates(coords);
  coords->Delete();

  coords->SetNumberOfComponents(grid->GetDimension());
  coords->SetNumberOfTuples(grid->GetNumberOfPoints());
  med_repere type_rep;
  char* names = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];
  memset(names, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);
  char* units = new char[MED_TAILLE_PNOM * grid->GetDimension() + 1];
  memset(units, '\0', MED_TAILLE_PNOM * grid->GetDimension() + 1);

  if(MEDcoordLire(this->FileId, mesh->GetName()->GetString(),
      grid->GetDimension(), (med_float*) coords->GetVoidPointer(0),
      MED_FULL_INTERLACE, MED_ALL, NULL, 0, &type_rep, names, units) < 0)
    {
    vtkErrorMacro("Load Coordinates for mesh " << mesh->GetName()->GetString());
    }

  char name[MED_TAILLE_PNOM + 1];
  char unit[MED_TAILLE_PNOM + 1];
  for(int comp = 0; comp < grid->GetDimension(); comp++)
    {
    memset(unit, '\0', MED_TAILLE_PNOM + 1);
    memset(name, '\0', MED_TAILLE_PNOM + 1);
    memcpy(unit, units + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM * sizeof(char));
    memcpy(name, names + comp * MED_TAILLE_PNOM, MED_TAILLE_PNOM * sizeof(char));
    grid->GetComponentName(comp)->SetString(name);
    grid->GetUnit(comp)->SetString(unit);
    }

  grid->SetCoordinateSystem(type_rep);

  delete[] names;
  delete[] units;
}

void vtkMedDriver::Load(vtkMedProfile* profile)
{
  if(!profile || profile->IsLoaded())
    return;

  FileOpen open(this);

  vtkMedIntArray* indices = vtkMedIntArray::New();
  profile->SetIds(indices);
  indices->Delete();

  indices->SetNumberOfTuples(profile->GetNumberOfElement());

  if(MEDprofilLire(this->FileId, indices->GetPointer(0),
      profile->GetName()->GetWritePointer()) < 0)
    {
    vtkErrorMacro("Reading profile indices " << profile->GetName()->GetString());
    }
}

void vtkMedDriver::LoadConnectivity(vtkMedMesh* mesh, vtkMedEntityArray* array)
{
  if(array->IsConnectivityLoaded())
    return;

  FileOpen open(this);

  char* meshName = mesh->GetName()->GetString();

  vtkMedIntArray* conn = vtkMedIntArray::New();
  array->SetConnectivityArray(conn);
  conn->Delete();

  if(array->GetGeometry() == MED_POLYGONE)
    {
    med_int connSize;
    if(MEDpolygoneInfo(this->FileId, meshName, array->GetType(),
        array->GetConnectivity(), &connSize) < 0)
      {
      vtkErrorMacro("Error while reading informations on polygons");
      return;
      }

    conn->SetNumberOfTuples(connSize);

    vtkMedIntArray* index = vtkMedIntArray::New();
    array->SetIndex0(index);
    index->Delete();

    vtkIdType ni = array->GetNumberOfEntity() + 1;
    index->SetNumberOfTuples(ni);

    if(MEDpolygoneConnLire(this->FileId, meshName, index->GetPointer(0), ni,
        conn->GetPointer(0), array->GetType(), array->GetConnectivity()) < 0)
      {
      vtkErrorMacro(<< "Error while reading connectivity of polygons." << endl
          << "MeshName=" << meshName << endl
          << "Pointer=" << index->GetPointer(0) << endl
          << "NumberOfEntity=" << ni << endl
          << "MeshEntity=" << vtkMedUtilities::TypeName(array->GetType()) << endl
          << "Connectivity=" << vtkMedUtilities::ConnectivityName(array->GetConnectivity()));
      return;
      }

    }
  else if(array->GetGeometry() == MED_POLYEDRE)
    {
    med_int connSize;
    med_int nf;
    if(MEDpolyedreInfo(this->FileId, meshName, array->GetConnectivity(), &nf,
        &connSize) < 0)
      {
      vtkErrorMacro("Error while reading informations on polyhedrons");
      return;
      }

    vtkMedIntArray* index0 = vtkMedIntArray::New();
    array->SetIndex0(index0);
    index0->Delete();

    vtkMedIntArray* index1 = vtkMedIntArray::New();
    array->SetIndex1(index1);
    index1->Delete();

    vtkIdType np = array->GetNumberOfEntity() + 1;
    conn->SetNumberOfTuples(connSize);
    index0->SetNumberOfTuples(np);
    index1->SetNumberOfTuples(nf);

    if(MEDpolyedreConnLire(this->FileId, meshName, index0->GetPointer(0), np,
        index1->GetPointer(0), nf, conn->GetPointer(0),
        array->GetConnectivity()) < 0)
      {
      vtkErrorMacro("Error while reading connectivity of polyhedrons");
      return;
      }
    }
  else
    {
    if(array->GetConnectivity() == MED_NOD)
      {
      conn->SetNumberOfTuples(array->GetNumberOfEntity()
          * vtkMedUtilities::GetNumberOfPoint(array->GetGeometry()));
      }
    else
      {
      conn->SetNumberOfTuples(array->GetNumberOfEntity()
          * vtkMedUtilities::GetNumberOfSubEntity(array->GetGeometry()));
      }

    if(MEDconnLire(this->FileId, meshName, mesh->GetGrid()->GetDimension(),
        conn->GetPointer(0), MED_FULL_INTERLACE, NULL, MED_ALL,
        array->GetType(), array->GetGeometry(), array->GetConnectivity()) < 0)
      {
      vtkErrorMacro("Error while load connectivity of cells " << array->GetGeometry());
      }
    }
}

void vtkMedDriver::LoadGlobalIds(vtkMedMesh* mesh, vtkMedEntityArray* array)
{
  if(array->IsGlobalIdsLoaded())
    return;

  FileOpen open(this);

  vtkMedIntArray* globalIds = vtkMedIntArray::New();
  array->SetGlobalIds(globalIds);
  globalIds->Delete();

  globalIds->SetNumberOfTuples(array->GetNumberOfEntity());

  if(MEDnumLire(this->FileId, (char *) mesh->GetName()->GetString(),
      globalIds->GetPointer(0), array->GetNumberOfEntity(), array->GetType(),
      array->GetGeometry()) < 0)
    {
    array->SetGlobalIds(NULL);
    }
}

void vtkMedDriver::Load(vtkMedMesh* mesh, vtkMedField* field,
    vtkMedFieldOverEntity* foe, vtkMedFieldStep* step,
    vtkMedFieldStepOnMesh* som)
{
  if(som == NULL)
    return;

  if(som->IsLoaded())
    return;

  FileOpen open(this);

  vtkDataArray* data = vtkMedUtilities::NewArray(field->GetDataType());
  som->SetData(data);
  data->Delete();

  data->SetNumberOfComponents(field->GetNumberOfComponent());
  data->SetNumberOfTuples(som->GetNumberOfValues());

  if(MEDchampLire(this->FileId, mesh->GetName()->GetString(),
      field->GetName()->GetString(), (unsigned char*) data->GetVoidPointer(0),
      MED_FULL_INTERLACE, MED_ALL,
      som->GetQuadratureDefinitionName()->GetWritePointer(),
      som->GetProfileName()->GetWritePointer(), MED_COMPACT, foe->GetMedType(),
      foe->GetGeometry(), step->GetTimeId(), step->GetIteration()) < 0)
    {
    vtkErrorMacro("Error on MEDchampLire, FileId=" << this->FileId
        << ", MeshName=" << mesh->GetName()->GetString()
        << ", FieldName=" << field->GetName()->GetString()
        << ", RawPointer=" << (unsigned char*) data->GetVoidPointer(0)
        << ", MED_FULL_INTERLACE,  MED_ALL, QuadratureDefinitionName="
        << som->GetQuadratureDefinitionName()->GetWritePointer()
        << ", ProfileName=" << som->GetProfileName()->GetWritePointer()
        << ", MED_COMPACT, MeshEntity=" << vtkMedUtilities::TypeName(foe->GetMedType())
        << ", Geometry=" << vtkMedUtilities::GeometryName(foe->GetGeometry())
        << ", TimeId=" << step->GetTimeId() << ", Order=" << step->GetIteration());
    }
}

void vtkMedDriver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_STRING(os, indent, FileName);
  PRINT_IVAR(os, indent, OpenLevel);
  PRINT_IVAR(os, indent, FileId);
}
