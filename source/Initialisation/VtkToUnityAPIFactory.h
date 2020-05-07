#pragma once

#include "../VtkToUnityAPI.h"

class VtkToUnityAPIFactory
{
public:
	static void Initialise(UnityGfxRenderer apiType);
	static void Shutdown();

	static bool Initialised();
	static std::weak_ptr<VtkToUnityAPI> GetAPI();
};
