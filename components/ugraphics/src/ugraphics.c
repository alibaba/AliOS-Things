#include "ugraphics.h"
#include <ulog/ulog.h>
#ifdef AOS_COMP_SDL2

#define MAX_TEXT 100
#define MAX_IMAGE 10

SDL_Window *window;
SDL_Renderer *renderer;

TTF_Font *font;
struct { char *string; SDL_Texture *texture; } text[MAX_TEXT];
struct { char *file; SDL_Texture *texture; } image[MAX_IMAGE];

// Internal function prototypes
static void graphics_generate_text(char *string);
static void graphics_wipe_text(void);
static void graphics_draw_texture(SDL_Texture *texture, int x, int y);
#endif

#define TAG "ugraphics"

int text_count = 0;
int image_count = 0;

int ugraphics_init(int width, int height)
{
#ifdef AOS_COMP_SDL2
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        LOGE(TAG, "SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1) {
        LOGE(TAG, "IMG_Init: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) == -1) {
        LOGE(TAG, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetWindowTitle(window, "ugraphics");
    return 0;
#endif
}

int ugraphics_load_font(const char *filename, int size)
{
#ifdef AOS_COMP_SDL2
    if (TTF_Init() == -1) {
        LOGE(TAG, "TTF_Init: %s\n", TTF_GetError());
        return -1;
    }

    font = TTF_OpenFont(filename, size);
    if (!font) {
        LOGE(TAG, "TTF_OpenFont: %s\n", TTF_GetError());
        return -1;
    }
    return 0;
#endif
}

void ugraphics_set_font_style(int style)
{
#ifdef AOS_COMP_SDL2
    if (font)
        TTF_SetFontStyle(font, style);
    else
        LOGE(TAG, "TTF_SeetFontStyle fail\n");
#endif
}

// Delete all generated text forcing them to be regenerated next frame
void graphics_wipe_text(void)
{
#ifdef AOS_COMP_SDL2
    for (int i = 0; i < text_count; i++) {
        if (text[i].string != NULL)
            free(text[i].string);
        SDL_DestroyTexture(text[i].texture);
    }
    text_count = 0;
#endif
}

void ugraphics_quit(void)
{
#ifdef AOS_COMP_SDL2
    if (font) {
        TTF_CloseFont(font);
        TTF_Quit();
    }
    graphics_wipe_text();
    graphics_wipe_image();
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
#endif
}

void ugraphics_flip(void)
{
#ifdef AOS_COMP_SDL2
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
    graphics_wipe_text();
    graphics_wipe_image();
#endif
}

void ugraphics_clear(void)
{
#ifdef AOS_COMP_SDL2
    SDL_RenderClear(renderer);
#endif
}

void ugraphics_set_color(unsigned int color)
{
#ifdef AOS_COMP_SDL2
    SDL_SetRenderDrawColor(renderer, color >> 24,
                             color >> 16, color >> 8, color);
#endif
}

static void graphics_generate_text(char *string)
{
#ifdef AOS_COMP_SDL2
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, string, color);
    if (!surface) {
        LOGE(TAG, "TTF_RenderUTF8_Blended: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        LOGE(TAG, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return;
    }
    text[text_count].string = strdup(string);
    // text[text_count].string = string;
    text[text_count].texture = texture;
    text_count++;
#endif
}

void graphics_wipe_image(void)
{
#ifdef AOS_COMP_SDL2
    for (int i = 0; i < image_count; i++)
        SDL_DestroyTexture(image[i].texture);

    image_count = 0;
#endif
}

void ugraphics_draw_rect(int x, int y, int w, int h)
{
#ifdef AOS_COMP_SDL2
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);
#endif
}

void ugraphics_fill_rect(int x, int y, int w, int h)
{
#ifdef AOS_COMP_SDL2
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
#endif
}

void ugraphics_draw_line(int x1, int y1, int x2, int y2)
{
#ifdef AOS_COMP_SDL2
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
#endif
}

#ifdef AOS_COMP_SDL2
static void graphics_draw_texture(SDL_Texture *texture, int x, int y)
{
    SDL_Rect drect = { x, y, 0, 0 };
    SDL_QueryTexture(texture, NULL, NULL, &drect.w, &drect.h);
    printf("drect.w: %d, drect.h: %d\n", drect.w, drect.h);
    SDL_RenderCopy(renderer, texture, NULL, &drect);
}
#endif

void ugraphics_draw_string(char *string, int x, int y)
{
#ifdef AOS_COMP_SDL2
    if (strcmp(string, "") == 0)
        return;
    for (int i = 0; i < text_count; i++) {
        if (strcmp(string, text[i].string) != 0)
            continue;
        graphics_draw_texture(text[i].texture, x, y);
        return;
    }
    /*If a texture for the string doesn't exist create it*/
    graphics_generate_text(string);
    graphics_draw_texture(text[text_count - 1].texture, x, y);
#endif
}

int ugraphics_string_width(char *string)
{
#ifdef AOS_COMP_SDL2
    if (strcmp(string, "") == 0)
        return 0;
    for (int i = 0; i < text_count; i++) {
        if (strcmp(string, text[i].string) != 0)
            continue;
        int width;
        SDL_QueryTexture(text[i].texture, NULL, NULL, &width, NULL);
        return width;
    }
#endif
    return 0;
}

static void graphics_generate_image(char *file)
{
#ifdef AOS_COMP_SDL2
    SDL_Texture *image_texture = IMG_LoadTexture(renderer, file);
    if (!image_texture) {
        LOGE(TAG, "Couldn't load %s\n", file);
        return -1;
    }

    image[image_count].file = file;
    image[image_count].texture = image_texture;
    image_count++;
#endif
}

int ugraphics_draw_image(const char *file,  int x, int y)
{
#ifdef AOS_COMP_SDL2
    SDL_Rect drect = { x, y, 0, 0 };
    if (strcmp(file, "") == 0) {
        LOG("[%s]file is null\n");
        return;
    }
    for (int i = 0; i < image_count; i++) {
        if (strcmp(file, image[i].file) != 0)
            continue;
        graphics_draw_texture(image[i].texture, x, y);
        return 0;
    }
    graphics_generate_image(file);
    graphics_draw_texture(image[image_count - 1].texture, x, y);
#endif
    return 0;
}

int ugraphics_save_image(char *buffer, int32_t len, const char *path)
{
    if (!buffer) {
        LOGE(TAG, "buffer is null\n");
        return -1;
    }
#ifdef AOS_COMP_SDL2
    FILE *file;

    file = fopen(path, "wb");

    if (file == NULL) {
        LOGE(TAG, "open file %s fail,\n", path);
        return -1;
    }

    if (fwrite(buffer, len, 1, file) < 1) {
        LOGE(TAG, "write file buf fail\n");
        return -1;
    }
    fclose(file);
#endif
    return 0;
}

