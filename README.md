## Multithreaded Ray Tracing
Anyone interested in making a raytracer of your own should consult [this repository by petershirley](https://github.com/petershirley/raytracinginoneweekend).

Future goals:
* Refactor all the code in headers
* Remove single threaded component
* Make config file instead of tweaking in mt.cpp
* Support for GPU rendering (Cuda, OpenCL)
* Standardize makefile
* Move file IO out of pipes and into C++ code
* Use PNG lib to write directly to PNG file rather than relying on `convert`
* Update this code more than a few times
