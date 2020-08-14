# VtkToUnity Plugin Build Instructions

## Software Requirements/Pre-requisits

* **VTK**
    * See these [instructions](VtkModifications) to download and build VTK
	* CMake, Visual Studio and git are also required from there
* **Unity**
	* https://unity.com/
	* Download and install the latest version

## Build Instructions

#### Make the project file for the plugin are

* Add a folder `build` to `.\VtkToUnityPlugin` folder
* Start `CMake` 
	* Set the folder you just created as the `build folder` 
	* Set `.\VtkToUnityPlugin\source` as the `source folder`
* Click `Configure`, and when selecting the compiler choose the 64bit version.
	* There will be an error
* Set the `vtk build folder` from above as `VTK-DIR`
* Click `Configure`
	* There will be new item in red, but this is not a problem
* Click `Configure`
	* Now there should be no error
* Click `Generate`

#### Build the plugin

* Open (**as Administrator**)  `VtkToUnityPlugin.sln` in `\UnityNativeVtkTestPlugin\VtkToUnityPlugin\build`
* `Build` the plugin, in `Release` format for maximum performance


