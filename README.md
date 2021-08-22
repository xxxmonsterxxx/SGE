# Simple Game Engine

How to use?

You could build this, but other option is to download release built version as static or shared library.
In additional to this you should to download (or include as submodule of git project) and include to your project follows libraries:
SGR (https://github.com/xxxmonsterxxx/SGR)
GLM (https://github.com/g-truc/glm)
GLFW (https://github.com/glfw/glfw)

How to build?

Each build can be multithreaded, after "cmake" command will be generated message with max number of your threads.

MacOS & Linux & Windows build is the same:

:cmake .

:make

For test of engine you can build run and follow:

:cd examplesData

:cmake .

:make