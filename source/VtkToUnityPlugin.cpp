// Example low level rendering Unity plugin

#include "VtkToUnityPlugin.h"

#include <assert.h>
#include <math.h>
#include <map>
#include <queue>
#include <vector>
#include <sstream>

#include <queue>
#include <mutex>
#include <condition_variable>


static Float4 ZeroFloat4()
{
	Float4 zeroFloat4;
	zeroFloat4.x = 0.0f;
	zeroFloat4.y = 0.0f;
	zeroFloat4.z = 0.0f;
	zeroFloat4.w = 0.0f;
	return zeroFloat4;
};


// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
	SafeQueue(void)
		: q()
		, m()
		, c()
	{}

	~SafeQueue(void)
	{}

	// Add an element to the queue.
	void enqueue(T t)
	{
		std::lock_guard<std::mutex> lock(m);
		q.push(t);
		c.notify_one();
	}

	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	T dequeue(void)
	{
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
		{
			// release lock as long as the wait and reaquire it afterwards.
			c.wait(lock);
		}
		T val = q.front();
		q.pop();
		return val;
	}

	bool empty(void)
	{
		std::unique_lock<std::mutex> lock(m);
		return q.empty();
	}

private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};


// --------------------------------------------------------------------------
// UnitySetInterfaces

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* sUnityInterfaces = NULL;
static IUnityGraphics* sGraphics = NULL;

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	sUnityInterfaces = unityInterfaces;
	sGraphics = sUnityInterfaces->Get<IUnityGraphics>();
	sGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
	
	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	sGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

#if UNITY_WEBGL
typedef void	(UNITY_INTERFACE_API * PluginLoadFunc)(IUnityInterfaces* unityInterfaces);
typedef void	(UNITY_INTERFACE_API * PluginUnloadFunc)();

extern "C" void	UnityRegisterVtkToUnityPlugin(PluginLoadFunc loadPlugin, PluginUnloadFunc unloadPlugin);

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterPlugin()
{
	UnityRegisterVtkToUnityPlugin(UnityPluginLoad, UnityPluginUnload);
}
#endif

// --------------------------------------------------------------------------
// GraphicsDeviceEvent


static VtkToUnityAPI* sCurrentAPI = NULL;
static UnityGfxRenderer sDeviceType = kUnityGfxRendererNull;


static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	// Create graphics API implementation upon initialization
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		assert(sCurrentAPI == NULL);
		sDeviceType = sGraphics->GetRenderer();
		sCurrentAPI = CreateVtkToUnityAPI(sDeviceType);
	}

	// Let the implementation process the device related events
	if (sCurrentAPI)
	{
		sCurrentAPI->ProcessDeviceEvent(eventType, sUnityInterfaces);
	}

	// Cleanup graphics API implementation upon shutdown
	if (eventType == kUnityGfxDeviceEventShutdown)
	{
		delete sCurrentAPI;
		sCurrentAPI = NULL;
		sDeviceType = kUnityGfxRendererNull;
	}
}


// --------------------------------------------------------------------------
// Load in a DICOM volume from the specified folder

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LoadDicomVolume(
	const char *dicomFolder)
{
	//Debug("LoadDicomVolume: Start");

	// load in the volume for the volume renderer too
	if (sCurrentAPI == NULL) {
		return false;
	}

	if (dicomFolder == NULL) {
		Debug("LoadDicomVolume: NULL string pointer passed in");
		return false;
	}

	if (*dicomFolder == '\0') {
		Debug("LoadDicomVolume: string with no length passed in");
		return false;
	}

	std::string dicomFolderStr(dicomFolder);

	sCurrentAPI->LoadDicomVolumeFromFolder(dicomFolderStr);
	return true; // todo return value from the underlying call
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LoadMhdVolume(
	const char *mhdPath)
{
	//Debug("LoadMhdVolume: Start");

	// load in the volume for the volume renderer too
	if (sCurrentAPI == NULL) {
		return false;
	}

	if (mhdPath == NULL) {
		Debug("LoadMhdVolume: NULL string pointer passed in");
		return false;
	}

	if (*mhdPath == '\0') {
		Debug("LoadMhdVolume: string with no length passed in");
		return false;
	}

	std::string mhdPathStr(mhdPath);

	sCurrentAPI->LoadUncMetaImage(mhdPathStr);
	return true; // todo return value from the underlying call
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LoadNrrdVolume(
	const char *nrrdPath)
{
	//Debug("LoadMhdVolume: Start");

	// load in the volume for the volume renderer too
	if (sCurrentAPI == NULL) {
		return false;
	}

	if (nrrdPath == NULL) {
		Debug("LoadNrrdVolume: NULL string pointer passed in");
		return false;
	}

	if (*nrrdPath == '\0') {
		Debug("LoadNrrdVolume: string with no length passed in");
		return false;
	}

	std::string nrrdPathStr(nrrdPath);

	sCurrentAPI->LoadNrrdImage(nrrdPathStr);
	return true; // todo return value from the underlying call
}



extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ClearVolumes()
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->ClearVolumes();
}


extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetNVolumes()
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->GetNVolumes();
}


PLUGINEX(Float4) GetVolumeSpacingM()
{
	if (sCurrentAPI == NULL) {
		return ZeroFloat4();
	}

	return sCurrentAPI->GetVolumeSpacingM();
}


PLUGINEX(Float4) GetVolumeExtentsMin()
{
	if (sCurrentAPI == NULL) {
		return ZeroFloat4();
	}

	return sCurrentAPI->GetVolumeExtentsMin();
}


PLUGINEX(Float4) GetVolumeExtentsMax()
{
	if (sCurrentAPI == NULL) {
		return ZeroFloat4();
	}

	return sCurrentAPI->GetVolumeExtentsMax();
}


PLUGINEX(Float4) GetVolumeOriginM()
{
	if (sCurrentAPI == NULL) {
		return ZeroFloat4();
	}

	return sCurrentAPI->GetVolumeOriginM();
}


PLUGINEX(int) AddVolumeProp()
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->AddVolumeProp();
}


PLUGINEX(int) AddCropPlaneToVolume(int volumeId)
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->AddCropPlaneToVolume(volumeId);
}


static SafeQueue<int> sNewVolumeIndex;
PLUGINEX(void) SetVolumeIndex(int index)
{
	sNewVolumeIndex.enqueue(index);
}


PLUGINEX(int) GetNTransferFunctions()
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->GetNTransferFunctions();
}
static SafeQueue<int> sNewTransferFunctionIndex;
PLUGINEX(void) SetTransferFunctionIndex(int index)
{
	sNewTransferFunctionIndex.enqueue(index);
}


static SafeQueue<std::pair<float, float>> sNewWWWL;
PLUGINEX(void) SetVolumeWWWL(
	float windowWidth, float windowLevel)
{
	sNewWWWL.enqueue(std::make_pair(windowWidth, windowLevel));
}


static SafeQueue<float> sNewOpacityFactor;
PLUGINEX(void) SetVolumeOpacityFactor(float opacityFactor)
{
	sNewOpacityFactor.enqueue(opacityFactor);
}


static SafeQueue<float> sNewBrightnessFactor;
PLUGINEX(void) SetVolumeBrightnessFactor(float brightnessFactor)
{
	sNewBrightnessFactor.enqueue(brightnessFactor);
}


static SafeQueue<bool> sNewRenderGPU;
PLUGINEX(void) SetRenderGPU(bool gpu)
{
	sNewRenderGPU.enqueue(gpu);
}


static SafeQueue<bool> sNewRenderComposite;
PLUGINEX(void) SetRenderComposite(bool composite)
{
	sNewRenderComposite.enqueue(composite);
}


static SafeQueue<bool> sNewTargetFramerateOn;
PLUGINEX(void) SetTargetFrameRateOn(bool targetOn)
{
	sNewTargetFramerateOn.enqueue(targetOn);
}

static SafeQueue<int> sNewTargetFramerateFps;
PLUGINEX(void) SetTargetFrameRateFps(int targetFps)
{
	sNewTargetFramerateFps.enqueue(targetFps);
}


PLUGINEX(int) AddMPR(int volumeId)
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->AddMPR(volumeId);
}

// Add a primitive shape to the scene
PLUGINEX(int) AddShapePrimitive(
	int shapeType,
	Float4 &rgbaColour,
	bool wireframe)
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return (sCurrentAPI->AddShapePrimitive(
		shapeType,
		rgbaColour,
		wireframe));
}

// --------------------------------------------------------------------------
// Lighting methods

PLUGINEX(int) AddLight()
{
	if (sCurrentAPI == NULL) {
		return -1;
	}

	return sCurrentAPI->AddLight();
}

PLUGINEX(void) SetLightingOn(
	bool lightingOn)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->SetLightingOn(
		lightingOn);
}

PLUGINEX(void) SetLightColor(
	int id,
	LightColorType lightingType,
	Float4 &rgbColor)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->SetLightColor(
		id,
		lightingType,
		rgbColor);
}

// Set light intensity 
PLUGINEX(void) SetLightIntensity(
	int id,
	float intensity)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->SetLightIntensity(
		id,
		intensity);
}


// --------------------------------------------------------------------------
// Volume lighting methods

PLUGINEX(void) SetVolumeLighting(
	VolumeLightType volumeLightType,
	float lightValue)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->SetVolumeLighting(
		volumeLightType,
		lightValue);
}


// --------------------------------------------------------------------------
// General Prop methods

// remove the shape primitve with the given index
PLUGINEX(void) RemoveProp3D(
	int id)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	sCurrentAPI->RemoveProp3D(
		id);
}


static SafeQueue<std::pair<int, Float16> > sPropTransformsWorldM;
PLUGINEX(void) SetProp3DTransform(
	int id,
	Float16 &transformWorldM)
{
	sPropTransformsWorldM.enqueue(std::make_pair(id, transformWorldM));
}


static SafeQueue<std::pair<int, Float16> > sMPRTransformsVolumeM;
PLUGINEX(void) SetMPRTransform(
	int id,
	Float16 &transformVolumeM)
{
	sMPRTransformsVolumeM.enqueue(std::make_pair(id, transformVolumeM));
}


// --------------------------------------------------------------------------
// Set the camera View matrix (column major array, Open GL style)
static SafeQueue<std::array<double, 16> > sViewMatrixColMajor;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetViewMatrix(
	Float16 &view4x4ColMajor)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	std::array<double, 16> matrixColMajor;

	for (unsigned int i = 0u; i < 16; ++i) {
		matrixColMajor[i] = static_cast<double>(view4x4ColMajor.elements[i]);
	}

	sViewMatrixColMajor.enqueue(matrixColMajor);
}

// Set the camera Projection matrix (column major array, Open GL style)
static SafeQueue<std::array<double, 16> > sProjectionMatrixColMajor;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetProjectionMatrix(
	Float16 &projection4x4ColMajor)
{
	if (sCurrentAPI == NULL) {
		return;
	}

	std::array<double, 16> matrixColMajor;

	for (unsigned int i = 0u; i < 16; ++i) {
		matrixColMajor[i] = static_cast<double>(projection4x4ColMajor.elements[i]);
	}

	sProjectionMatrixColMajor.enqueue(matrixColMajor);
}

// --------------------------------------------------------------------------
// OnRenderEvent
// This will be called for GL.IssuePluginEvent script calls; eventID will
// be the integer passed to IssuePluginEvent. In this example, we just ignore
// that value.
static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
	// Unknown / unsupported graphics device type? Do nothing
	if (sCurrentAPI == NULL)
	{
		return;
	}

	while (!sPropTransformsWorldM.empty())
	{
		std::pair<int, Float16> propTransformWorldM = sPropTransformsWorldM.dequeue();
		sCurrentAPI->SetProp3DTransform(
			propTransformWorldM.first,
			propTransformWorldM.second);
	}

	// set the volume index to the latest index
	{
		int index(-1);
		bool setIndex(false);

		while (!sNewVolumeIndex.empty())
		{
			index = sNewVolumeIndex.dequeue();
			setIndex = true;
		}

		if (setIndex)
		{
			sCurrentAPI->SetVolumeIndex(index);
		}
	}

	// MPR transforms
	// If there are multiple requests to move the same MPR just use the most recent one
	{
		std::map<int, Float16> thinnedMprTransformsM;

		while (!sMPRTransformsVolumeM.empty())
		{
			std::pair<int, Float16> mprTransformVolumeM = sMPRTransformsVolumeM.dequeue();
			thinnedMprTransformsM.insert(mprTransformVolumeM);
		}

		for (auto const &mprTransformsM : thinnedMprTransformsM) {
			sCurrentAPI->SetMPRTransform( 
				mprTransformsM.first,
				mprTransformsM.second);
		}
	}

	while (!sNewTransferFunctionIndex.empty())
	{
		int transferFunctionIndex = sNewTransferFunctionIndex.dequeue();
		sCurrentAPI->SetTransferFunctionIndex(transferFunctionIndex);
	}

	while (!sNewWWWL.empty())
	{
		std::pair<float, float> wwwl = sNewWWWL.dequeue();
		sCurrentAPI->SetVolumeWWWL(wwwl.first, wwwl.second);
	}

	while (!sNewOpacityFactor.empty())
	{
		float opacityFactor = sNewOpacityFactor.dequeue();
		sCurrentAPI->SetVolumeOpactityFactor(opacityFactor);
	}

	while (!sNewBrightnessFactor.empty())
	{
		float brightnessFactor = sNewBrightnessFactor.dequeue();
		sCurrentAPI->SetVolumeBrightnessFactor(brightnessFactor);
	}

	while (!sNewRenderGPU.empty())
	{
		const bool gpu = sNewRenderGPU.dequeue();
		sCurrentAPI->SetRenderGPU(gpu);
	}

	while (!sNewRenderComposite.empty())
	{
		const bool composite = sNewRenderComposite.dequeue();
		sCurrentAPI->SetRenderComposite(composite);
	}

	while (!sNewTargetFramerateOn.empty())
	{
		const bool targetFramerateOn = sNewTargetFramerateOn.dequeue();
		sCurrentAPI->SetTargetFrameRateOn(targetFramerateOn);
	}

	while (!sNewTargetFramerateFps.empty())
	{
		const int targetFramerateFps = sNewTargetFramerateFps.dequeue();
		sCurrentAPI->SetTargetFrameRateFps(targetFramerateFps);
	}

	sCurrentAPI->UpdateVtkCameraAndRender(
		sViewMatrixColMajor.dequeue(), 
		sProjectionMatrixColMajor.dequeue());
}

// --------------------------------------------------------------------------
// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}

