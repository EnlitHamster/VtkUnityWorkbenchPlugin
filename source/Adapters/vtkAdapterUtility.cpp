#include "vtkAdapterUtility.h"
#include "../Singleton.h"

#include "vtkConeSourceAdapter.h"

const std::unordered_map<LPCSTR, VtkAdapter*> VtkAdapterUtility::s_adapters =
{
	{ Singleton<VtkConeSourceAdapter>::Instance()->GetAdaptingObject(), Singleton<VtkConeSourceAdapter>::Instance() },
};


VtkAdapter* VtkAdapterUtility::GetAdapter(
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