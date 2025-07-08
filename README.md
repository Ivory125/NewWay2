# NewWay2

This repository contains the client and server sources for a Metin2-based project.

## External dependencies

The Visual Studio and server builds expect a directory named `Extern` at the repository
root containing third-party headers and libraries. If you cloned the repository
without these files, extract them from `Extern.tgz` as described in
`server/README-SERVER.txt`:

```bash
tar -xzf Extern.tgz
```

Place the resulting `Extern` directory alongside the `Client` and `server`
folders so that paths such as `../../Extern/include` resolve during compilation.
