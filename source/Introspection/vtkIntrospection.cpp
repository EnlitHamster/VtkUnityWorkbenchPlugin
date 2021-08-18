#include "vtkIntrospection.h"

#include <Python.h>

#include <vtkAlgorithmOutput.h>
#include <vtkPythonUtil.h>

#include <sstream>
#include <cstdarg>
#include <ctype.h>

#define NOMINMAX
#include <windows.h>


// Initializing static attributes

PyObject *VtkIntrospection::pIntrospector;
std::unordered_map<vtkObjectBase *, PyObject *> VtkIntrospection::nodes;

std::stringstream VtkIntrospection::errorBuffer;


#ifdef PYTHON_EMBED_LOG
std::ofstream VtkIntrospection::log;
#endif


void VtkIntrospection::InitIntrospector()
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log.open("python_embed.log");
	VtkIntrospection::log << "called VtkIntrospection::InitIntrospector" << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Activating virtual environment */
	const wchar_t *sPyHome = L"venv";
	Py_SetPythonHome(sPyHome);

	/* Initializing Python environment and setting PYTHONPATH. */
	Py_Initialize();

	/* Both the "." and cwd notations are left in for security, as after being built in
	   a DLL they may change. */
	PyRun_SimpleString("import sys\nimport os");
	PyRun_SimpleString("sys.path.append( os.path.dirname(os.getcwd()) )");
	PyRun_SimpleString("sys.path.append(\".\")");

	/* Decode module from its name. Returns error if the name is not decodable. */
	PyObject *pIntrospectorModuleName = PyUnicode_DecodeFSDefault("Introspector");
	if (pIntrospectorModuleName == NULL)
	{
		VtkIntrospection::ErrorSet("Fatal error: cannot decode module name");
		return;
	}

	/* Imports the module previously decoded. Returns error if the module is not found. */
	PyObject *pIntrospectorModule = PyImport_Import(pIntrospectorModuleName);
	Py_DECREF(pIntrospectorModuleName);
	if (pIntrospectorModule == NULL)
	{
		VtkIntrospection::ErrorSet("Failed to load \"Introspector\"");
		return;
	}

	/* Looks for the Introspector class in the module. If it does not find it, returns and error. */
	PyObject* pIntrospectorClass = PyObject_GetAttrString(pIntrospectorModule, "Introspector");
	Py_DECREF(pIntrospectorModule);
	if (pIntrospectorClass == NULL || !PyCallable_Check(pIntrospectorClass))
	{
		VtkIntrospection::ErrorSet("Cannot find class \"Introspector\"");
		if (pIntrospectorClass != NULL)
		{
			Py_DECREF(pIntrospectorClass);
		}
		return;
	}

	/* Instantiates an Introspector object. If the call returns NULL there was an error
	   creating the object, and thus it returns error. */
	VtkIntrospection::pIntrospector = PyObject_CallObject(pIntrospectorClass, NULL);
	Py_DECREF(pIntrospectorClass);
	if (VtkIntrospection::pIntrospector == NULL)
	{
		VtkIntrospection::ErrorSet("Introspector instantiation failed");
		return;
	}
}


void VtkIntrospection::FinalizeIntrospector()
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::FinalizeIntrospector" << std::endl;
	VtkIntrospection::log.flush();
#endif

	for (auto iNode : VtkIntrospection::nodes)
	{
		vtkObjectBase *pVtkObject = iNode.first;
		VtkIntrospection::DeleteObject(pVtkObject);
	}
	VtkIntrospection::nodes.clear();

	Py_DECREF(VtkIntrospection::pIntrospector);
	Py_Finalize();

	VtkIntrospection::ErrorSet(NULL); // Set an error if there is any

#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log.close();
#endif
}


vtkObjectBase *VtkIntrospection::CreateObject(
	LPCSTR classname)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::CreateObject with classname = " << classname << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Creating the object and getting the reference. Returns error if the object could not
	   be created.*/
	PyObject *pPyVtkObject = PyObject_CallMethod(VtkIntrospection::pIntrospector, "createVtkObject", "s", classname);
	if (pPyVtkObject == NULL)
	{
		VtkIntrospection::ErrorSet("Cannot call \"createVtkObject\" on \"", classname, "\"");
		return NULL;
	}

	/* Retrieving vtk instance. Returns error if it cannot access the vtk instance. */
	PyObject *pPyVtkInstance = PyObject_GetAttrString(pPyVtkObject, "vtkInstance");
	if (pPyVtkInstance == NULL)
	{
		VtkIntrospection::ErrorSet("Cannot access \"vtkInstance\" of VTK wrapped object");
		return NULL;
	}

	/* Retrieving C object from vtk instance */
	vtkObjectBase *pVtkObject = vtkPythonUtil::GetPointerFromObject(pPyVtkInstance, classname);
	Py_DECREF(pPyVtkInstance);

	/* Adding a node entry to the vtk objects - nodes map. */
	VtkIntrospection::nodes.insert(std::make_pair(pVtkObject, pPyVtkObject));

	return pVtkObject;
}


LPCSTR VtkIntrospection::GetProperty(
	vtkObjectBase *pVtkObject,
	LPCSTR propertyName)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::GetProperty with pVtkObject = " << pVtkObject << ", propertyName = " << propertyName << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Retrieving node from registry. Returns error if the VTK object has no node. */
	auto iNode = VtkIntrospection::nodes.find(pVtkObject);
	if (VtkIntrospection::nodes.end() != iNode)
	{
		/* Getting Python node. */
		PyObject *pNode = iNode->second;

		/* Retrieving the property value. Returns error if there is no property with the given name. */
		PyObject *pVal = PyObject_CallMethod(VtkIntrospection::pIntrospector, "getVtkObjectAttribute", "Os", pNode, propertyName);
		if (pVal == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot access the VTK object's attribute \"", propertyName, "\"");
			return NULL;
		}

		/* Converting the value to string. Returns error if unable to. */
		const char* propertyValue = PyString_AsString(pVal);
		Py_DECREF(pVal);
		if (propertyValue == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot convert attribute \"", propertyName, "\" to string");
			return NULL;
		}

		return propertyValue;
	}
	else
	{
		return NULL;
	}
}


void VtkIntrospection::SetProperty(
	vtkObjectBase *pVtkObject,
	LPCSTR propertyName,
	LPCSTR format,
	LPCSTR newValue)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::SetProperty with pVtkObject = " << pVtkObject << ", propertyName = " << propertyName << ", format = " << format << ", newValue = " << newValue << std::endl;
	VtkIntrospection::log.flush();
#endif
	/* Retriving node from registry. Returns error if the VTK object has no node. */
	auto iNode = VtkIntrospection::nodes.find(pVtkObject);
	if (VtkIntrospection::nodes.end() != iNode)
	{
		/* Getting Python node. */
		PyObject *pNode = iNode->second;

		/* Executing method call to set value. Returns error if the value could not be set. */
		PyObject *pCheck = PyObject_CallMethod(VtkIntrospection::pIntrospector, "setVtkObjectAttribute", "Osss", pNode, propertyName, format, newValue);
		if (pCheck == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot set the VTK object's attribute \"", propertyName, "\"");
			return;
		}
		Py_DECREF(pCheck);
	}
}


LPCSTR VtkIntrospection::GetDescriptor(
	vtkObjectBase *pVtkObject)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::GetDescriptor with pVtkObject = " << pVtkObject << std::endl;
	VtkIntrospection::log.flush();
#endif

	auto iNode = VtkIntrospection::nodes.find(pVtkObject);
	if (VtkIntrospection::nodes.end() != iNode)
	{
		/* Getting Python node. */
		PyObject *pNode = iNode->second;

		/* Retrieving the descriptor. Returns error if the descriptor could not be built. */
		PyObject *pDescriptor = PyObject_CallMethod(VtkIntrospection::pIntrospector, "getVtkObjectDescriptor", "O", pNode);
		if (pDescriptor == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot access the VTK object's descriptor");
			return NULL;
		}

		/* Converting the value to string. Returns error if unable to. */
		const char* descriptor = PyString_AsString(pDescriptor);
		Py_DECREF(pDescriptor);
		if (descriptor == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot convert descriptor to string");
			return NULL;
		}

		return descriptor;
	}
	else
	{
		return NULL;
	}
}


void VtkIntrospection::DeleteObject(
	vtkObjectBase* pVtkObject)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::DeleteObject with pVtkObject = " << pVtkObject << std::endl;
	VtkIntrospection::log.flush();
#endif

	auto iNode = VtkIntrospection::nodes.find(pVtkObject);
	if (VtkIntrospection::nodes.end() != iNode)
	{
		/* Getting Python node. */
		PyObject *pNode = iNode->second;

		/* Executing method call to set value. Returns error if the value could not be set. */
		PyObject *pCheck = PyObject_CallMethod(VtkIntrospection::pIntrospector, "deleteVtkObject", "O", pNode);
		if (pCheck == NULL)
		{
			VtkIntrospection::ErrorSet("Cannot delete the VTK object");
			return;
		}
		Py_DECREF(pCheck);

		/* Freeing the node's space and cleaning up. */
		Py_DECREF(pNode);
		VtkIntrospection::nodes.erase(pVtkObject);
	}
}


static size_t argsize(LPCSTR str)
{
	size_t size = 0;
	size_t maxsize = std::strlen(str);
	for (int i = 0; i < maxsize; ++i)
	{
		if (isalpha(str[i]))
		{
			++size;
		}
	}

	return size;
}


PyObject *VtkIntrospection::ArgvTuple(
	LPCSTR format,
	size_t argc,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	PyObject *pArgs = PyTuple_New(argc);
	if (pArgs == NULL)
	{
		VtkIntrospection::ErrorSet("Unable to create a tuple of size ", std::to_string(argc).c_str());
		return NULL;
	}

	/* Populating arguments. */
	for (int i = 0; i < argc; ++i)
	{
		int objects = 0;
		int values = 0;
		PyObject *pVal = NULL;

		char def = format[i];

		/* Checking for size specifications */
		std::stringstream strspec;
		int di = 1;
		while (isdigit(format[i + di]))
		{
			strspec << format[i + di];
			++di;
		}

		int spec = 0;
		if (di > 1)
		{
			spec = std::stoi(strspec.str());
			i += di - 1;
		}

		if (def == 'o' || def == 'O')
		{
			/* Getting the object's reference. */
			auto iNodeRef = nodes.find(refv[objects]);
			if (nodes.end() != iNodeRef)
			{
				pVal = iNodeRef->second;
				++objects;
			}
			else
			{
				/* The object may be wrappable as a VTK object. */
				pVal = vtkPythonUtil::GetObjectFromPointer(refv[objects]);

				if (pVal == NULL)
				{
					VtkIntrospection::ErrorSet("Reference out of bound ", std::to_string(objects).c_str());
					Py_XDECREF(pArgs);
					return NULL;
				}
			}
		}
		else
		{
			/* Getting the string. */
			if (argv[values] == NULL)
			{
				VtkIntrospection::ErrorSet("Value out of bound ", std::to_string(values).c_str());
				Py_XDECREF(pArgs);
				return NULL;
			}

			/* Unspec-ed argument */
			if (spec == 0)
			{
				const char *str = argv[values++];

				switch (def)
				{
				case 's':
				case 'S':
					pVal = PyString_FromString(str);
					break;

				case 'd':
				case 'D':
					pVal = PyLong_FromLong(std::stol(str, nullptr));
					break;

				case 'f':
				case 'F':
					pVal = PyFloat_FromDouble(std::stod(str, nullptr));
					break;

				case 'b':
				case 'B':
					pVal = PyBool_FromLong(std::stol(str, nullptr));
					break;

				default:
					VtkIntrospection::ErrorSet("Format no recognised ", def);
					Py_XDECREF(pArgs);
					return NULL;
				}
			}
			/* Spec-ed argument */
			else
			{
				PyObject *pTuple = PyTuple_New(spec);
				if (pTuple == NULL)
				{
					VtkIntrospection::ErrorSet("Unable to create a tuple of size ", std::to_string(spec).c_str());
					Py_XDECREF(pArgs);
					return NULL;
				}

				for (int j = 0; j < spec; ++j)
				{
					const char *str = argv[values++];

					switch (def)
					{
					case 's':
					case 'S':
						PyTuple_SET_ITEM(pTuple, j, PyString_FromString(str));
						break;

					case 'd':
					case 'D':
						PyTuple_SET_ITEM(pTuple, j, PyLong_FromLong(std::stol(str, nullptr)));
						break;

					case 'f':
					case 'F':
						PyTuple_SET_ITEM(pTuple, j, PyFloat_FromDouble(std::stod(str, nullptr)));
						break;

					case 'b':
					case 'B':
						PyTuple_SET_ITEM(pTuple, j, PyBool_FromLong(std::stol(str, nullptr)));
						break;

					default:
						VtkIntrospection::ErrorSet("Format no recognised ", def);
						Py_XDECREF(pTuple);
						Py_XDECREF(pArgs);
						return NULL;
					}
				}

				pVal = pTuple;
			}
		}

		if (pVal == NULL)
		{
			VtkIntrospection::ErrorSet("Argument number ", std::to_string(i).c_str(), " is not encodable with type \"", def, "\"");
			return NULL;
		}
		PyTuple_SET_ITEM(pArgs, i, pVal);
	}

	return pArgs;
}


PyObject *VtkIntrospection::CallMethod(
	vtkObjectBase *pVtkObject,
	LPCSTR method,
	LPCSTR format,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::ObjectMethod with pVtkObject = " << pVtkObject << ", method = " << method << ", format = " << format << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Retrieving node from registry. Returns error if the VTK object has no node. */
	auto iNode = VtkIntrospection::nodes.find(pVtkObject);
	if (VtkIntrospection::nodes.end() != iNode)
	{
		/* Getting Python node. */
		PyObject *pNode = iNode->second;

		/* Generating argument list. */
		size_t argc = argsize(format);

		PyObject *pArgs = VtkIntrospection::ArgvTuple(format, argc, refv, argv);
		if (pArgs == NULL)
		{
			/* Escalating error. */
			return NULL;
		}

		/* Calling the method. */
		PyObject *pReturn = PyObject_CallMethod(VtkIntrospection::pIntrospector, "vtkInstanceCall", "OsO", pNode, method, pArgs);
		Py_XDECREF(pArgs);
		if (pReturn == NULL)
		{
			VtkIntrospection::ErrorSet("Method \"", method, "\" call resulted in error.");
			return NULL;
		}

		return pReturn;
	}
	else
	{
		VtkIntrospection::ErrorSet("Cannot find node with id ", pVtkObject);
		return NULL;
	}
}


LPCSTR VtkIntrospection::CallMethod_AsString(
	vtkObjectBase *pVtkObject,
	LPCSTR method,
	LPCSTR format,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. */
	PyObject *pVal = VtkIntrospection::CallMethod(pVtkObject, method, format, refv, argv);
	if (pVal == NULL)
	{
		/* Escalating the error. */
		return NULL;
	}

#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "VtkIntrospection::ObjectMethod called successfully" << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Decoding return value. */
	PyObject *pReturn = PyObject_Str(pVal);
	Py_DECREF(pVal);
	if (pReturn == NULL)
	{
		VtkIntrospection::ErrorSet("Unable to decode return value.");
		return NULL;
	}

#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "Return value analysed successfully" << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* TODO: format return value based on return type. */

	/* Extracting string value. */
	LPCSTR str = strdup(PyString_AsString(pReturn));
	Py_DECREF(pReturn);
	
	return str;
}


vtkObjectBase *VtkIntrospection::CallMethod_AsVtkObject(
	vtkObjectBase *pVtkObject,
	LPCSTR method,
	LPCSTR classname,
	LPCSTR format,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. */
	PyObject *pVal = VtkIntrospection::CallMethod(pVtkObject, method, format, refv, argv);
	if (pVal == NULL)
	{
		/* Escalating the error. */
		return NULL;
	}

	/* Retrieving VTK Object. */
	for (auto node : VtkIntrospection::nodes)
	{
		if (node.second == pVal)
		{
			return node.first;
		}
	}

	/* The VTK object is not yet registered. Registering it now. */
	PyObject *pNewNode = PyObject_CallMethod(pIntrospector, "createVtkObjectWithInstance", "sO", classname, pVal);
	if (pNewNode == NULL)
	{
		VtkIntrospection::ErrorSet("Cannot create node for new object");
		return NULL;
	}

	/* Retrieving C object from vtk instance */
	vtkObjectBase *pReturnVtkObject = vtkPythonUtil::GetPointerFromObject(pVal, classname);
	Py_DECREF(pVal);

	/* Adding a node entry to the vtk objects - nodes map. */
	VtkIntrospection::nodes.insert(std::make_pair(pReturnVtkObject, pNewNode));

	return pReturnVtkObject;
}


void VtkIntrospection::CallMethod_AsVoid(
	vtkObjectBase *pVtkObject,
	LPCSTR method,
	LPCSTR format,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. Releasing it immediately as the result is of no interest. */
	PyObject *pVal = VtkIntrospection::CallMethod(pVtkObject, method, format, refv, argv);
	Py_XDECREF(pVal);
}


static void argsize(LPCSTR str, size_t *pRefs, size_t *pVals)
{
	size_t maxsize = std::strlen(str);
	for (int i = 0; i < maxsize; ++i)
	{
		if (str[i] == 'o' || str[i] == 'O')
		{
			++(*pRefs);
		}
		else if (isalpha(str[i]))
		{
			++(*pVals);
		}
	}
}


/*
 * Intermediaries need to be VTK objects
 */
PyObject *VtkIntrospection::CallMethodPiped(
	vtkObjectBase *pVtkObject,
	std::vector<LPCSTR> methods,
	std::vector<LPCSTR> formats,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
#ifdef PYTHON_EMBED_LOG
	VtkIntrospection::log << "called VtkIntrospection::CallMethod_Piped with pVtkObject = " << pVtkObject << std::endl;
	VtkIntrospection::log.flush();
#endif

	/* Getting the first arguments. */
	LPCSTR method = methods[0];
	LPCSTR format = formats[0];
	size_t refc = 0;
	size_t valc = 0;
	argsize(format, &refc, &valc);
	std::vector<vtkObjectBase *> pRefv(refv.begin(), refv.begin() + refc);
	std::vector<LPCSTR> pArgv(argv.begin(), argv.begin() + valc);

	/* First call is on a node. Further calls are not. */
	PyObject *pPipedCaller = VtkIntrospection::CallMethod(pVtkObject, method, format, pRefv, pArgv);

	for (int i = 1; i < methods.size(); ++i)
	{
		/* Getting new arguments. */
		method = methods[i];
		format = formats[i];
		size_t oldrefc = refc;
		size_t oldvalc = valc;
		argsize(format, &refc, &valc);
		pRefv.assign(refv.begin() + oldrefc, refv.begin() + refc);
		pArgv.assign(argv.begin() + oldvalc, argv.begin() + valc);

		/* Call on next piped element. */
		PyObject *pArgs = VtkIntrospection::ArgvTuple(format, (refc - oldrefc) + (valc - oldvalc), pRefv, pArgv);
		if (pArgs == NULL)
		{
			/* Escalating error. */
			return NULL;
		}

		/* Getting the next pipe object. */
		PyObject *pNextPipedCaller = PyObject_CallMethod(pIntrospector, "genericCall", "OsO", pPipedCaller, method, pArgs);
		if (pNextPipedCaller == NULL)
		{
			VtkIntrospection::ErrorSet("Could not call \"", method, "\"");
			return NULL;
		}

		/* Swapping to next caller. */
		Py_DECREF(pPipedCaller);
		pPipedCaller = pNextPipedCaller;
	}

	/* Returning the last return value. */
	return pPipedCaller;
}


LPCSTR VtkIntrospection::CallMethodPiped_AsString(
	vtkObjectBase *pVtkObject,
	std::vector<LPCSTR> methods,
	std::vector<LPCSTR> formats,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. */
	PyObject *pVal = VtkIntrospection::CallMethodPiped(pVtkObject, methods, formats, refv, argv);
	if (pVal == NULL)
	{
		/* Escalating the error. */
		return NULL;
	}

	/* Decoding return value. */
	PyObject *pReturn = PyObject_CallMethod(pIntrospector, "outputFormat", "(O)", pVal);
	Py_DECREF(pVal);
	if (pReturn == NULL)
	{
		VtkIntrospection::ErrorSet("Unable to decode return value");
		return NULL;
	}

	/* Extracting string value. */
	LPCSTR str = strdup(PyString_AsString(pReturn));
	Py_DECREF(pReturn);

	return str;
}


vtkObjectBase *VtkIntrospection::CallMethodPiped_AsVtkObject(
	vtkObjectBase *pVtkObject,
	std::vector<LPCSTR> methods,
	std::vector<LPCSTR> formats,
	LPCSTR classname,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. */
	PyObject *pVal = VtkIntrospection::CallMethodPiped(pVtkObject, methods, formats, refv, argv);
	if (pVal == NULL)
	{
		/* Escalating the error. */
		return NULL;
	}

	/* Retrieving VTK Object. */
	for (auto node : VtkIntrospection::nodes)
	{
		if (node.second == pVal)
		{
			return node.first;
		}
	}

	/* The VTK object is not yet registered. Registering it now. */
	PyObject *pNewNode = PyObject_CallMethod(pIntrospector, "createVtkObjectWithInstance", "sO", classname, pVal);
	if (pNewNode == NULL)
	{
		VtkIntrospection::ErrorSet("Cannot create node for new object");
		return NULL;
	}

	/* Retrieving C object from vtk instance */
	vtkObjectBase *pReturnVtkObject = vtkPythonUtil::GetPointerFromObject(pVal, classname);
	Py_DECREF(pVal);

	/* Adding a node entry to the vtk objects - nodes map. */
	VtkIntrospection::nodes.insert(std::make_pair(pReturnVtkObject, pNewNode));

	return pReturnVtkObject;
}


void VtkIntrospection::CallMethodPiped_AsVoid(
	vtkObjectBase *pVtkObject,
	std::vector<LPCSTR> methods,
	std::vector<LPCSTR> formats,
	std::vector<vtkObjectBase *> refv,
	std::vector<LPCSTR> argv)
{
	/* Calling the method and getting encoded result. Releasing it immediately as the result is of no interest. */
	PyObject *pVal = VtkIntrospection::CallMethodPiped(pVtkObject, methods, formats, refv, argv);
	Py_XDECREF(pVal);
}


void VtkIntrospection::ErrorSet(
	LPCSTR pGenericMessages ...)
{
	VtkIntrospection::errorBuffer.clear();

	if (PyErr_Occurred())
	{
		PyObject *pType, *pValue, *pTraceback;
		PyErr_Fetch(&pType, &pValue, &pTraceback);

		VtkIntrospection::errorBuffer << PyString_AsString(pValue) << std::endl;

#ifdef PYTHON_EMBED_LOG
		VtkIntrospection::log << VtkIntrospection::errorBuffer.str();
		VtkIntrospection::log.flush();
#endif
	}
	else if (pGenericMessages != NULL)
	{
		va_list args;
		va_start(args, pGenericMessages);

		VtkIntrospection::errorBuffer << "Unknown error: ";

		while (pGenericMessages != '\0')
		{
			VtkIntrospection::errorBuffer << va_arg(args, LPCSTR);
		}

		VtkIntrospection::errorBuffer << std::endl;

#ifdef PYTHON_EMBED_LOG
		VtkIntrospection::log << VtkIntrospection::errorBuffer.str();
		VtkIntrospection::log.flush();
#endif
	}
}


LPCSTR VtkIntrospection::ErrorGet()
{
	LPCSTR pErrorMessage = VtkIntrospection::errorBuffer.str().c_str();
	VtkIntrospection::errorBuffer.clear();
	return pErrorMessage;
}

bool VtkIntrospection::ErrorOccurred()
{
	return VtkIntrospection::errorBuffer.rdbuf()->in_avail() > 0;
}