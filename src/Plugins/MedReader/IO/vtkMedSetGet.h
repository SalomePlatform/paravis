#ifndef _vtkMedSetGet_h_
#define _vtkMedSetGet_h_

//BTX
template <class T> class vtkObjectVector;
template <class T> class vtkList;

#define vtkGetObjectVectorMacro(name, type)\
	virtual type* Get##name (int index);\
	virtual vtkIdType GetNumberOf##name ();

#define vtkSetObjectVectorMacro(name, type)\
	virtual void	AllocateNumberOf##name (vtkIdType size);\
	virtual void Set##name (vtkIdType index, type* obj);\
	virtual void	Append##name (type* obj);

#define vtkSetAbstractObjectVectorMacro(name, type)\
	virtual void SetNumberOf##name (vtkIdType size);\
	virtual void Set##name (vtkIdType index, type* obj);\
	virtual void Append##name (type* obj);

#define vtkCxxGetObjectVectorMacro(class, name, type)\
	type* class::Get##name (int index)\
	{\
		if(index < 0 || index >= this->name->size())\
			return NULL;\
		return this->name->at(index);\
	}\
	vtkIdType class::GetNumberOf##name ()\
	{\
		return this->name->size();\
	}

#define vtkCxxSetObjectVectorMacro(class, name, type)\
	void	class::AllocateNumberOf##name (vtkIdType size)\
	{\
		if(this->name->size() == size)\
			return;\
		if(size <= 0 )\
			this->name->clear();\
		else\
			this->name->resize(size);\
		for(vtkIdType _ii=0; _ii<this->name->size(); _ii++)\
			{\
				this->name->at(_ii) = vtkSmartPointer< type > ::New();\
			}\
		this->Modified();\
	}\
	void class::Set##name (vtkIdType index, type* obj)\
	{\
		if(index < 0 || index >= this->name->size())\
		{\
			vtkWarningMacro("has not been allocated before setting value, please call AllocateNumberOf##name before" );\
			return;\
		}\
		if( this->name->at(index) == obj)\
			return;\
		this->name->at(index) = obj; \
		this->Modified();\
	}\
	void	class::Append##name (type* obj)\
	{\
		this->name->resize(this->name->size()+1);\
		this->name->at(this->name->size()-1) = obj;\
		this->Modified();\
	}

#define vtkCxxSetAbstractObjectVectorMacro(class, name, type)\
	void	class::SetNumberOf##name (vtkIdType size)\
	{\
		if(this->name->size() == size)\
			return;\
		if(size <= 0 )\
			this->name->clear();\
		else\
			this->name->resize(size);\
		this->Modified();\
	}\
	void class::Set##name (vtkIdType index, type* obj)\
	{\
		if(index < 0 || index >= this->name->size())\
		{\
			vtkWarningMacro("has not been allocated before setting value, please call AllocateNumberOf##name before" );\
			return;\
		}\
		if( this->name->at(index) == obj)\
			return;\
		this->name->at(index) = obj; \
		this->Modified();\
	}\
	void	class::Append##name (type* obj)\
	{\
		this->name->resize(this->name->size()+1);\
		this->name->at(this->name->size()-1) = obj;\
		this->Modified();\
	}

//ETX

#endif
