#pragma once

#include <string>

#define NOMINMAX
#include <windows.h>

#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkObjectBase.h>
#include <vtkObjectBase.h>

/// Adapters should be singletons !!!
///
/// Even though Singletons are a difficult pattern to master AND PEOPLE ABUSE IT
// (see this thread FOR HEAVEN'S SAKE: 
//  https://stackoverflow.com/questions/86582/singleton-how-should-it-be-used
//  and for further reference:
//  https://stackoverflow.com/questions/1008019/c-singleton-design-pattern)
/// This case REQUIRES singletons, as we are creating interface adapters that require
/// resources in a real-time rendering system, i.e. without thread-safe, multi-thread at
/// best, singletons, we would waste resources and risk a plethora of memory leaks
///
/// A singleton provider class is implemented in Singleton.h
/// always use the adapters wrapped in a singleton !!!
class vtkAdapter
{
public:
	template <typename T> using getter = std::stringstream(T::*)(vtkObjectBase *);
	template <typename T> using setter = void (T::*)(vtkObjectBase *, LPCSTR);

	virtual ~vtkAdapter() { }

	inline LPCSTR GetAdaptingObject() 
	{
		return m_vtkObjectName;
	}

	virtual void SetAttribute(
		vtkObjectBase *object,
		LPCSTR propertyName,
		LPCSTR newValue) = 0;

	virtual LPCSTR GetAttribute(
		vtkObjectBase *object,
		LPCSTR propertyName) = 0;

	virtual LPCSTR GetDescriptor() const = 0;

	virtual vtkObjectBase* NewInstance() = 0;

protected:
	// The name of the VTK object (as written in the wiki) for which
	// the class acts as an adapter
	LPCSTR m_vtkObjectName;

	vtkAdapter(
		LPCSTR vtkObjectName) 
	{ 
		m_vtkObjectName = vtkObjectName;
	};
};