#include "vtkMedSelection.h"

#include "vtkObjectFactory.h"
#include "vtkStringArray.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

class vtkMedSelectionInternals
{
public :
	map< string, bool > Status;
	vector< string > Keys;
};

vtkCxxRevisionMacro(vtkMedSelection, "$Revision$")
vtkStandardNewMacro(vtkMedSelection)

vtkMedSelection::vtkMedSelection()
{
	this->Internals = new vtkMedSelectionInternals();
}

vtkMedSelection::~vtkMedSelection()
{
	delete this->Internals;
}

void  vtkMedSelection::Initialize()
{
	this->Internals->Status.clear();
	this->Internals->Keys.clear();
}

void  vtkMedSelection::AddKey(const char* key)
{
	if(this->Internals->Status.find(key) != this->Internals->Status.end())
		{
		return ;
		}
	this->Internals->Keys.push_back(key);
	this->Internals->Status[key] = 1;
}

void  vtkMedSelection::SetKeyStatus(const char* key, int status)
{
	if(this->Internals->Status.find(key) == this->Internals->Status.end())
		{
		this->AddKey(key);
		}
	this->Internals->Status[key] = status;
}

int vtkMedSelection::GetKeyStatus(const char* key)
{
	if(this->Internals->Status.find(key) == this->Internals->Status.end())
		{
		return 0;
		}
	return this->Internals->Status[key];
}

const char* vtkMedSelection::GetKey(int index)
{
	if(index < 0  || index >= this->Internals->Keys.size())
		{
		return NULL;
		}
	return this->Internals->Keys[index].c_str();
}

int vtkMedSelection::vtkMedSelection::GetNumberOfKey()
{
	return this->Internals->Keys.size();
}

int vtkMedSelection::KeyExists(const char *key)
{
	return this->Internals->Status.find(key)
			!= this->Internals->Status.end();
}

void  vtkMedSelection::RemoveKeyByIndex(int index)
{
	if(index < 0  || index >= this->Internals->Keys.size())
		{
		return;
		}
	string name = this->Internals->Keys[index];
	this->Internals->Status.erase(name);
	this->Internals->Keys.erase(this->Internals->Keys.begin() + index);
}

void vtkMedSelection::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
