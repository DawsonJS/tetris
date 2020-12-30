#include "tetris.h"


void pieceInit(Game *game, Piece *piece)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        piece->color[i] = rand() % 256;
    }
    piece->pieceNum = rand() % 7;
    for (i = 0; i < 4; i++)
    {
        piece->tetromino[i] = game->tetrominoes[piece->pieceNum][i];
    }
    if (game->time <= 0) piece->pieceMoved = 0;
}

void pieceDraw(Game *game, Piece *piece)
{

    SDL_SetRenderDrawColor(game->renderer, piece->color[0], piece->color[1],piece->color[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(game->renderer, piece->tetromino, 4);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void pieceMove(Game *game, Piece *piece, InactivePieces *inactivePieces, enum DIRECTION dir)
{
    switch(dir)
    {
        case RIGHT: // Right
            if (!(pieceCollide(game, piece, inactivePieces, dir))) {
                piece->tetromino[0].x += (SPEED * TILE_WIDTH);
                piece->tetromino[1].x += (SPEED * TILE_WIDTH);
                piece->tetromino[2].x += (SPEED * TILE_WIDTH);
                piece->tetromino[3].x += (SPEED * TILE_WIDTH);
            }
            break;
        case LEFT: // Left
            if (!(pieceCollide(game, piece, inactivePieces, dir))) { 
                piece->tetromino[0].x -= (SPEED * TILE_WIDTH);
                piece->tetromino[1].x -= (SPEED * TILE_WIDTH);
                piece->tetromino[2].x -= (SPEED * TILE_WIDTH);
                piece->tetromino[3].x -= (SPEED * TILE_WIDTH);
            }
            break;
        case ACCELERATED_DOWN: // Accelerate Down
            if (!(pieceCollide(game, piece, inactivePieces, dir))) {
                piece->tetromino[0].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[1].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[2].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[3].y += (SPEED * TILE_HEIGHT);
                piece->pieceMoved = game->time;
            }
            break;
        case DOWN:
        default:
            if (!(pieceCollide(game, piece, inactivePieces, dir)) && (piece->pieceMoved + 1000 <= game->time)) {
                piece->tetromino[0].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[1].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[2].y += (SPEED * TILE_HEIGHT);
                piece->tetromino[3].y += (SPEED * TILE_HEIGHT);
                piece->pieceMoved = game->time; 
            }
            else if ((pieceCollide(game, piece, inactivePieces, dir)) && (piece->pieceMoved + 1000 <= game->time)) {
                pieceDeactivate(game, piece, inactivePieces);
                unsigned int rowsCleared = inactivePieceClearRows(game, inactivePieces);
                gameScoreUpdate(game, rowsCleared);
                pieceInit(game, piece);
            }
            break;
    }
}

int pieceCollide(Game *game, Piece *piece, InactivePieces *inactivePieces, enum DIRECTION dir)
{
    int i, j, x[4], y[4];
    for (i = 0; i < 4; i++)
    {
        x[i] = (piece->tetromino[i].x / TILE_WIDTH);
        y[i] = (piece->tetromino[i].y / TILE_HEIGHT);
    }

    switch (dir)
    {
        case RIGHT:
            for (j = 0; j < 4; j++)
            {
                if (piece->tetromino[j].x + TILE_WIDTH >= game->w) return 1;
                else if (inactivePieces->grid[x[j] + 1][y[j]]) return 1;
            }
            break;
        case LEFT:
            for (j = 0; j < 4; j++)
            {
                if (piece->tetromino[j].x == 0) return 1;
                else if (inactivePieces->grid[x[j] - 1][y[j]]) return 1;
            }
            break;
        case ACCELERATED_DOWN:
        case DOWN:
        default:
            for (j = 0; j < 4; j++)
            {
                if (piece->tetromino[j].y + TILE_HEIGHT == game->h) return 1;
                else if (inactivePieces->grid[x[j]][y[j] + 1]) return 1;
            }
            break;
    }
    return 0;
}

void pieceRotate(Game *game, Piece *piece, InactivePieces *inactivePieces)
{
    int i;
    int xPrime[4], yPrime[4];
    int originPiece;
    switch(piece->pieceNum)
    {
        case 0:
        case 2:
        case 5:
            originPiece = 1;
            break;
        case 1:
        case 4:
        case 6:
            originPiece = 2;
            break;
        case 3:
        default:
            return;
    }
    for (i = 0; i < 4; i++)
    {
        xPrime[i] = piece->tetromino[originPiece].x - (piece->tetromino[i].y - piece->tetromino[originPiece].y);
        yPrime[i] = piece->tetromino[originPiece].y + (piece->tetromino[i].x - piece->tetromino[originPiece].x);
        if ((inactivePieces->grid[xPrime[i] / TILE_WIDTH][yPrime[i] / TILE_HEIGHT]) && 
                (inactivePieces->grid[xPrime[i] / TILE_WIDTH][yPrime[i] / TILE_HEIGHT]))
        {
            return;
        }
    }
    for (i = 0; i < 4; i++)
    {
        piece->tetromino[i].x = xPrime[i];
        piece->tetromino[i].y = yPrime[i];
    }
}

void pieceDeactivate(Game *game, Piece *piece, InactivePieces *inactivePieces)
{
    
    int i, j, x, y; 
    for (i = 0; i < 4; i++)
    {
        x = (piece->tetromino[i].x / TILE_WIDTH);
        y = (piece->tetromino[i].y / TILE_HEIGHT);
        inactivePieces->grid[x][y] = 1;
        for (j = 0; j < 3; j++)
        {
            inactivePieces->color[x][y][j] = piece->color[j];
        } 
    }
}

void inactivePieceDraw(Game *game, InactivePieces *inactivePieces)
{
    int x, y;
    SDL_Rect toDraw = {.x = 0, .y = 0, .w = TILE_WIDTH, .h = TILE_HEIGHT};
    for (x = 0; x < 10; x++)
    {
        for (y = 0; y < 20; y++)
        {
            if (inactivePieces->grid[x][y]) {
                SDL_SetRenderDrawColor(game->renderer, inactivePieces->color[x][y][0], inactivePieces->color[x][y][1], inactivePieces->color[x][y][2], SDL_ALPHA_OPAQUE);
                toDraw.x = x * TILE_WIDTH;
                toDraw.y = y * TILE_HEIGHT;
                SDL_RenderFillRect(game->renderer, &toDraw);
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }
        }
    }
}

int inactivePieceCheck(Game *game, InactivePieces *inactivePieces) 
{
    int x, y;
    for (y = 0; y < 20; y++)
    {
        int line = 1;
        for (x = 0; x < 10; x++)
        {
            line &= inactivePieces->grid[x][y];
        }
        if (line) return y;
    }
    return -1;
}

int inactivePieceClearRow(Game *game, InactivePieces *inactivePieces, int row)
{
    int x, j;
    for (x = 0; x < 10; x++)
    {
        inactivePieces->grid[x][row] = 0;
        for (j = 0; j < 3; j++)
        {
            inactivePieces->color[x][row][j] = 0;
        }
    }
    return row;
}

int inactivePieceCheckRowEmpty(Game *game, InactivePieces *inactivePieces, int row)
{
    int x;
    for (x = 0; x < 10; x++)
    {
        if (inactivePieces->grid[x][row] != 0) return 0;
    }
    return 1;
}

void inactivePieceShiftRows(Game *game, InactivePieces *inactivePieces, int initialRow)
{
    int row, x, j;
    for (row = initialRow; inactivePieceCheckRowEmpty(game, inactivePieces, row - 1) != 1; row -= 1)
    {
        if (inactivePieceCheckRowEmpty(game, inactivePieces, row) != 1) break;
        for (x = 0; x < 10; x++)
        {
            inactivePieces->grid[x][row] = inactivePieces->grid[x][row - 1];
            for (j = 0; j < 3; j++)
            {
                inactivePieces->color[x][row][j] = inactivePieces->color[x][row - 1][j];
            }
        }
        inactivePieceClearRow(game, inactivePieces, row - 1);
    }
}

int inactivePieceClearRows(Game *game, InactivePieces *inactivePieces)
{
    int rowsCleared = 0;
    int row = inactivePieceCheck(game, inactivePieces);
    while (row != -1)
    {
        rowsCleared += 1;
        inactivePieceShiftRows(game, inactivePieces, inactivePieceClearRow(game, inactivePieces, row));
        row = inactivePieceCheck(game, inactivePieces);
    }
    return rowsCleared;
}
