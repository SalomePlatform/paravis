#ifndef __vtkMedFile_h_
#define __vtkMedFile_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"

class vtkMedString;
class vtkMedMesh;
class vtkMedField;
class vtkMedProfile;
class vtkMedLocalization;
class vtkMedLink;
class vtkMedDriver;

class VTK_EXPORT vtkMedFile: public vtkObject
{
public:
  static vtkMedFile* New();
  vtkTypeRevisionMacro(vtkMedFile, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the file name to read from
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // This is the description of this file as stored in the med file.
  vtkGetObjectMacro(MedDriver, vtkMedDriver);

  // Description:
  // This method tries to create a new driver for this file.
  // It returns 1 on success, 0 on failure.
  virtual int CreateDriver();

  // Description:
  // read information from this file, and create the meta data structure
  virtual void  ReadInformation();

  // Description:
  // Container of the meshes.
  vtkGetObjectVectorMacro(Mesh, vtkMedMesh);
  vtkSetObjectVectorMacro(Mesh, vtkMedMesh);
  virtual vtkMedMesh* GetMesh(vtkMedString*);

  // Description:
  // Container of the fields.
	vtkGetObjectVectorMacro(Field, vtkMedField);
	vtkSetObjectVectorMacro(Field, vtkMedField);

  // Description:
  // Container of the fields.
  vtkGetObjectVectorMacro(Link, vtkMedLink);
  vtkSetObjectVectorMacro(Link, vtkMedLink);

  // Description:
  // Container of the profiles.
	vtkGetObjectVectorMacro(Profile, vtkMedProfile);
	vtkSetObjectVectorMacro(Profile, vtkMedProfile);
	virtual vtkMedProfile*	GetProfile(vtkMedString*);

  // Description:
  // Container of the quadrature definitions.
  vtkGetObjectVectorMacro(Localization, vtkMedLocalization);
  vtkSetObjectVectorMacro(Localization, vtkMedLocalization);
  virtual vtkMedLocalization*	GetLocalization(vtkMedString*);

  // Description:
  // This is the description of this file as stored in the med file.
  vtkGetObjectMacro(Comment, vtkMedString);

  // Description:
  // Those 3 numbers describe the version of med used to create this file.
  vtkSetMacro(VersionMajor, int);
  vtkGetMacro(VersionMajor, int);
  vtkSetMacro(VersionMinor, int);
  vtkGetMacro(VersionMinor, int);
  vtkSetMacro(VersionRelease, int);
  vtkGetMacro(VersionRelease, int);

protected:
	vtkMedFile();
  virtual ~vtkMedFile();

  char * FileName;
  vtkMedDriver* MedDriver;
  virtual void  SetMedDriver(vtkMedDriver*);

  int VersionMajor;
  int VersionMinor;
  int VersionRelease;

  vtkMedString* Comment;
  //BTX
  vtkObjectVector<vtkMedMesh>* Mesh;
  vtkObjectVector<vtkMedField>* Field;
  vtkObjectVector<vtkMedProfile>* Profile;
  vtkObjectVector<vtkMedLocalization>* Localization;
  vtkObjectVector<vtkMedLink>* Link;
  //ETX

private:
	vtkMedFile(const vtkMedFile&); // Not implemented.
  void operator=(const vtkMedFile&); // Not implemented.
};

#endif //__vtkMedMetaData_h_
