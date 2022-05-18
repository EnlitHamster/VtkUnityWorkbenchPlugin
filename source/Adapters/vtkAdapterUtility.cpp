#include "vtkAdapterUtility.h"
#include "Singleton.h"

#include "vtkConeSourceAdapter.h"

const std::unordered_map<LPCSTR, vtkAdapter*> vtkAdapterUtility::s_adapters =
{
//	{ Singleton<vtkConeSourceAdapter>::Instance()->GetAdaptingObject(), Singleton<vtkConeSourceAdapter>::Instance() },
};

std::ofstream vtkAdapterUtility::s_log;


vtkAdapter* vtkAdapterUtility::GetAdapter(
	LPCSTR vtkAdaptedObject)
{
	if (!s_log.is_open())
	{
		s_log.open("adapter_utility.log");
	}

	s_log << "Loaded adapters" << std::endl;
	for (auto a : s_adapters)
	{
		s_log << a.first << std::endl;
	}
	s_log << std::endl << "Looking for " << vtkAdaptedObject << std::endl;

	for (auto adapter : s_adapters)
	{
		if (strcmp(adapter.first, vtkAdaptedObject))
		{
			s_log << "found!" << std::endl;
			s_log.flush();
			return adapter.second;
		}
	}

	s_log.flush();
	return NULL;
}