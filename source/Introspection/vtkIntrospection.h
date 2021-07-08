#pragma once

#define BOOST_PYTHON_STATIC_LIB

#include <boost/python.hpp>

#include <vtkObjectBase.h>
#include <vtkAlgorithm.h>

#define NOMINMAX
#include <windows.h>

#include <unordered_map>


class VtkIntrospection
{
public:
	static void InitVtkIntrospection();

	static vtkObjectBase * CreateVtkObject(
		LPCSTR objectName);

	static const char * GetVtkObjectProperty(
		vtkObjectBase * vtkObject,
		LPCSTR propertyName,
		LPCSTR expectedType);

	static void SetVtkObjectProperty(
		vtkObjectBase * vtkObject,
		LPCSTR propertyName,
		LPCSTR newValue);

	static char * GetVtkObjectDescriptor(
		vtkObjectBase * vtkObject);

	static void DeleteVtkObject(
		vtkObjectBase * vtkObject);

	static void ConnectVtkObject(
		vtkObjectBase * vtkObject,
		vtkAlgorithm * vtkTarget);

private:
	static boost::python::object introspector;
	static std::unordered_map<vtkObjectBase *, boost::python::object> nodes;
};