#include "vtkConeSourceAdapter.h"

#include <vtkConeSource.h>
#include <vtkAlgorithm.h>
#include <vtkMapper.h>
#include <vtkAlgorithmOutput.h>

#include "../VtkToUnityInternalHelpers.h"

#include <fstream>

const std::unordered_map<std::string, std::pair<VtkAdapter::getter<VtkConeSourceAdapter>, VtkAdapter::setter<VtkConeSourceAdapter>>>
	VtkConeSourceAdapter::s_attributes =
{
	{ "Height", std::make_pair(&GetHeight, &SetHeight) },
	{ "Radius", std::make_pair(&GetRadius, &SetRadius) },
	{ "Resolution",	std::make_pair(&GetResolution, &SetResolution) },
	{ "Angle", std::make_pair(&GetAngle, &SetAngle) },
	{ "Capping", std::make_pair(&GetCapping, &SetCapping) },
	{ "Center", std::make_pair(&GetCenter, &SetCenter) },
	{ "Direction", std::make_pair(&GetDirection, &SetDirection) }
};

VtkConeSourceAdapter::VtkConeSourceAdapter()
	: VtkAdapter("vtkConeSource") { }


vtkObjectBase* VtkConeSourceAdapter::NewInstance()
{
	return vtkConeSource::New();
}

// Source access based on https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ReverseAccess/

void VtkConeSourceAdapter::GetAttribute(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR propertyName,
	char* retValue)
{
	auto algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm);
	auto itAttribute = s_attributes.find(propertyName);

	if (itAttribute == s_attributes.end())
	{
		retValue = "err::(Property not found)";
	}
	else
	{
		(this->*itAttribute->second.first)(actor) >> retValue;
	}
}

void VtkConeSourceAdapter::GetDescriptor(
	char* retValue) const
{
	std::unordered_map<std::string, std::string> desc = {
		{ "Height", "double" },
		{ "Radius", "double" },
		{ "Resolution", "int" },
		{ "Capping", "int" },
		{ "Center", "double3" },
		{ "Direction", "double3" },
	};
	MapDescriptorToCharArray(desc) >> retValue;
}

void VtkConeSourceAdapter::SetAttribute(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR propertyName,
	LPCSTR newValue)
{
	auto algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm);

	auto itAttribute = s_attributes.find(propertyName);

	if (itAttribute != s_attributes.end())
	{
		(this->*itAttribute->second.second)(actor, newValue);
	}
}

std::stringstream VtkConeSourceAdapter::GetHeight(
	vtkSmartPointer<vtkActor> actor)
{
	// Based on https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ReverseAccess/
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueDoubleToCharArray(coneSource->GetHeight());
}

void VtkConeSourceAdapter::SetHeight(
	vtkSmartPointer<vtkActor> actor, 
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetHeight(CharArrayToValueDouble(newValue));
}

std::stringstream VtkConeSourceAdapter::GetRadius(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueDoubleToCharArray(coneSource->GetRadius());
}

void VtkConeSourceAdapter::SetRadius(
	vtkSmartPointer<vtkActor> actor, 
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetRadius(CharArrayToValueDouble(newValue));
}

std::stringstream VtkConeSourceAdapter::GetResolution(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueIntToCharArray(coneSource->GetResolution());
}

void VtkConeSourceAdapter::SetResolution(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetResolution(CharArrayToValueInt(newValue));
}

std::stringstream VtkConeSourceAdapter::GetAngle(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueDoubleToCharArray(coneSource->GetAngle());
}

void VtkConeSourceAdapter::SetAngle(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetAngle(CharArrayToValueDouble(newValue));
}

std::stringstream VtkConeSourceAdapter::GetCapping(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueIntToCharArray(coneSource->GetCapping());
}

void VtkConeSourceAdapter::SetCapping(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetCapping(CharArrayToValueInt(newValue));
}

std::stringstream VtkConeSourceAdapter::GetCenter(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueVector3ToCharArray(coneSource->GetCenter());
}

void VtkConeSourceAdapter::SetCenter(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetCenter(CharArrayToValueVector3(newValue));
}

std::stringstream VtkConeSourceAdapter::GetDirection(
	vtkSmartPointer<vtkActor> actor)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	return ValueVector3ToCharArray(coneSource->GetDirection());
}

void VtkConeSourceAdapter::SetDirection(
	vtkSmartPointer<vtkActor> actor,
	LPCSTR newValue)
{
	vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
	auto coneSource = dynamic_cast<vtkConeSource*>(algorithm.GetPointer());
	coneSource->SetDirection(CharArrayToValueVector3(newValue));
}