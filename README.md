> IMPORTANT \
> this app is very experimental. I haven't tested this app much at all. there
will be bugs, and data lost may happen. so use the app at your own risk

# How to build

## Windows
You will need mingw-w64 installed

Build the makefile generator

```
g++ makefilegen.cpp -o makefilegen
```

Run the makefile generator

```
makefilegen
```

Run make

```
mingw32-make
```

## Web
You will need emscripten installed

The same as building for windows, but replace

```
makefilegen
```

with

```
makefilegen web
```

A `clickclick.js` will appear in `build/web/bin/`. you will need a index.html to
run it tho, which you can find in the `html/` folder

# LICENSE
Click click test is under the MIT license. The license file can be found at [LICENSE](LICENSE)

# THIRD PARTY LICENSES
## GLFW
the [zlib/libpng license](https://www.glfw.org/license.html)

Copyright (c) 2002-2006 Marcus Geelnard
Copyright (c) 2006-2019 Camilla Löwy

The license file is located at [lib/glfw-3.3.8/LICENSE.md](lib/glfw-3.3.8/LICENSE.md)

## GLAD
BSD 3-Clause License

Copyright (c) 2018, Salesforce
All rights reserved.

The license file is located at [lib/glad/LICENSE](lib/glad/LICENSE)

## Stb_image and Stb_image_write
Public Domain [www.unlicense.org](https://www.unlicense.org)

The license file is located at [lib/stb/LICENSE](lib/stb/LICENSE)
