# OBJ2BIN

Small command line helper tool for GpuPAD (https://github.com/houmain/gpupad) that parses 3D mesh from **OBJ** file. 
Serializing the data: create a unique vertices array - each unique combination of position, UV, and normal indices
in the .obj file defines a unique vertex (as for OpenGL).
Construct an index buffer that can be used directly with these unique vertices.
The result is written per attribute in the binary files: Index (uint32), Vertices/Normals/UVs float.

Tools use **rapidobj** https://github.com/guybrush77/rapidobj

## Usage

```
OBJ2BIN.exe path_to/file.obj [ascii]
```
The output will be written in the same folder:
```
file_idxs.bin
file_norm.bin
file_uv.bin
file_vert.bin
```
Using **ascii** option arrays, they would be written in ascii mode (for debugging).

## Limitations
- Only vertex, normals, and texture coordinates are supported
- Only the first object is exporting
- In case of no texture coordinates or no vertex normals, the app might crash (TODO: fix this)

## License
It is released under the GNU GPLv3. Please see LICENSE for license details.
