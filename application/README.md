# TUD_FYP
### C21376161
TU858 Computer Science International 4th year

There are two folders in this zip,
The App folder has the precompiled .exe, library files and godot code required to run the program

The code folder has all the cpp source code required to compile the exe (You may be required to copy over the dll files from the App folder depending on your enviroment variables)

to run the godot script simply attach it to your root node, and ensure all the other files are in your root res:: directory in godot

this will generate a mesh surface out of triangles

The variables in the OS.execute are as follows with their default values:

"main.exe" the exe that creates the height map
'4' the amount of grids by itself (ie 4x4) there will be
'3' the depth of the height mesh
'3' the spread of the affected area (width)when heightmesh is modified
'6' the spread of the affected area (depth)when heightmesh is modified
'1024' the width of your final image resolution (advised to be a multiple of your amount of grids)
'1024' the height of your final image resolution (advised to be a multiple of your amount of grids)



the gcc command to compile the exe is 

g++ main.cpp -o main -L"C:/msys64/mingw64/lib" -lraylib

if running the exe outside of Godot please use
./"main" '4' '3' '3' '6' '1024' '1024' 


The program will close after 80 timesteps have been complete. This was chosen as a safe value and to ensure as few issues to arise as possible
The exe will output an png of the heightmap