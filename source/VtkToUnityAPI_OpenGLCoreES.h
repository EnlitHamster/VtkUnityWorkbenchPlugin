#pragma once

#include "VtkToUnityAPI.h"
#include "PlatformBase.h"

#include <ExternalVTKWidget.h>
#include <vtkProp3D.h>
#include <vtkNew.h>
#include <vtkColorTransferFunction.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkPiecewiseFunction.h>
#include <vtkTransform.h>
#include <vtkVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <memory>

#include "vtkExternalOpenGLRenderer3dh.h"
#include "Introspection/vtkIntrospection.h"

// Renderer Class Declaraion ======================================================================

class VtkToUnityAPI_OpenGLCoreES : public VtkToUnityAPI
{
public:
	typedef std::pair<int, std::array<double, 4>> TransferFunctionPoint;
	typedef std::map<
		TransferFunctionPoint::first_type,
		TransferFunctionPoint::second_type> TransferFunction;

	VtkToUnityAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	~VtkToUnityAPI_OpenGLCoreES();

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

	virtual void SetDebugLogFunction(DebugLogFunc func);

	virtual bool GetUsesReverseZ() { return false; }

	virtual bool LoadDicomVolumeFromFolder(
		const std::string &folder);
	virtual bool LoadUncMetaImage(
		const std::string &mhdPath);
	virtual bool LoadNrrdImage(
		const std::string &nrrdPath);

	virtual bool CreatePaddingMask(int paddingValue);

	virtual void ClearVolumes();
	virtual int GetNVolumes();

	virtual Float4 GetVolumeSpacingM();
	virtual Float4 GetVolumeExtentsMin();
	virtual Float4 GetVolumeExtentsMax();
	virtual Float4 GetVolumeOriginM();

	virtual void SetVolumeIndex(
		const int index);

	virtual int AddVolumeProp();

	virtual int AddCropPlaneToVolume(const int volumeId);

	virtual int GetNTransferFunctions();
	virtual int GetTransferFunctionIndex();
	virtual void SetTransferFunctionIndex(const int index);
	virtual int AddTransferFunction();
	virtual int ResetTransferFunctions();
	virtual void SetTransferFunctionPoint(
		const int transferFunctionIndex,
		const double windowFraction,
		const double red1,
		const double green1,
		const double blue1,
		const double opacity1);
	virtual void SetVolumeWWWL(const double windowWidth, const double windowLevel);
	virtual void SetVolumeOpactityFactor(const double opacityFactor);
	virtual void SetVolumeBrightnessFactor(const double brightnessFactor);

	virtual void SetRenderComposite(const bool composite);

	virtual void SetTargetFrameRateOn(const bool targetOn);
	virtual void SetTargetFrameRateFps(const int targetFps);

	virtual int AddMPR(const int existingMprId, const int flipAxis);
	virtual void SetMPRWWWL(const double windowWidth, const double windowLevel);

	/////////////////////////////////////////////
	// Generic Vtk calls


	/*
	
		//------------------------------------\\
		|   API SPECIFICATION FOR VTK ACCESS   |
		\\------------------------------------//

		||  --- Base API ---  ||
		
		Constitues the base of the API for VTK. These methods
		are the bottom access layer. All other methods are utilities
		built upon these just to allow easier interfacing from
		C#. These methods dispatch either to an adapter or to the
		introspection layer.

		[+] VtkResource_Create
				<< class name
				>> index in resource registry

			Generates a resource in VTK, registers it in the array of
			the created resources and returns to Unity the index of
			the resource in the array. Returns -1 if there was an
			error.

		[+] VtkResource_CallMethod
				<< index in resource registry
				<< method name
				<< arguments format Python-like
				<< arguments
				>> result or NULL in case of error

			Calls the specified method on the VTK object pointed to
			in the resource registry. The call is forwarded to VTK
			and its result returned in string format. If the call
			failed, NULL is returned to Unity.

		[+] VtkError_Get
				>> error

			When an error occurs, a message is stored in the buffer
			and can be accessed from Unity. This happens on request
			to avoid passing unneccessary data back. Clears the buffer
			when called.

		[+] VtkError_Occurred
				>> is there an error

			Returns whether the error buffer is set. If there has been
			an error, the buffer contains the error message. If there was
			no error, it is set to NULL.

		-- Special Notes --

		the Python-like representation of arguments follows similar
		rules to the Python/C API. In particular, the supported 
		argument types and formats are as follows:

		>> o | O	=	refers to a VTK object that is stored in the 
						resource registry
		>> s | S	=	refers to a string or character
		>> d | D	=	refers to a decimal or long value
		>> f | F	=	refers to a double or floating point value
		>> b | B	=	refers to a boolean value

		Furtermore, special additions refer to how the data needs to
		be formatted. By $ we mean any of the previous symbols except for
		object reference symbols (o and O):

		>> $#		=	where # is a decimal number, represents a tuple
						of $ type

	*/

	virtual int VtkResource_CallObject(
		LPCSTR classname,
		const Float4 &color,
		const bool wireframe);

	virtual int VtkResource_CallObject(
		LPCSTR classname);

	virtual LPCSTR VtkResource_CallMethodAsString(
		const int rid,
		LPCSTR method,
		LPCSTR format,
		const char *const *argv);

	virtual int VtkResource_CallMethodAsVtkObject(
		const int rid,
		LPCSTR method,
		LPCSTR format,
		LPCSTR classname,
		const char *const *argv);

	virtual void VtkResource_CallMethodAsVoid(
		const int rid,
		LPCSTR method,
		LPCSTR format,
		const char *const *argv);

	virtual LPCSTR VtkResource_CallMethodPipedAsString(
		const int rid,
		const int methodc,
		const int formatc,
		const char *const *methodv,
		const char *const *formatv,
		const char *const *argv);

	virtual int VtkResource_CallMethodPipedAsVtkObject(
		const int rid,
		const int methodc,
		const int formatc,
		LPCSTR classname,
		const char *const *methodv,
		const char *const *formatv,
		const char *const *argv);

	virtual void VtkResource_CallMethodPipedAsVoid(
		const int rid,
		const int methodc,
		const int formatc,
		const char *const *methodv,
		const char *const *formatv,
		const char *const *argv);

	virtual void VtkResource_Connect(
		LPCSTR connectionType,
		const int sourceRid,
		const int targetRid);

	virtual void VtkResource_AddActor(
		const int rid,
		const Float4 &color,
		const bool wireframe);

	virtual LPCSTR VtkError_Get();

	virtual bool VtkError_Occurred();

	virtual LPCSTR VtkResource_GetAttrAsString(
		const int rid,
		LPCSTR propertyName);

	virtual void VtkResource_SetAttrFromString(
		const int rid,
		LPCSTR propertyName,
		LPCSTR format,
		LPCSTR newValue);

	virtual LPCSTR VtkResource_GetDescriptor(
		const int rid);

	virtual void GetDescriptor(
		const int shapeId,
		char* retDescriptor);


	virtual int AddLight();

	virtual void SetLightingOn(
		bool lightingOn);

	virtual void SetLightColor(
		int id,
		LightColorType lightColorType,
		Float4 &rgbColor);

	virtual void SetLightIntensity(
		int id,
		float intensity);

	virtual void SetVolumeLighting(
		VolumeLightType volumeLightType,
		float lightValue);

	virtual void RemoveProp3D(
		int id);

	virtual void SetProp3DTransform(
		int id,
		Float16 transform);

	virtual void SetMPRTransform(
		const int id,
		Float16 transformVolume);

	virtual void UpdateVtkCameraAndRender(
		const std::array<double, 16> &viewMatrix,
		const std::array<double, 16> &projectionMatrix);


protected:
	void CreateResources();

	void LogToDebugLog(const DebugLogLevel level, const std::string& message);

	void AddVolume(vtkSmartPointer<vtkImageData> volumeImageData);

	bool CheckVolumeExtentSpacingOrigin(
		vtkSmartPointer<vtkImageData> volumeImageData);

	void ReverseVolumeAlongZ(
		vtkSmartPointer<vtkImageData> volumeImageData);

	void UpdateVolumeColorAndOpacity();

protected:
	UnityGfxRenderer mAPIType;

	// vtk
	vtkNew<ExternalVTKWidget> mExternalVTKWidget;
	vtkSmartPointer<vtkExternalOpenGLRenderWindow> mRenderWindow;
	vtkNew<vtkExternalOpenGLRenderer3dh> mRenderer;

	std::shared_ptr<DebugLogFunc> mDebugLog;

	int mNextActorIndex;
	// Direct access to VTK objects, as they may not be directly connected to an actor
	std::map<int, vtkObjectBase *> mNonVolumePropObjects;
	// So we have a set of actors for the non volumes, e.g. primitives and MPRs etc.
	std::map<int, vtkSmartPointer<vtkProp3D>> mNonVolumeProp3Ds;
	// Mapping the NonVolumeProps to their type string representation
	std::map<int, LPCSTR> mNonVolumePropTypes;
	// And a set of vectors of actors for the volumes
	std::map<int, std::vector<vtkSmartPointer<vtkProp3D>>> mVolumeProp3Ds;
	// And a set of lights
	std::map<int, vtkSmartPointer<vtkLight>> mLights;

	// Volume data to render
	std::vector<vtkSmartPointer<vtkImageData>> mVolumeDataVector;
	vtkSmartPointer<vtkImageData> mCurrentVolumeData;
	int mCurrentVolumeIndex;

	vtkSmartPointer<vtkImageData> mVolumeMask;

	// Synthetic volume to fall back on to rendering
	vtkNew<vtkImageData> mSyntheticVolumeData;

	// We also require one volume map per volume prop - so these are now a vector too
	std::map<int, std::vector<vtkSmartPointer<vtkGPUVolumeRayCastMapper>>> mVolumeMappers;
	vtkNew<vtkColorTransferFunction> mVolumeColor;
	vtkNew<vtkPiecewiseFunction> mVolumeOpacity;
	vtkNew<vtkVolumeProperty> mVolumeProperty;

	std::vector<TransferFunction> mTransferFunctions;

	// Each MPR plane requires its transform, and color as well as the image actor.
	// the latter seems to be required in order to get the image to update correctly
	std::map<int, vtkSmartPointer<vtkImageReslice> > mReslice; 
	std::map<int, vtkSmartPointer<vtkTransform> > mResliceTransforms;
	vtkNew<vtkLookupTable> mResliceLookupTable;
	std::map<int, vtkSmartPointer<vtkImageMapToColors> > mResliceColors;

	std::array<int, 6> mVolumeExtent;
	std::array<int, 3> mVolumeExtentMin;
	std::array<int, 3> mVolumeExtentMax;
	std::array<double, 3> mVolumeSpacingM;
	std::array<double, 3> mVolumeOrigin;
	std::array<double, 3> mVolumeCentre;
	std::array<double, 3> mVolumeOriginCentredM;
	std::array<double, 3> mVolumeCroppingPoint;

	std::map<int, vtkSmartPointer<vtkPlane> > mVolumeCropPlanes;

	bool mRenderScene;

	double mWindowWidth;
	double mWindowLevel;
	double mOpacityFactor;
	double mBrightnessFactor;
	int mTransferFunctionIndex;

	// Utility methods
	virtual void VtkArgs_Prepare(
		LPCSTR format,
		const char *const *argv,
		std::vector<vtkObjectBase *>& refs,
		std::vector<LPCSTR>& vals);

	virtual void VtkArgs_PreparePiped(
		const int formatc,
		const char *const *formatv,
		const char *const *argv,
		std::vector<vtkObjectBase *>& refs,
		std::vector<LPCSTR>& vals);
};

