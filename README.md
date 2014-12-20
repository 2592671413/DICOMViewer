DICOMViewer
===========

GUI for Visualization of DICOM datasets using
[Carna](https://github.com/RWTHmediTEC/Carna)

---
## Contents

* [Dependencies](#1-dependencies)
* [Build instructions](#2-build-instructions)
	* [Creating Visual Studio project files](#21-creating-visual-studio-project-files)
	* [Building directly](#22-building-directly)
	* [The MediTEC-way](#24-the-meditec-way)
 
---
## 1. Dependencies

Required:

* Qt ≥ 4.8
* GLEW ≥ 1.7
* [Carna](https://github.com/RWTHmediTEC/Carna) = 2.5
* [Carna-DICOM](https://github.com/RWTHmediTEC/Carna-DICOM) = 0.1
* [TRTK](https://github.com/Haenisch/TRTK) ≥ 0.13.1
* [Eigen](http://eigen.tuxfamily.org/) ≥ 3.0.5

Optional:

* [CRA](https://github.com/RWTHmediTEC/CRA) = 1.16

---
## 2. Build instructions

The default build process requires CMake = 3.0.2.

This section explains three ways of building DICOMViewer:

1. Creating Visual Studio project files and building it from the IDE
2. Building DICOMViewer directly through CMake from command line
3. If you are a colleague from MediTEC, you can use the batch script.

Regardless which build method you pick,
first thing you need to do is to fetch the latest stable version.
If you are using git from command line,
you can simply run following command
from within the directory where you want to download the sources:

```bat
git clone https://github.com/RWTHmediTEC/DICOMViewer.git
```
    
After a few seconds there should be a new folder named `DICOMViewer`
at your current working directory.

### 2.1. Creating Visual Studio project files

First create the directory where the Visual Studio files should go to:

```bat
cd DICOMViewer

mkdir build
mkdir build\VisualStudio2010

cd build\VisualStudio2010
```
    
Then initialize the Visual Studio environment:

```bat
call "%VS100COMNTOOLS%\vsvars32.bat"
```
    
And finnaly invoke CMake like this:

```bat
cmake -G"Visual Studio 10" ..\..
```

At this point the Visual Studio project files are ready.
You can proceed by opening the solution file `DICOMViewer.sln`
that was created in `DICOMViewer\build\VisualStudio2010`.

### 2.2. Building directly

The first step is to create the directories
where the results of the building process will be stored.
You can use any names you like for the directories,
it's only important to distinguish between "debug" and "release" files:

```bat
cd DICOMViewer

mkdir build
mkdir build\debug
mkdir build\release
```

Then initialize the building enviroment.
Use the command below if you are going to use Visual Studio for compilation:

```bat
call "%VS100COMNTOOLS%\vsvars32.bat"
```
    
Now it's time to run the build process.
Lets build the "debug" version first:

```bat
cd build\debug
cmake -G"NMake Makefiles" ..\..
nmake

cd ..\nmake_release
cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..
nmake
```