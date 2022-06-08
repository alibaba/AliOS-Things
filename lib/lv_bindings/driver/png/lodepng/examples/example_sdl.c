/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

/*
Compile command for Linux:
gcc lodepng.c example_sdl.c -ansi -pedantic -Wall -Wextra -lSDL -O3 -o showpng

*/

/*
LodePNG SDL example
This example displays a PNG with a checkerboard pattern to show tranparency.
It requires the SDL library to compile and run.
If multiple filenames are given to the command line, it shows all of them.
Press any key to see next image, or esc to quit.
*/

#include "lodepng.h"

#include <SDL/SDL.h>

/*shows image with SDL. Returns 1 if user wants to fully quit, 0 if user wants to see next image.*/
int show(const char* filename) {
  unsigned error;
  unsigned char* image;
  unsigned w, h, x, y;
  SDL_Surface* scr;
  SDL_Event event;
  int done;
  size_t jump = 1;

  printf("showing %s\n", filename);

  /*load the PNG in one function call*/
  error = lodepng_decode32_file(&image, &w, &h, filename);

  /*stop if there is an error*/
  if(error) {
    printf("decoder error %u: %s\n", error, lodepng_error_text(error));
    return 0;
  }

  /*avoid too large window size by downscaling large image*/

  if(w / 1024 >= jump) jump = w / 1024 + 1;
  if(h / 1024 >= jump) jump = h / 1024 + 1;

  /*init SDL*/
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Error, SDL video init failed\n");
    return 0;
  }
  scr = SDL_SetVideoMode(w / jump, h / jump, 32, SDL_HWSURFACE);
  if(!scr) {
    printf("Error, no SDL screen\n");
    return 0;
  }
  SDL_WM_SetCaption(filename, NULL); /*set window caption*/

  /*plot the pixels of the PNG file*/
  for(y = 0; y + jump - 1 < h; y += jump)
  for(x = 0; x + jump - 1 < w; x += jump) {
    int checkerColor;
    Uint32* bufp;
    Uint32 r, g, b, a;

    /*get RGBA components*/
    r = image[4 * y * w + 4 * x + 0]; /*red*/
    g = image[4 * y * w + 4 * x + 1]; /*green*/
    b = image[4 * y * w + 4 * x + 2]; /*blue*/
    a = image[4 * y * w + 4 * x + 3]; /*alpha*/

    /*make translucency visible by placing checkerboard pattern behind image*/
    checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
    r = (a * r + (255 - a) * checkerColor) / 255;
    g = (a * g + (255 - a) * checkerColor) / 255;
    b = (a * b + (255 - a) * checkerColor) / 255;

    /*give the color value to the pixel of the screenbuffer*/
    bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) / jump + (x / jump);
    *bufp = 65536 * r + 256 * g + b;
  }

  /*pause until you press escape and meanwhile redraw screen*/
  done = 0;
  while(done == 0) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) done = 2;
      else if(SDL_GetKeyState(NULL)[SDLK_ESCAPE]) done = 2;
      else if(event.type == SDL_KEYDOWN) done = 1; /*press any other key for next image*/
    }
    SDL_UpdateRect(scr, 0, 0, 0, 0); /*redraw screen*/
    SDL_Delay(5); /*pause 5 ms so it consumes less processing power*/
  }

  /*cleanup*/
  free(image);
  SDL_Quit();
  return done == 2 ? 1 : 0;
}

int main(int argc, char* argv[]) {
  int i;

  if(argc <= 1) printf("Please enter PNG file name(s) to display\n");;

  for(i = 1; i < argc; i++) {
    if(show(argv[i])) return 0;
  }
  return 0;
}
