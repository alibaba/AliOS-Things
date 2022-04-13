Building SDL2_image
-------------------

The easiest way to use SDL2_image with Emscripten is to use Emscripten ports (https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html#emscripten-ports) (-s USE_SDL_IMAGE=2).

If you want to build it yourself instead you can use these instructions:

Step 0 - get emscripten

Step 1 - get sdl2-emscripten
 * clone https://github.com/emscripten-ports/SDL2.git
 * follow the build instructions in SDL2/docs/README-emscripten.md (make sure to pass a --prefix to configure)
 * make install

Step 2 - get sdl_image
 * emconfigure ./configure  --disable-sdltest --with-sdl-prefix=/path/to/sdl --prefix=/path/to/install
 * (where /path/to/sdl is the path you passed as --prefix to SDL2 configure)
 * emmake make
 * make install
 
