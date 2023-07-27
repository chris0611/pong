# Pong

Simple clone of Pong in C11, using raylib.

## Dependencies

- C11 compiler (`gcc`, `msvc`)
- [cmake]
- [raylib]

## Building

From the root directory of project:

1. `mkdir build && cd build`
2. `cmake ..` (This may take a while if raylib is not installed, as it will then be downloaded and compiled.)
3. `cmake --build .`

Finally, to run pong (also from root dir):

```console
$ build/pong
```

[cmake]: https://cmake.org/
[raylib]: https://www.raylib.com
