#include "render.h"

void pieceDraw(Game *game, Piece *piece)
{
    SDL_Rect tetromino[NUM_ACTIVE_TILES];
    for (int i = 0; i < NUM_ACTIVE_TILES; i++)
    {
        tetromino[i] = (SDL_Rect) {.x = piece->x[i] * TILE_WIDTH, .y = piece->y[i] * TILE_HEIGHT, .w = TILE_WIDTH, .h = TILE_HEIGHT};
    }

    SDL_SetRenderDrawColor(game->renderer, piece->color[0], piece->color[1],piece->color[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(game->renderer, tetromino, NUM_ACTIVE_TILES);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void inactivePieceDraw(Game *game, InactivePieces *inactivePieces)
{
    int x, y;
    SDL_Rect toDraw = {.x = 0, .y = 0, .w = TILE_WIDTH, .h = TILE_HEIGHT};
    for (x = 0; x < GRID_WIDTH; x++)
    {
        for (y = 0; y < GRID_HEIGHT; y++)
        {
            if (inactivePieces->grid[x][y] == INACTIVE) {
                SDL_SetRenderDrawColor(game->renderer, inactivePieces->color[x][y][0], inactivePieces->color[x][y][1], inactivePieces->color[x][y][2], SDL_ALPHA_OPAQUE);
                toDraw.x = x * TILE_WIDTH;
                toDraw.y = y * TILE_HEIGHT;
                SDL_RenderFillRect(game->renderer, &toDraw);
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }
        }
    }
}
