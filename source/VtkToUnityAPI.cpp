#include "VtkToUnityAPI.h"
#include "PlatformBase.h"
#include "Unity/IUnityGraphics.h"


VtkToUnityAPI* CreateVtkToUnityAPI(UnityGfxRenderer apiType)
{
#	if SUPPORT_D3D11
	if (apiType == kUnityGfxRendererD3D11)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_D3D11();
		return CreateVtkToUnityAPI_D3D11();
	}
#	endif // if SUPPORT_D3D11

#	if SUPPORT_D3D9
	if (apiType == kUnityGfxRendererD3D9)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_D3D9();
		return CreateVtkToUnityAPI_D3D9();
	}
#	endif // if SUPPORT_D3D9

#	if SUPPORT_D3D12
	if (apiType == kUnityGfxRendererD3D12)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_D3D12();
		return CreateVtkToUnityAPI_D3D12();
	}
#	endif // if SUPPORT_D3D9


#	if SUPPORT_OPENGL_UNIFIED
	if (apiType == kUnityGfxRendererOpenGLCore || apiType == kUnityGfxRendererOpenGLES20 || apiType == kUnityGfxRendererOpenGLES30)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_OpenGLCoreES(UnityGfxRenderer apiType);
		return CreateVtkToUnityAPI_OpenGLCoreES(apiType);
	}
#	endif // if SUPPORT_OPENGL_UNIFIED

#	if SUPPORT_OPENGL_LEGACY
	if (apiType == kUnityGfxRendererOpenGL)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_OpenGL2();
		return CreateVtkToUnityAPI_OpenGL2();
	}
#	endif // if SUPPORT_OPENGL_LEGACY

#	if SUPPORT_METAL
	if (apiType == kUnityGfxRendererMetal)
	{
		extern VtkToUnityAPI* CreateVtkToUnityAPI_Metal();
		return CreateVtkToUnityAPI_Metal();
	}
#	endif // if SUPPORT_METAL


	// Unknown or unsupported graphics API
	return NULL;
}
