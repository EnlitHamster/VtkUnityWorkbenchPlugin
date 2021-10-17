#pragma once

/* This file has been automatically generated through the Python header generation utility
 * 
 * This file contains the necessary information to allow the VtkToUnity plugin to know
 * that the adapters exist and it can call them. As such, this should be generated every
 * time the plugin is built to avoid losing any adapters in the compilation.
 */


// Utility includes
#include <unordered_map>

#define NOMINMAX
#include <windows.h>

#include "vtkAdapter.h"


// This class is used to register the adapters
class vtkAdapterUtility
{
public:
	static vtkAdapter* GetAdapter(
		LPCSTR vtkAdaptedObject);

private:
	// Map with all the adapters registered in this folder
	static const std::unordered_map<LPCSTR, vtkAdapter*> s_adapters;
};
