#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "render.h" // Include the render functions and structures
#include "util.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAP_SIZE 16
const uint8_t MAP[MAP_SIZE * MAP_SIZE] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 3, 2, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 2, 0, 1,
	1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0, 1, 
	1, 0, 0, 0, 0, 0, 3, 2, 2, 2, 2, 2, 2, 2, 0, 1,
	1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};


#define ASSERT(cond, ...) \
    if (!cond) { \
        fprintf(stderr, __VA_ARGS__); \
        exit(1); \
    }

#define PI 3.14159265f
const float playerFOV = (PI / 2.0f);
const float maxDepth = 20.0f;

// Define other constants and structures

int main(int argc, char *argv[]) {
	ASSERT(!SDL_Init(SDL_INIT_VIDEO),
		   "SDL failed to initialize; %s\n",
		   SDL_GetError());
	State state = {
        .quit = false,
    };
	state.window =
		SDL_CreateWindow("Raycast",
						 SDL_WINDOWPOS_CENTERED_DISPLAY(0),
						 SDL_WINDOWPOS_CENTERED_DISPLAY(0),
						 SCREEN_WIDTH,
						 SCREEN_HEIGHT,
						 SDL_WINDOW_ALLOW_HIGHDPI);
	ASSERT(state.window,
		   "failed to create SDL window: %s\n",
		   SDL_GetError());

	state.renderer =
		SDL_CreateRenderer(state.window,
						   -1,
						   SDL_RENDERER_PRESENTVSYNC);
	ASSERT(state.renderer,
		   "failed to create SDL renderer: %s\n",
		   SDL_GetError());

    // Call the loadTextures function to load the wall textures
    loadTextures(state.renderer);

    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
    SDL_SetRelativeMouseMode(SDL_TRUE);

	Player player = {
        .pos = {.x =  4.0f, .y =  4.0f},
        .dir = {.x = -1.0f, .y =  0.0f},
        .plane = {.x = 0.0f, .y = 0.66f},
    };

	const float
		rotateSpeed = 0.025,
		moveSpeed = 0.05;
	
	while (!state.quit) {
		SDL_Event event;
        int mouse_xrel = 0;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
		        case SDL_QUIT:
		        	state.quit = true;
		        	break;
                case SDL_MOUSEMOTION:
                    mouse_xrel = event.motion.xrel;
                    break;
            }
		}

        const uint8_t* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_ESCAPE]) state.quit = true;
        if (mouse_xrel != 0) { // rotate
            float rotSpeed = rotateSpeed * (mouse_xrel * -0.1);
            // both camera direction and camera plane must be rotated
            Vec2F oldDir = player.dir;
            player.dir.x = player.dir.x * cosf(rotSpeed) - player.dir.y * sinf(rotSpeed);
            player.dir.y = oldDir.x     * sinf(rotSpeed) + player.dir.y * cosf(rotSpeed);

            Vec2F oldPlane = player.plane;
            player.plane.x = player.plane.x * cosf(rotSpeed) - player.plane.y * sinf(rotSpeed);
            player.plane.y = oldPlane.x     * sinf(rotSpeed) + player.plane.y * cosf(rotSpeed);
        }

        Vec2F deltaPos = {
            .x = player.dir.x * moveSpeed,
            .y = player.dir.y * moveSpeed,
        };
        if (keystate[SDL_SCANCODE_W]) { // forwards
            if (MAP[xy2index(
                        player.pos.x + deltaPos.x, 
                        player.pos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.x += deltaPos.x;
            }
            if (MAP[xy2index(
                        player.pos.x, 
                        player.pos.y + deltaPos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.y += deltaPos.y;
            }
        }
        if (keystate[SDL_SCANCODE_S]) { // backwards
            if (MAP[xy2index(
                        player.pos.x - deltaPos.x, 
                        player.pos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.x -= deltaPos.x;
            }
            if (MAP[xy2index(
                        player.pos.x, 
                        player.pos.y - deltaPos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.y -= deltaPos.y;
            }
        }
        if (keystate[SDL_SCANCODE_A]) { // strafe left
            if (MAP[xy2index(
                        player.pos.x - deltaPos.y, 
                        player.pos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.x -= deltaPos.y;
            }
            if (MAP[xy2index(
                        player.pos.x, 
                        player.pos.y - -deltaPos.x, 
                        MAP_SIZE)] == 0) {
                player.pos.y -= -deltaPos.x;
            }
        }
        if (keystate[SDL_SCANCODE_D]) { // strafe right
            if (MAP[xy2index(
                        player.pos.x - -deltaPos.y, 
                        player.pos.y, 
                        MAP_SIZE)] == 0) {
                player.pos.x -= -deltaPos.y;
            }
            if (MAP[xy2index(
                        player.pos.x, 
                        player.pos.y - deltaPos.x, 
                        MAP_SIZE)] == 0) {
                player.pos.y -= deltaPos.x;
            }
        }

        SDL_SetRenderDrawColor(state.renderer, 0x18, 0x18, 0x18, 0xFF);
        SDL_RenderClear(state.renderer);

		render(&state, &player);

		SDL_RenderPresent(state.renderer);
	}

	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	return 0;
}