# ***Simple Game Engine ver. 0.6.0***

![Image of project](https://i.postimg.cc/7h6fcBVB/sge.png)

### *Description*
___
This project is a simple to use library for game and graphics applications development. Tasks like window creation, draw 2d/3d geometry, load for them textures and create sprite animations, using SGE become easier that with naked Vulkan, OpenGL, DirectX etc. Using this engine you achieve fully crossplatform. To work demonstrate of this engine you can build and run example.

### *How to use?*
___

There is possibility to build library from source code, but other
option is to download release built version as static or shared
[library](https://github.com/xxxmonsterxxx/SGE/releases). It's necessary to download (or include as submodule of git project) and include to your
project follows libraries:    
- [**Simple Graphics Renderer**](https://github.com/xxxmonsterxxx/SGR) (Graphics engine)
- [**GLM**](https://github.com/g-truc/glm) (Math-graphics library)
- [**Vulkan**](https://www.lunarg.com/vulkan-sdk)
- [**GLFW**](https://github.com/glfw/glfw) (Crossplatform system library)

Set path to `SGE/include` as include path and
`libSGE.*` as external library
for your projects.

Manual documentation is currently unavailable. :cry:

### *Environment:*
Currently, there is no automatic script available for setting up the environment.

### *How to build?*
___

- General note:
  To build the project, you will need the appropriate version of the SGR library that matches the version of SGE you are building.

- **Linux / MacOS**
  - To build the project, you need to run the `build.sh` script.
  - Use the `-h` flag to get help on how to use the script.
  - **Debug Mode:**
    - To build the engine in debug mode: `./build.sh -d`
    - To build in debug mode and compile a test project that will automatically run after the build: `./build.sh -de`
  - **Release Mode:**
    - To build the release version: `./build.sh -r`
    - To build and then install the release version: `./build.sh -ri`
  - **Note:** You might need superuser privileges for installation.

- **Windows**
  - The project can also be built on Windows in both debug and release modes.
  - Open the CMake file in Microsoft Visual Studio, which will automatically configure the project settings.
  - You can switch between build modes using the standard methods available in Microsoft Visual Studio.
  - ***WARNING:***
    For engine testing, you are required to use the `DEBUG` version of the SGR library.



Each build can be **multithreaded**, after `cmake` command will be generated message with max number of your threads.