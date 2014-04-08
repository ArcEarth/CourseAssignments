An simple OpenGL/C++ win32 application support Observing vertex colored .obj model.
We try to build this as an object oriented application instead of an mass C-style application. And is still WIP.

Model requirement:
put the corresponding obj file inside Mesh\ folder, the app folder should look like
OpenGLWindow.exe
Mesh\cow_color.obj
Mesh\v1_color.obj

Usage : 
press 'Z' to zoom out
press 'X' to zoom in
press 'P' to swith to perspective camera
press 'O' to swith to orithographic camera
Up/Down/Left/Right to move the model
Hold <Left Mouse Button> to rotate the model
use 1/2 to switch different model (1 for cow_color.obj, 2 for v1_color.obj)

Technique Used for optimzing:
the rendering process is handled with GL array APIs, should be much mroe better than dozens of Vertex3f, but I'm not sure about weather the vertex/index array is buffered inside GPU, or must be transfered between CPU/GPU every frame.

Library referenced:
openGL/GLU
Windows API
DirectX Math
STL (C++11 support required)

All of these should ne installed along with Visual Studio 2012/2013 (Desktop Express)