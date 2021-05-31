#pragma once

#include "../vtkAdapter.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <unordered_map>

#include <string>

// TODO: create an actor adapter for the actor operations, and extend that instead
class VtkConeSourceAdapter : public VtkAdapter
{
public:
	VtkConeSourceAdapter();
	virtual ~VtkConeSourceAdapter() { }

	virtual void SetAttribute(
		vtkSmartPointer<vtkActor> actor, 
		LPCSTR propertyName, 
		LPCSTR newValue);
	
	virtual void GetAttribute(
		vtkSmartPointer<vtkActor> actor, 
		LPCSTR propertyName, 
		char* retValue);

	virtual void GetDescriptor(
		char* retValue) const;

protected:
	// Mapping the sources attributes to the specific getter and setter methods
	static const std::unordered_map<std::string, std::pair<getter<VtkConeSourceAdapter>, setter<VtkConeSourceAdapter>>> s_attributes;

	virtual std::stringstream GetHeight(vtkSmartPointer<vtkActor> actor);
	virtual void SetHeight(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetRadius(vtkSmartPointer<vtkActor> actor);
	virtual void SetRadius(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetResolution(vtkSmartPointer<vtkActor> actor);
	virtual void SetResolution(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetAngle(vtkSmartPointer<vtkActor> actor);
	virtual void SetAngle(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetCapping(vtkSmartPointer<vtkActor> actor);
	virtual void SetCapping(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetCenter(vtkSmartPointer<vtkActor> actor);
	virtual void SetCenter(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);

	virtual std::stringstream GetDirection(vtkSmartPointer<vtkActor> actor);
	virtual void SetDirection(vtkSmartPointer<vtkActor> actor, LPCSTR newValue);
};