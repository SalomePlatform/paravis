#ifndef __vtkMedString_h_
#define __vtkMedString_h_

#include "vtkObject.h"

class vtkMedString : public vtkObject
{
public:
  static vtkMedString* New();
  vtkTypeRevisionMacro(vtkMedString, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the content of the string to the given buffer.
  // If the given string is longer than the internal length, clamp it.
	void SetString(const char*);

  // Description:
	// returns the internal pointer
	char* GetString() const
	{
		return this->String;
	}

  // Description:
	// returns the internal pointer for write.
	// The internal buffer must have been previously allocated with SetSize.
	char* GetWritePointer()
	{
		return this->String;
	}

  // Description:
	// allocates storage for the given size. Add 1 for the final '\0' character
	virtual void	SetSize(int);
protected :
	vtkMedString();
	~vtkMedString();

	char* String;
	int Size;
};

#endif //__vtkMedString_h_
