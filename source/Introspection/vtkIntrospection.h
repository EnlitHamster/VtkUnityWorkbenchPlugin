#pragma once

#include <Python.h>

#include <vtkObjectBase.h>
#include <vtkAlgorithm.h>

#define NOMINMAX
#include <windows.h>

#include <unordered_map>
#include <map>


#define PYTHON_EMBED_LOG

#ifdef PYTHON_EMBED_LOG
#include <fstream>
#endif


class VtkIntrospection
{
public:
	/*
	 * Initializes Python interpreter and the Introspection object.
	 */
	static void InitIntrospector();

	/*
	 * Finalizes the Python interpreter, the Introspection object and
	 * the clears the nodes registry, freeing the entire Python space.
	 */
	static void FinalizeIntrospector();

	/*
	 * Creates a ClassTree node in Python for the object and extracts
	 * the vtkObjectBase C++ object from it, returning it to the user.
	 */
	static vtkObjectBase *CreateObject(
		LPCSTR vtkClassname);

	/*
	 * Accesses the Python node corresponding to the vtkObjectBase,
	 * retrieving the queried property in string form to the user.
	 */
	static LPCSTR GetProperty(
		vtkObjectBase *pVtkObject,
		LPCSTR propertyName);

	/*
	 * Accesses the Python node corresopnding to the vtkObjectBase,
	 * parsing the string value and setting the property to the
	 * parsed value.
	 */
	static void SetProperty(
		vtkObjectBase *pVtkObject,
		LPCSTR propertyName,
		LPCSTR format,
		LPCSTR newValue);

	/*
	 * Returns a string representation of the class, with the properties
	 * that can be modified by the user that are directly part of the
	 * object and not inherited.
	 */
	static LPCSTR GetDescriptor(
		vtkObjectBase *pVtkObject);

	/*
	 * Deletes the Python references to the object. This does not
	 * free the VTK object's memory space, that is the user's
	 * responsibility.
	 */
	static void DeleteObject(
		vtkObjectBase *pVtkObject);

	/*
	 * Generic call on the object. Base API support. Returns a string.
	 */
	static LPCSTR CallMethod_AsString(
		vtkObjectBase *pVtkObject,
		LPCSTR method,
		LPCSTR format,
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/*
	 * Generic call on the object. Base API support. Returns a VTK object.
	 */
	static vtkObjectBase *CallMethod_AsVtkObject(
		vtkObjectBase *pVtkObject,
		LPCSTR method,
		LPCSTR format,
		LPCSTR classname,
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/*
	 * Generic call on the object. Base API support. Has no return value.
	 */
	static void CallMethod_Void(
		vtkObjectBase *pVtkObject,
		LPCSTR method,
		LPCSTR format,
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/* 
	 * Generic piped call on the object. Base API support. Returns a string.
	 */
	static LPCSTR CallMethodPiped_AsString(
		vtkObjectBase *pVtkObject,
		std::vector<LPCSTR> methods,
		std::vector<LPCSTR> formats,
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/*
	 * Generic piped call on the object. Base API support. Returns a string.
	 */
	static vtkObjectBase *CallMethodPiped_AsVtkObject(
		vtkObjectBase *pVtkObject,
		std::vector<LPCSTR> methods,
		std::vector<LPCSTR> formats,
		LPCSTR classname, 
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/*
	 * Generic piped call on the object. Base API support. Returns a string.
	 */
	static void CallMethodPiped_Void(
		vtkObjectBase *pVtkObject,
		std::vector<LPCSTR> methods,
		std::vector<LPCSTR> formats,
		std::vector<vtkObjectBase *> references,
		std::vector<LPCSTR> argv);

	/*
	 * Returns whether the error buffers are set.
	 */
	static bool ErrorOccurred();

	/*
	 * Dumps the error buffers and clears them.
	 */
	static LPCSTR ErrorGet();

private:

	/*
	 * Introspector object, entry point for the Python code.
	 */
	static PyObject *pIntrospector;
	
	/*
	 * Mapping from VTK object to its node in the ClassTree.
	 */
	static std::unordered_map<vtkObjectBase *, PyObject *> nodes;

	/*
	 * Error log buffers.
	 */
	static std::stringstream errorBuffer;

#ifdef PYTHON_EMBED_LOG
	static std::ofstream log;
#endif

	/*
	 * Checks whether an error was thrown and stores it.
	 */
	static void ErrorSet(
		LPCSTR pGenericMessages ...);

	/*
	 * Generates an args tuple from an argument vector.
	 */
	static PyObject *ArgvTuple(
		LPCSTR format,
		size_t argc,
		std::vector<vtkObjectBase *> pReferences,
		std::vector<LPCSTR> argv);

	/*
	 * Calls a method and returns the PyObject encoded return value.
	 */
	static PyObject *CallMethod(
		vtkObjectBase *pVtkObject,
		LPCSTR method,
		LPCSTR format,
		std::vector<vtkObjectBase *> pReferences,
		std::vector<LPCSTR> argv);

	/*
	 * Executes a piped call. All intermediaries have to be vtkObjectBase instances.
	 */
	static PyObject *CallMethodPiped(
		vtkObjectBase *pVtkObject,
		std::vector<LPCSTR> methods,
		std::vector<LPCSTR> formats,
		std::vector<vtkObjectBase *> pReferences,
		std::vector<LPCSTR> argv);
};