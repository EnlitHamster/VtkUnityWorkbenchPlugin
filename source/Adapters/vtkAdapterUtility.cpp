#include "vtkAdapterUtility.h"
#include "Singleton.h"

#include "vtkConeSourceAdapter.h"

const std::unordered_map<LPCSTR, vtkAdapter*> vtkAdapterUtility::s_adapters =
{
	{ Singleton<vtkConeSourceAdapter>::Instance()->GetAdaptingObject(), Singleton<vtkConeSourceAdapter>::Instance() },
};


vtkAdapter* vtkAdapterUtility::GetAdapter(
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