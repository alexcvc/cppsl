[![CMake on multiple platforms](https://github.com/alexcvc/cppsl/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/alexcvc/cppsl/actions/workflows/cmake-multi-platform.yml)

Another C++ support library.
============================


This C++ implementation is based on previous support libraries for Power Automation Projects and IEC 61850 MMS, SV and GOOSE simulations, and has been adapted for C++20. 
The libraries have undergone extensive refactoring and reworking. 
All classes currently supported by the C++ standard library have been removed. Other improvements, such as tests and the CMake project, have also been added.
The library contains classes that can be used with C++20 and that are not currently supported by existing ARM compilers. 


## C++ Version

The base compiler version is C++20, and all tests will be created with the gcc compiler in mind.

## Use in Targets Raspberry PI4 and BeagleBoard X15, BeagleBoard AI/AI64

```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app/lib
```

