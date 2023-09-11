#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "render.h" // Include the rendering structures and constants

ColorRGBA RGBA_Red   = {.r = 0xFF, .g = 0x00, .b = 0x00, .a = 0xFF};
ColorRGBA RGBA_Green = {.r = 0x00, .g = 0xFF, .b = 0x00, .a = 0xFF};
ColorRGBA RGBA_Blue  = {.r = 0x00, .g = 0x00, .b = 0xFF, .a = 0xFF}; 

int xy2index(int x, int y, int w) {
    return y * w + x;
}

// Define the mapping between map values and texture filenames
const char* textureMapping[] = {
    NULL,   // 0 (empty)
    "pics/wood.png",   // 1 (red wall)
    "pics/redbrick.png", // 2 (blue wall)
    "pics/bluestone.png"  // 3 (green wall)
};

// Load textures based on the mapping
SDL_Texture* textures[4];


void loadTextures(SDL_Renderer* renderer) {
    for (int i = 1; i <= 3; i++) { // Start from 1, since 0 is empty
        textures[i] = IMG_LoadTexture(renderer, textureMapping[i]);
        if (!textures[i]) {
            fprintf(stderr, "Failed to load texture for map value %d\n", i);
            exit(1);
        }
    }
}

void render(State *state, Player* player) {    
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        // calculate ray position and direction
        float cameraX = 2 * x / (float)SCREEN_WIDTH - 1; // x-coordinate in camera space
        Vec2F rayDir = {
            .x = player->dir.x + player->plane.x * cameraX,
            .y = player->dir.y + player->plane.y * cameraX,
        };

        // wich box of the map we're in 
        Vec2I mapBox = {
            .x = (int)player->pos.x, 
            .y = (int)player->pos.y
        };
        // Length of ray from current position to next x- or y-side
        Vec2F sideDist = {};
        // Lenth of ray from one x- or y-side to next x- or y-side 
        Vec2F deltaDist = {
            .x = static_cast<float>(rayDir.x == 0) ? 1e30f : fabsf(1.0f / rayDir.x),
            .y = static_cast<float>(rayDir.y == 0) ? 1e30f : fabsf(1.0f / rayDir.y),
        };
        float perpWallDist;
        // What direction to step in x- or y-direction (either +1 or -1)
        Vec2I stepDir = {};

        bool hit = false; // was there a wall hit
        Side side; // was a NorthSouth or EastWest wall hit

        // calculate stepDir and initial sideDist
        if (rayDir.x < 0) {
            stepDir.x = -1;
            sideDist.x = (player->pos.x - mapBox.x) * deltaDist.x;
        } else {
            stepDir.x = 1;
            sideDist.x = (mapBox.x + 1.0f - player->pos.x) * deltaDist.x;
        }
        if (rayDir.y < 0) {
            stepDir.y = -1;
            sideDist.y = (player->pos.y - mapBox.y) * deltaDist.y;
        } else {
            stepDir.y = 1;
            sideDist.y = (mapBox.y + 1.0f - player->pos.y) * deltaDist.y;
        }

        // Perform DDA
        while (!hit) {
            // jump to next map square
            if (sideDist.x < sideDist.y) {
                sideDist.x += deltaDist.x;
                mapBox.x += stepDir.x;
                side = EastWest;
            } else {
                sideDist.y += deltaDist.y;
                mapBox.y += stepDir.y;
                side = NorthSouth;
            }
            // check if ray has hit a wall
            if (MAP[xy2index(mapBox.x, mapBox.y, MAP_SIZE)] > 0) {
                hit = true;
            }
        }

        // Calculate the distance projceted on camera direction
        // (Euclidian distance would give fisheye effect)
        switch (side) {
            case EastWest:
                perpWallDist = (sideDist.x - deltaDist.x);
                break;
            case NorthSouth:
                perpWallDist = (sideDist.y - deltaDist.y);
                break;
        }

        // Calculate height of line to draw on screen 
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2; 
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT;

        // Get the map value
        int mapValue = MAP[xy2index(mapBox.x, mapBox.y, MAP_SIZE)];

        // Render the wall segment using the corresponding texture
        if (mapValue >= 1 && mapValue <= 3) {
            SDL_Rect destRect = { x, drawStart, 1, lineHeight };
            SDL_RenderCopy(state->renderer, textures[mapValue], NULL, &destRect);
        }
    }
}
