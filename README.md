# ***Simple Game Engine***

### *Description*
___
This project is a simple to use library for game and graphics applications development. Tasks like window creation, draw 2d/3d geometry, load for them textures and create sprite animations, using SGE become easier that witn naked Vulkan, OpenGL, DirectX etc. Using this engine you achieve fully crossplatform. To work demonstrate of  engine you can build and run example.

### *How to use?*
___

There is possibility to build library from source code, but other
option is to download release built version as static or shared
[library](https://github.com/xxxmonsterxxx/SGE/releases). It's necessary to download (or include as submodule of git project) and include to your
project follows libraries:    
- [**Simple Graphics Renderer**](https://github.com/xxxmonsterxxx/SGR)
- [**GLM**](https://github.com/g-truc/glm)
- [**Vulkan**](https://www.lunarg.com/vulkan-sdk)
- [**GLFW**](https://github.com/glfw/glfw)

Set path to `SGE-vX.X.X/include` as include path and
`SGE-vX.X.X/lib/static` or shared as external libraries
for your projects.

Manual documentation is currently unavailable. :cry:

### *How to build?*
___
#### *Environement:*
- **Linux**
  - *First step*
- **MacOS**
  - *First step*
- **Windows**
  - *First step*
  - ***WARNING:***
For engine test you required `DEBUG` version of SGR library.


Each build can be **multithreaded**, after `cmake` command will be generated message with max number of your threads.