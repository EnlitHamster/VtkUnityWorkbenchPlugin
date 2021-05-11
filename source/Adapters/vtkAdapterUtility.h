#pragma once

/* This file has been automatically generated through the Python header generation utility
 * 
 * This file contains the necessary information to allow the VtkToUnity plugin to know
 * that the adapters exist and it can call them. As such, this should be generated every
 * time the plugin is built to avoid losing any adapters in the compilation.
 */


// Adapters' header files found in the folder (.h and .hpp)

// Utility includes
#include <unordered_map>

#define NOMINMAX
#include <windows.h>

#include "../Singleton.h"
#include "../vtkAdapter.h"

#include "vtkConeSourceAdapter.h"


// This class is used to register the adapters
class VtkAdapterUtility
{
public:
	static void RegisterAll()
	{
		s_adapters[Singleton<VtkConeSourceAdapter>::Instance()->GetAdaptingObject()] = Singleton<VtkConeSourceAdapter>::Instance();
	}

	static VtkAdapter* GetAdapter(
		LPCSTR vtkAdaptedObject)
	{
		auto itAdapter = s_adapters.find(vtkAdaptedObject);
		if (itAdapter != s_adapters.end())
		{
			return itAdapter->second;
		}
		else
		{
			return NULL;
		}
	}

private:
	// Map with all the adapters registered in this folder
	static std::unordered_map<LPCSTR, VtkAdapter*> s_adapters;
};
