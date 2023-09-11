#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAP_SIZE 16

extern const uint8_t MAP[MAP_SIZE * MAP_SIZE];

extern const float playerFOV;
extern const float maxDepth;

typedef enum { NorthSouth, EastWest } Side;

typedef struct {
    float x, y;
} Vec2F;

typedef struct {
    int x, y;
} Vec2I;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool quit;
} State;

typedef struct {
    Vec2F pos;
    Vec2F dir;
    Vec2F plane;
} Player;

typedef struct {
    uint8_t r, g, b, a;
} ColorRGBA;

extern ColorRGBA RGBA_Red;
extern ColorRGBA RGBA_Green;
extern ColorRGBA RGBA_Blue;

int xy2index(int x, int y, int w);
void render(State *state, Player *player);
void loadTextures(SDL_Renderer* renderer);
#endif