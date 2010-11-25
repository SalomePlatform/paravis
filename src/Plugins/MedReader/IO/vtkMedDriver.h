#ifndef __vtkMedDriver_h_
#define __vtkMedDriver_h_

#include "vtkObject.h"
#include "vtkMed.h"

class vtkMedFile;
class vtkMedMesh;
class vtkMedField;
class vtkMedFamily;
class vtkMedRegularGrid;
class vtkMedStandardGrid;
class vtkMedUnstructuredGrid;
class vtkMedQuadratureDefinition;
class vtkMedProfile;
class vtkMedFieldOverEntity;
class vtkMedEntityArray;
class vtkMedFieldStep;
class vtkMedFieldStepOnMesh;

class VTK_EXPORT vtkMedDriver: public vtkObject
{
public:
	static vtkMedDriver* New();
vtkTypeRevisionMacro(vtkMedDriver, vtkObject)
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Set the file name to read from
	vtkSetStringMacro(FileName)
	vtkGetStringMacro(FileName)

	// Description:
	// open the file for reading. Returns 0 on success, or error code.
	virtual int Open();
	virtual void Close();
	virtual bool CanReadFile();

	// Description:
	// Get the Version numbers from the file
	virtual void ReadFileVersion(int* major, int* minor, int* release);

	// Description:
	// load all meta data associated with this file.
	virtual void ReadMeta(vtkMedFile*);

	// Description:
	// load all meta data associated with this mesh.
	virtual void ReadMeta(vtkMedMesh*);

	// Description:
	// load all meta data associated with this field.
	virtual void ReadMeta(vtkMedField*);

	// Description:
	// load all meta data associated with this regular (Cartesian or polar) grid.
	virtual void ReadMeta(vtkMedMesh*, vtkMedRegularGrid*);

	// Description:
	// load all meta data associated with this standard grid.
	virtual void ReadMeta(vtkMedMesh*, vtkMedStandardGrid*);

	// Description:
	// load all meta data associated with this unstructured grid.
	virtual void ReadMeta(vtkMedMesh*, vtkMedUnstructuredGrid*);

	// Description:
	// load all meta data associated with this family.
	virtual void ReadMeta(vtkMedMesh*, vtkMedFamily*);

	// Description:
	// load all meta data associated with this family.
	virtual void ReadMeta(vtkMedProfile*);

	// Description:
	// load the given information on a field and a given cell type
	virtual void ReadMeta(vtkMedField*, vtkMedFieldOverEntity*);

	// Description:
	// load the given quadrature scheme definition
	virtual void ReadMeta(vtkMedQuadratureDefinition* def);

	// Description:
	// load the given quadrature scheme definition
	virtual void Load(vtkMedQuadratureDefinition* def);

	// Description:
	// Load the indices of the profile
	virtual void Load(vtkMedProfile* profile);

	// Description:
	// Load the values of the given step
	virtual void Load(vtkMedMesh*, vtkMedField*, vtkMedFieldOverEntity*, vtkMedFieldStep*, vtkMedFieldStepOnMesh*);

	virtual void LoadFamilyIds(vtkMedMesh*, vtkMedEntityArray*);

	virtual void LoadCoordinates(vtkMedMesh*);

	virtual void LoadPointGlobalIds(vtkMedMesh*);

	virtual void LoadPointFamilyIds(vtkMedMesh*);

	virtual void LoadConnectivity(vtkMedMesh* mesh,
																vtkMedEntityArray*);

	virtual void LoadGlobalIds(	vtkMedMesh* mesh,
															vtkMedEntityArray*);

	//BTX
	class FileOpen
	{
	public:
	  FileOpen(vtkMedDriver* driver)
	  {
	    this->Driver = driver;
	    this->Driver->Open();
	  }
	  ~FileOpen()
	  {
	    this->Driver->Close();
	  }
	protected:
	  vtkMedDriver* Driver;
	};
	//ETX

protected:
	vtkMedDriver();
	virtual ~vtkMedDriver();

	// name of the file to read from
	char* FileName;

	int OpenLevel;

	med_idt FileId;

private:
	vtkMedDriver(const vtkMedDriver&); // Not implemented.
	void operator=(const vtkMedDriver&); // Not implemented.
};

#endif //__vtkMedDriver_h_
