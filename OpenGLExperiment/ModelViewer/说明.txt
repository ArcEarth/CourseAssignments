An simple OpenGL/GLUT/C++ application support Observing vertex colored .obj model.
Support Rotation/Translation/Zoom/[Selecing models].
We try to build this as an object oriented application instead of an mass C-style application. And is still WIP.
=========================================
* GLUT is extremly out-dated, there is another version using pure Win32 API, all these source code is hosted on Github.
* I used some C++ 11 features inside the source, so you need an C++ 11 compilier (VS 11+)
* There is no specify in the requirement about "Math Library", I use <DirectMath> instead <glm.h> for better performance and a C++ friendly style.
=========================================
Model requirement:
put the corresponding obj file into app folder, should look like
ModelViewer.exe
cow_color.obj
v1_color.obj
=========================================
Usage : 

Press '1'/'2' to switch different model (1 for cow_color.obj, 2 for v1_color.obj)

Hold 'Z'/'X' to zoom out/in
Hold Up/Down/Left/Right to translate the model

Hold <Left Mouse Button> to rotate the model

Press 'P' to swith to perspective camera
Press 'O' to swith to orithographic camera
=========================================
Technique Used for optimzing:

Vertex/Index array
the rendering process is handled with GL array APIs, should be much mroe better than dozens of Vertex3f, but I'm not sure about weather the vertex/index array is buffered inside GPU, or must be transfered between CPU/GPU every frame.
Display List
This is technique is easier to use, but I don't use it here because I haven't read any performence compare between Array/DisplayList, 

SSE2 Acceleration for Vector types
This is finished by using DirectX Math labrary, this inline C++ lab provide extrme performence by using SSE2/ARM_NEON instrinctions.
=========================================
Library referenced:
openGL/GLU
DirectX Math (Included in Windows Kit, version 8, compile required Visual C++ 11/12(2012/2013)
DirectX Toolkit :: SimpleMath (http://directxtk.codeplex.com/, it's an inline lib, and the header is included inside the package.)
STL (C++11 support required)

All of these should ne installed along with Visual Studio 2012/2013 (Desktop Express)