#include "vtkIntrospection.h"

#include <boost/filesystem.hpp>
#include <Python.h>

#include <vtkAlgorithmOutput.h>
#include <vtkPythonUtil.h>

#include <string>
#include <sstream>
#include <fstream>


boost::python::object VtkIntrospection::introspector;
std::unordered_map<vtkObjectBase *, boost::python::object> VtkIntrospection::nodes;


// TODO LOW: add error checks in the body of the methods

void VtkIntrospection::InitVtkIntrospection()
{
	try
	{
		Py_Initialize();

		boost::filesystem::path working_dir = boost::filesystem::absolute("./").normalize();
		PyObject* sys_path = PySys_GetObject("path");
		PyList_Insert(sys_path, 0, PyUnicode_FromString(working_dir.string().c_str()));

		VtkIntrospection::introspector = boost::python::import("Introspector").attr("Introspector")();
	}
	catch (boost::python::error_already_set& e)
	{
		PyErr_PrintEx(0);
	}
}


vtkObjectBase * VtkIntrospection::CreateVtkObject(
	LPCSTR objectName)
{
	// Temporary debugging log system as Debug does not seem to work...
	ofstream logFile;
	logFile.open("vtkIntrospection.log");
	logFile << "VtkIntrospection::CreateVtkObject(" << objectName << ");" << std::endl;
	try
	{
		boost::python::object py_object = VtkIntrospection::introspector.attr("createVtkObject")(objectName);
		boost::python::object py_vtk_object = py_object.attr("vtkInstance");
		vtkObjectBase * vtk_object = vtkPythonUtil::GetPointerFromObject(py_vtk_object.ptr(), objectName);
		VtkIntrospection::nodes.insert(std::make_pair(vtk_object, py_object));
		return vtk_object;
	}
	catch (boost::python::error_already_set& e)
	{
		PyObject * p_type, * p_value, * p_traceback;
		PyErr_Fetch(&p_type, &p_value, &p_traceback);
		logFile << PyString_AsString(p_value);
		PyErr_PrintEx(0);
		return NULL;
	}
	logFile.close();
}


const char * VtkIntrospection::GetVtkObjectProperty(
	vtkObjectBase * vtkObject,
	LPCSTR propertyName,
	LPCSTR expectedType)
{
	auto nodeIter = VtkIntrospection::nodes.find(vtkObject);
	if (VtkIntrospection::nodes.end() != nodeIter)
	{
		try
		{
			boost::python::object node = nodeIter->second;
			boost::python::object val = VtkIntrospection::introspector.attr("getVtkObjectAttribute")(node, propertyName);
			std::stringstream buffer;
			std::string str_val = boost::python::extract<char *>(val);
			buffer << expectedType << "::" << str_val;
			return buffer.str().c_str();
		}
		catch (boost::python::error_already_set& e)
		{
			PyErr_PrintEx(0);
			return NULL;
		}
	}
}

void VtkIntrospection::SetVtkObjectProperty(
	vtkObjectBase * vtkObject,
	LPCSTR propertyName,
	LPCSTR newValue)
{
	auto nodeIter = VtkIntrospection::nodes.find(vtkObject);
	if (VtkIntrospection::nodes.end() != nodeIter)
	{
		try
		{
			boost::python::object node = nodeIter->second;
			VtkIntrospection::introspector.attr("setVtkObjectAttribute")(node, propertyName, newValue);
		}
		catch (boost::python::error_already_set& e)
		{
			PyErr_PrintEx(0);
		}
	}
}

char * VtkIntrospection::GetVtkObjectDescriptor(
	vtkObjectBase * vtkObject)
{
	auto nodeIter = VtkIntrospection::nodes.find(vtkObject);
	if (VtkIntrospection::nodes.end() != nodeIter)
	{
		try
		{
			boost::python::object node = nodeIter->second;
			boost::python::object descriptor = VtkIntrospection::introspector.attr("getVtkObjectDescriptor")(node);
			return boost::python::extract<char *>(descriptor);
		}
		catch (boost::python::error_already_set& e)
		{
			PyErr_PrintEx(0);
			return NULL;
		}
	}
}


void VtkIntrospection::DeleteVtkObject(
	vtkObjectBase * vtkObject)
{
	auto nodeIter = VtkIntrospection::nodes.find(vtkObject);
	if (VtkIntrospection::nodes.end() != nodeIter)
	{
		try
		{
			boost::python::object node = nodeIter->second;
			VtkIntrospection::introspector.attr("deleteVtkObject")(node);
			VtkIntrospection::nodes.erase(vtkObject);
		}
		catch (boost::python::error_already_set& e)
		{
			PyErr_PrintEx(0);
		}
	}
}


void VtkIntrospection::ConnectVtkObject(
	vtkObjectBase * vtkObject,
	vtkAlgorithm * vtkTarget)
{
	auto nodeIter = VtkIntrospection::nodes.find(vtkObject);
	if (VtkIntrospection::nodes.end() != nodeIter)
	{
		try
		{
			boost::python::object node = nodeIter->second;
			boost::python::object output_port = VtkIntrospection::introspector.attr("getVtkObjectOutputPort")(node);
			vtkTarget->SetInputConnection((vtkAlgorithmOutput *)vtkPythonUtil::GetPointerFromObject(output_port.ptr(), "vtkAlgorithmOutput"));
		}
		catch (boost::python::error_already_set& e)
		{
			PyErr_PrintEx(0);
		}
	}
}