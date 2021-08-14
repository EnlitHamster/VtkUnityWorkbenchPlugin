#include "vtkConeSourceAdapter.h"

#include <vtkConeSource.h>
#include <vtkAlgorithm.h>
#include <vtkMapper.h>
#include <vtkAlgorithmOutput.h>

#include "../VtkToUnityInternalHelpers.h"

#include <fstream>

const std::unordered_map<std::string, std::pair<vtkAdapter::getter<vtkConeSourceAdapter>, vtkAdapter::setter<vtkConeSourceAdapter>>>
	vtkConeSourceAdapter::s_attributes =
{
	{ "Height", std::make_pair(&GetHeight, &SetHeight) },
	{ "Radius", std::make_pair(&GetRadius, &SetRadius) },
	{ "Resolution",	std::make_pair(&GetResolution, &SetResolution) },
	{ "Angle", std::make_pair(&GetAngle, &SetAngle) },
	{ "Capping", std::make_pair(&GetCapping, &SetCapping) },
	{ "Center", std::make_pair(&GetCenter, &SetCenter) },
	{ "Direction", std::make_pair(&GetDirection, &SetDirection) }
};

vtkConeSourceAdapter::vtkConeSourceAdapter()
	: vtkAdapter("vtkConeSource") { }


vtkObjectBase* vtkConeSourceAdapter::NewInstance()
{
	return vtkConeSource::New();
}

// Source access based on https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ReverseAccess/

LPCSTR vtkConeSourceAdapter::GetAttribute(
	vtkObjectBase *object,
	LPCSTR propertyName)
{
	auto itAttribute = s_attributes.find(propertyName);

	if (itAttribute == s_attributes.end())
	{
		return "err::(Property not found)";
	}
	else
	{
		return (this->*itAttribute->second.first)(object).str().c_str();
	}
}

LPCSTR vtkConeSourceAdapter::GetDescriptor() const
{
	std::unordered_map<std::string, std::string> desc = {
		{ "Height", "double" },
		{ "Radius", "double" },
		{ "Resolution", "int" },
		{ "Capping", "int" },
		{ "Center", "double3" },
		{ "Direction", "double3" },
	};
	return MapDescriptorToCharArray(desc).str().c_str();
}

void vtkConeSourceAdapter::SetAttribute(
	vtkObjectBase *object,
	LPCSTR propertyName,
	LPCSTR newValue)
{
	auto itAttribute = s_attributes.find(propertyName);

	if (itAttribute != s_attributes.end())
	{
		(this->*itAttribute->second.second)(object, newValue);
	}
}

std::stringstream vtkConeSourceAdapter::GetHeight(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueDoubleToCharArray(coneSource->GetHeight());
}

void vtkConeSourceAdapter::SetHeight(
	vtkObjectBase *object, 
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetHeight(CharArrayToValueDouble(newValue));
}

std::stringstream vtkConeSourceAdapter::GetRadius(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueDoubleToCharArray(coneSource->GetRadius());
}

void vtkConeSourceAdapter::SetRadius(
	vtkObjectBase *object, 
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetRadius(CharArrayToValueDouble(newValue));
}

std::stringstream vtkConeSourceAdapter::GetResolution(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueIntToCharArray(coneSource->GetResolution());
}

void vtkConeSourceAdapter::SetResolution(
	vtkObjectBase *object,
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetResolution(CharArrayToValueInt(newValue));
}

std::stringstream vtkConeSourceAdapter::GetAngle(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueDoubleToCharArray(coneSource->GetAngle());
}

void vtkConeSourceAdapter::SetAngle(
	vtkObjectBase *object,
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetAngle(CharArrayToValueDouble(newValue));
}

std::stringstream vtkConeSourceAdapter::GetCapping(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueIntToCharArray(coneSource->GetCapping());
}

void vtkConeSourceAdapter::SetCapping(
	vtkObjectBase *object,
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetCapping(CharArrayToValueInt(newValue));
}

std::stringstream vtkConeSourceAdapter::GetCenter(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueVector3ToCharArray(coneSource->GetCenter());
}

void vtkConeSourceAdapter::SetCenter(
	vtkObjectBase *object,
	LPCSTR newValue)
{;
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetCenter(CharArrayToValueVector3(newValue));
}

std::stringstream vtkConeSourceAdapter::GetDirection(
	vtkObjectBase *object)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	return ValueVector3ToCharArray(coneSource->GetDirection());
}

void vtkConeSourceAdapter::SetDirection(
	vtkObjectBase *object,
	LPCSTR newValue)
{
	auto coneSource = dynamic_cast<vtkConeSource*>(object);
	coneSource->SetDirection(CharArrayToValueVector3(newValue));
}