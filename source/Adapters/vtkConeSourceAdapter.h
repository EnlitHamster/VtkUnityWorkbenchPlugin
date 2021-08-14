#pragma once

#include "vtkAdapter.h"

#include <vtkSmartPointer.h>
#include <vtkObjectBase.h>
#include <vtkObjectBase.h>
#include <unordered_map>

#include <string>

// TODO: create an object adapter for the object operations, and extend that instead
class vtkConeSourceAdapter : public vtkAdapter
{
public:
	vtkConeSourceAdapter();
	virtual ~vtkConeSourceAdapter() { }

	virtual void SetAttribute(
		vtkObjectBase *object, 
		LPCSTR propertyName, 
		LPCSTR newValue);
	
	virtual LPCSTR GetAttribute(
		vtkObjectBase *object, 
		LPCSTR propertyName);

	virtual LPCSTR GetDescriptor() const;

	virtual vtkObjectBase* NewInstance();

protected:
	// Mapping the sources attributes to the specific getter and setter methods
	static const std::unordered_map<std::string, std::pair<getter<vtkConeSourceAdapter>, setter<vtkConeSourceAdapter>>> s_attributes;

	virtual std::stringstream GetHeight(vtkObjectBase *object);
	virtual void SetHeight(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetRadius(vtkObjectBase *object);
	virtual void SetRadius(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetResolution(vtkObjectBase *object);
	virtual void SetResolution(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetAngle(vtkObjectBase *object);
	virtual void SetAngle(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetCapping(vtkObjectBase *object);
	virtual void SetCapping(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetCenter(vtkObjectBase *object);
	virtual void SetCenter(vtkObjectBase *object, LPCSTR newValue);

	virtual std::stringstream GetDirection(vtkObjectBase *object);
	virtual void SetDirection(vtkObjectBase *object, LPCSTR newValue);
};