# OBJ2BIN

Small command line helper tool for GpuPAD (https://github.com/houmain/gpupad) that parse 3D mesh from **OBJ** file. Serializing the data: create unique vertices array - each unique combination of position, UV, and normal indices in the .obj file defines a unique vertex (as for OpenGL).
Construct an index buffer can use directly with these unique vertices.

Tools use **rapidobj** https://github.com/guybrush77/rapidobj

## Usage

```
OBJ2BIN.exe path_to/file.obj [ascii]
```
Output will be wrote in the same folder:
```
file_idxs.bin
file_norm.bin
file_uv.bin
file_vert.bin
```

## Limitations
- Only vertex, normals and texture coordinates are supported
- Only first object are exporting
- In case of no texture coordinates or no vertex normals app might be crashed (TODO: fix this)

## License
It is released under the GNU GPLv3. Please see LICENSE for license details.