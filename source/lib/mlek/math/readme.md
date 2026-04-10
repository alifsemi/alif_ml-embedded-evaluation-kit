# Math

This is a CMake static library that allows application level code and ML use case APIs to use basic mathematical
functions on scalars and vectors. This library calls the standard C/C++ math functions when being compiled for
native targets but uses Arm CMSIS-DSP functions if compiled for Arm CPU targets where DSP is available. This is done
to have an abstraction around the mathematical functions that are extensively used mostly in the pre-processing of data.
