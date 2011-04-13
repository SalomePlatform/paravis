#include "vtkMedFile.h"

#include "vtkObjectFactory.h"
#include "vtkDataArraySelection.h"
#include "vtkSmartPointer.h"

#include "vtkMedMesh.h"
#include "vtkMedField.h"
#include "vtkMedProfile.h"
#include "vtkMedLocalization.h"
#include "vtkMedUtilities.h"
#include "vtkMedLink.h"
#include "vtkMedDriver.h"
#include "vtkMedFactory.h"

vtkCxxGetObjectVectorMacro(vtkMedFile, Mesh, vtkMedMesh);
vtkCxxSetObjectVectorMacro(vtkMedFile, Mesh, vtkMedMesh);

vtkCxxGetObjectVectorMacro(vtkMedFile, Field, vtkMedField);
vtkCxxSetObjectVectorMacro(vtkMedFile, Field, vtkMedField);

vtkCxxGetObjectVectorMacro(vtkMedFile, Profile, vtkMedProfile);
vtkCxxSetObjectVectorMacro(vtkMedFile, Profile, vtkMedProfile);

vtkCxxGetObjectVectorMacro(vtkMedFile, Localization, vtkMedLocalization);
vtkCxxSetObjectVectorMacro(vtkMedFile, Localization, vtkMedLocalization);

vtkCxxGetObjectVectorMacro(vtkMedFile, Link, vtkMedLink);
vtkCxxSetObjectVectorMacro(vtkMedFile, Link, vtkMedLink);

vtkCxxSetObjectMacro(vtkMedFile, MedDriver, vtkMedDriver);

vtkCxxRevisionMacro(vtkMedFile, "$Revision$")
vtkStandardNewMacro(vtkMedFile)

vtkMedFile::vtkMedFile()
{
  this->Comment = NULL;
  this->Mesh = new vtkObjectVector<vtkMedMesh> ();
  this->Field = new vtkObjectVector<vtkMedField> ();
  this->Profile = new vtkObjectVector<vtkMedProfile> ();
  this->Localization = new vtkObjectVector<vtkMedLocalization> ();
  this->Link = new vtkObjectVector<vtkMedLink> ();
  this->FileName = NULL;
  this->MedDriver = NULL;
  this->VersionMajor = -1;
  this->VersionMinor = -1;
  this->VersionRelease = -1;
}

vtkMedFile::~vtkMedFile()
{
  this->SetComment(NULL);
  delete this->Mesh;
  delete this->Field;
  delete this->Profile;
  delete this->Localization;
  delete this->Link;
  this->SetFileName(NULL);
  this->SetMedDriver(NULL);
}

int vtkMedFile::CreateDriver()
{
  int major, minor, release;
  vtkMedDriver* driver=vtkMedDriver::New();
  driver->SetMedFile(this);
  bool canRead=driver->CanReadFile();
  if(!canRead)
    {
    driver->Delete();
    this->SetMedDriver(NULL);
    return 0;
    }
  driver->ReadFileVersion(&major, &minor, &release);
  driver->Delete();
  vtkMedFactory* factory=vtkMedFactory::New();
  driver=factory->NewMedDriver(major, minor, release);
  factory->Delete();
  this->SetMedDriver(driver);
  if (driver)
    {
    driver->SetMedFile(this);
    return 1;
    }
  return 0;
}

void  vtkMedFile::ReadInformation()
{
  if(this->MedDriver == NULL)
    {
    if(!this->CreateDriver())
      return;
    }

  // at this point, we know that we have a valid driver.
  this->MedDriver->ReadFileInformation(this);
}

vtkMedMesh* vtkMedFile::GetMesh(const char* str)
{
  for (int m = 0; m < this->Mesh->size(); m++)
    {
    vtkMedMesh* mesh = this->Mesh->at(m);
    if (strcmp(mesh->GetName(), str) == 0)
      {
      return mesh;
      }
    }
  return NULL;
}

vtkMedProfile* vtkMedFile::GetProfile(const char* str)
{
  for (int profId = 0; profId < this->Profile->size(); profId++)
    {
    vtkMedProfile* profile = this->Profile->at(profId);
    if (strcmp(profile->GetName(), str) == 0)
      {
      return profile;
      }
    }
  return NULL;

}

vtkMedLocalization* vtkMedFile::GetLocalization(const char* str)
{
  for (int quadId = 0; quadId < this->Localization->size(); quadId++)
    {
    vtkMedLocalization* loc = this->Localization->at(quadId);
    if (strcmp(loc->GetName(), str) == 0)
      {
      return loc;
      }
    }
  return NULL;
}

void vtkMedFile::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  PRINT_OBJECT_VECTOR(os, indent, Mesh);
  PRINT_OBJECT_VECTOR(os, indent, Field);
  PRINT_OBJECT_VECTOR(os, indent, Profile);
  PRINT_OBJECT_VECTOR(os, indent, Localization);
  PRINT_OBJECT_VECTOR(os, indent, Link);
}
