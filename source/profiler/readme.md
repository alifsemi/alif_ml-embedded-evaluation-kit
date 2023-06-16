# Profiler

This is a CMake static library that exposes a C++ API for a very simple logging based profiler implementation. It
depends on the hardware abstraction layer's platform drivers to get performance measurement counters (PMU) from the
platform. It makes no assumptions about the type of data these counters might contain and therefore each individual
platform is free to implement their own flavour. It works on the principle that each counter capsule will have one, or
several, 64-bit counters which are used to maintain rolling statistics.
