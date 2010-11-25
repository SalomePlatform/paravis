#include "vtkMedString.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"

vtkCxxRevisionMacro(vtkMedString, "$Revision$")
vtkStandardNewMacro(vtkMedString)

vtkMedString::vtkMedString()
{
	this->String = NULL;
	this->Size = 0;
}

void vtkMedString::SetSize(int size)
{
	if (size == this->Size)
		return;

	if (this->String != NULL)
		{
		delete[] this->String;
		this->String = NULL;
		}

	this->Size = size;
	if (this->Size >= 0)
		{
		this->String = new char[this->Size + 1];
		memset(this->String, '\0', this->Size + 1);
		}
	this->Modified();
}

vtkMedString::~vtkMedString()
{
	if (this->String)
		delete[] this->String;
}

void vtkMedString::SetString(const char* buf)
{
	if(this->String == NULL)
		{
		if(buf == NULL)
			return;
		vtkWarningMacro("Allocate space for the string first!");
		return;
		}

	if (buf == NULL || buf[0] == '\0')
		{
		if (this->String[0] == '\0')
			return;
		memset(this->String, '\0', this->Size + 1);
		this->Modified();
		return;
		}

	if (strlen(buf) > this->Size)
		{
		char tmp[this->Size + 1];
		tmp[this->Size] = '\0';
		for (int i = 0; i < this->Size; i++)
			tmp[i] = buf[i];
		this->SetString(tmp);
		}

	if (strcmp(this->String, buf) == 0)
		return;

	strcpy(this->String, buf);
	this->Modified();
}

void vtkMedString::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Size);
	os << indent << "String : " << this->String << endl;
}
