#ifndef __vtkMedFile_h_
#define __vtkMedFile_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"

class vtkMedString;
class vtkMedMesh;
class vtkMedField;
class vtkMedProfile;
class vtkMedQuadratureDefinition;

class VTK_EXPORT vtkMedFile: public vtkObject
{
public:
  static vtkMedFile* New();
  vtkTypeRevisionMacro(vtkMedFile, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

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
  // Container of the profiles.
	vtkGetObjectVectorMacro(Profile, vtkMedProfile);
	vtkSetObjectVectorMacro(Profile, vtkMedProfile);
	virtual vtkMedProfile*	GetProfile(vtkMedString*);

  // Description:
  // Container of the quadrature definitions.
	vtkGetObjectVectorMacro(QuadratureDefinition, vtkMedQuadratureDefinition);
	vtkSetObjectVectorMacro(QuadratureDefinition, vtkMedQuadratureDefinition);
	virtual vtkMedQuadratureDefinition*	GetQuadratureDefinition(vtkMedString*);

  // Description:
  // This is the description of this file as stored in the med file.
	vtkGetObjectMacro(Description, vtkMedString);


protected:
	vtkMedFile();
  virtual ~vtkMedFile();

  vtkMedString* Description;
  //BTX
  vtkObjectVector<vtkMedMesh>* Mesh;
  vtkObjectVector<vtkMedField>* Field;
  vtkObjectVector<vtkMedProfile>* Profile;
  vtkObjectVector<vtkMedQuadratureDefinition>* QuadratureDefinition;
  //ETX


private:
	vtkMedFile(const vtkMedFile&); // Not implemented.
  void operator=(const vtkMedFile&); // Not implemented.
};

#endif //__vtkMedMetaData_h_
