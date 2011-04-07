#include "vtkMedDriver.h"

#include "vtkObjectFactory.h"
#include "vtkStringArray.h"
#include "vtkDataArray.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"

#include "vtkMedFile.h"
#include "vtkMedCartesianGrid.h"
#include "vtkMedPolarGrid.h"
#include "vtkMedCurvilinearGrid.h"
#include "vtkMedUnstructuredGrid.h"
#include "vtkMedField.h"
#include "vtkMedMesh.h"
#include "vtkMedFamily.h"
#include "vtkMedUtilities.h"
#include "vtkMedEntityArray.h"
#include "vtkMedLocalization.h"
#include "vtkMedProfile.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedFieldStep.h"
#include "vtkMedGroup.h"
#include "vtkMedString.h"
#include "vtkMedIntArray.h"
#include "vtkMedLink.h"

vtkCxxSetObjectMacro(vtkMedDriver, MedFile, vtkMedFile);

vtkCxxRevisionMacro(vtkMedDriver, "$Revision$")
vtkStandardNewMacro(vtkMedDriver)

vtkMedDriver::vtkMedDriver()
{
  this->MedFile = NULL;
  this->OpenLevel = 0;
  this->FileId = -1;
}

vtkMedDriver::~vtkMedDriver()
{
  if (this->OpenLevel > 0)
    {
    vtkWarningMacro("The file has not be closed before destructor.")
    this->OpenLevel = 1;
    this->Close();
    }
  this->SetMedFile(NULL);
}

int vtkMedDriver::Open()
{
  int res = 0;
  if (this->MedFile == NULL || this->MedFile->GetFileName() == NULL)
    {
    vtkDebugMacro("Error : FileName has not been set ");
    return -1;
    }

  if (this->OpenLevel <= 0)
    {

    med_bool hdfok;
    med_bool medok;

    med_err conforme = MEDfileCompatibility(this->MedFile->GetFileName(),
                                            &hdfok, &medok);
    if (!hdfok)
      {
      vtkErrorMacro("The file " << this->MedFile->GetFileName()
          << " is not a HDF5 file, aborting.");
      return -1;
      }

    if (!medok)
      {
      vtkErrorMacro("The file " << this->MedFile->GetFileName()
          << " has not been written with the"
          << " same version as the one currently used to read it, this may lead"
          << " to errors. Please use the medimport tool.");
      return -1;
      }

    if(conforme < 0)
      {
      vtkErrorMacro("The file " << this->MedFile->GetFileName()
                    << " is not compatible, please import it to the new version using medimport.");
      return -1;
      }

    // the following line should be
    //this->FileId = MEDfileOpen(this->MedFile->GetFileName(), MED_ACC_RDONLY);
    // but there is a bug in med that forces me to use RDWR mode instead.

    this->FileId = MEDfileOpen(this->MedFile->GetFileName(), MED_ACC_RDONLY);
    if (this->FileId < 0)
      {
      vtkDebugMacro("Error : unable to open file "
                    << this->MedFile->GetFileName());
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
  if (this->OpenLevel == 0)
    {
    if (MEDfileClose(this->FileId) < 0)
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
  if (MEDfileNumVersionRd(this->FileId, &amajor, &aminor, &arelease) < 0)
    {
    vtkErrorMacro("Impossible to read the version of this file");
    return;
    }
  *major = amajor;
  *minor = aminor;
  *release = arelease;
}

void vtkMedDriver::ReadRegularGridInformation(vtkMedRegularGrid* grid)
{
  vtkErrorMacro("vtkMedDriver::ReadInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadCurvilinearGridInformation(vtkMedCurvilinearGrid* grid)
{
  vtkErrorMacro("vtkMedDriver::ReadInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadUnstructuredGridInformation(vtkMedUnstructuredGrid* grid)
{
  vtkErrorMacro("vtkMedDriver::ReadInformation not Implemented !");
  return;
}

// Description:
// load all Information data associated with this standard grid.
void vtkMedDriver::ReadGridInformation(vtkMedGrid* grid)
{
  if(vtkMedRegularGrid::SafeDownCast(grid) != NULL)
    {
    this->ReadRegularGridInformation(vtkMedRegularGrid::SafeDownCast(grid));
    }
  if(vtkMedCurvilinearGrid::SafeDownCast(grid) != NULL)
    {
    this->ReadCurvilinearGridInformation(vtkMedCurvilinearGrid::SafeDownCast(grid));
    }
  if(vtkMedUnstructuredGrid::SafeDownCast(grid) != NULL)
    {
    this->ReadUnstructuredGridInformation(vtkMedUnstructuredGrid::SafeDownCast(grid));
    }
}

void vtkMedDriver::ReadFamilyInformation(vtkMedMesh* mesh, vtkMedFamily* family)
{
  vtkErrorMacro("vtkMedDriver::ReadFamilyInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadFileInformation(vtkMedFile* file)
{
  vtkErrorMacro("vtkMedDriver::ReadFileInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadProfileInformation(vtkMedProfile* profile)
{
  vtkErrorMacro("vtkMedDriver::ReadProfileInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadFieldInformation(vtkMedField* field)
{
  vtkErrorMacro("vtkMedDriver::ReadFieldInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadFieldOverEntityInformation(vtkMedFieldOverEntity* fieldOverEntity)
{
  vtkErrorMacro("vtkMedDriver::ReadFieldOverEntityInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadMeshInformation(vtkMedMesh* mesh)
{
  vtkErrorMacro("vtkMedDriver::ReadMeshInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadLocalizationInformation(vtkMedLocalization* loc)
{
  vtkErrorMacro("vtkMedDriver::ReadLocalizationInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadInterpolationInformation(vtkMedInterpolation* interp)
{
  vtkErrorMacro("vtkMedDriver::ReadInterpolationInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadFieldStepInformation(vtkMedFieldStep* step)
{
  vtkErrorMacro("vtkMedDriver::ReadFieldStepInformation not Implemented !");
  return;
}

void vtkMedDriver::ReadFieldOnProfileInformation(vtkMedFieldOnProfile* fop)
{
  vtkErrorMacro("vtkMedDriver::ReadFieldOnProfileInformation not Implemented !");
  return;
}

void vtkMedDriver::LoadFamilyIds(vtkMedEntityArray* array)
{
  vtkErrorMacro("vtkMedDriver::LoadFamilyIds not Implemented !");
  return;
}

void vtkMedDriver::LoadPointGlobalIds(vtkMedGrid* grid)
{
  vtkErrorMacro("vtkMedDriver::LoadPointGlobalIds not Implemented !");
  return;
}

void vtkMedDriver::LoadCoordinates(vtkMedGrid* grid)
{
  vtkErrorMacro("vtkMedDriver::LoadCoordinates not Implemented !");
  return;
}

void vtkMedDriver::LoadProfile(vtkMedProfile* profile)
{
  vtkErrorMacro("vtkMedDriver::LoadProfile not Implemented !");
  return;
}

void vtkMedDriver::LoadConnectivity(vtkMedEntityArray* array)
{
  vtkErrorMacro("vtkMedDriver::LoadConnectivity not Implemented !");
  return;
}

void vtkMedDriver::LoadCellGlobalIds(vtkMedEntityArray* array)
{
  vtkErrorMacro("vtkMedDriver::LoadGlobalIds not Implemented !");
  return;
}

void vtkMedDriver::LoadField(vtkMedFieldOnProfile* foe)
{
  vtkErrorMacro("vtkMedDriver::LoadFieldOnProfile not Implemented !");
  return;
}

void vtkMedDriver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, OpenLevel);
  PRINT_IVAR(os, indent, FileId);
}
