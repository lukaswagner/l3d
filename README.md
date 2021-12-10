# l3d &ndash; controlling LEDs using fragment shaders

## components

- [WIP] l2d - render to 2d canvas
- [planned] l3d - render to 3d vertices
- [planned] d3l - locate LEDs in 3d space

## dependencies

- [GLFW](https://www.glfw.org/)
- [glbinding](https://glbinding.org/)
- [glm](https://github.com/g-truc/glm)
- [cli11](https://github.com/CLIUtils/CLI11)
- [logger](https://github.com/lukaswagner/logger)

You can download and build all using `cmake -P dependencies.cmake`. You may want to edit the script first, changing `configure_args` and `build_args` to the generator and build type of your liking.

## building

Windows:

```
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config RelWithDebInfo
```

Unix:

```
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build .
```
