#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef enum {NorthSouth, EastWest} Side;
typedef struct {
    float x, y;
} Vec2F;
typedef struct {
    int x, y;
} Vec2I;
typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool quit;
} State;
typedef struct {
	Vec2F pos;
	Vec2F dir;
    Vec2F plane;
} Player;
typedef struct{
    uint8_t r,g,b,a;
} ColorRGBA;

void castRays(State *state, Player* player);

#endif