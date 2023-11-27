[![CMake on multiple platforms](https://github.com/alexcvc/cppsl/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/alexcvc/cppsl/actions/workflows/cmake-multi-platform.yml)

Another C++ support library.
============================

This is a C++ implementation based on the previous support libraries in power automation projects, IEC61850 MMS, SV, GOOSE simulations and adapted for C++20. The libraries have been refactored. 
All classes currently supported by the C++ standard library have been removed. Other improvements such as tests and the cmake project have also been added.
The library contains classes that can be used starting with C++20 that are not currently supported by the existing ARM compilers. 


## C++ Version

C++20 

## Use in Targets Raspberry PI4 and BeagleBoard X15, BeagleBoard AI/AI64

```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app/lib
```

