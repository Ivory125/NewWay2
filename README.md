# NewWay2

This repository contains both the client and server projects.

## Third-party dependencies

Both projects rely on a directory named `Extern` which holds all third-party
libraries (headers and static libs). These files are not tracked in the
repository.

The server instructions (see `server/README-SERVER.txt`) reference the archive
`Extern.tgz`. Typical extraction looks like:

```sh
cd server/Srcs
# or the directory where Extern.tgz is located
tar -xzf Extern.tgz
```

After extraction you will have an `Extern` directory with `include` and `lib`
subdirectories. The Visual Studio project files expect this folder to be found
at `../../extern` relative to each project file. When building with Visual
Studio 2019 or 2022 make sure the unpacked directory is placed next to the
`Client` and `server` folders, as shown below:

```
NewWay2/
├── Client/
├── server/
├── extern/      # extracted third‑party dependencies
```

Placing the folder here ensures both the client and server projects can locate
the headers and libraries.
