// includes
#include "raylib.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"

// macros
#define BOARD_HEIGHT 28
#define BOARD_WIDTH 10

// constants
const int boardHeight = BOARD_HEIGHT;
const int boardWidth = BOARD_WIDTH;

const int boardXOffset = 10;
const int boardYOffset = 10;

const int screenWidth = 500;
const int screenHeight = 550;
const int fontSizeTitle = 40;
const int fontSizeGame = 20;

const int tileSize = 18;
const int tileOutlineSize = 1;

double lastTick = 0;
double tickRate = 0.75;

Color colorTable[9] = { BLACK, PURPLE, DARKPURPLE, GREEN, WHITE, YELLOW, RED, MAROON, ORANGE};

int state = 0;

int score = 0;
int lines = 0;

int board[BOARD_WIDTH][BOARD_HEIGHT];

// Function prototypes
Color randomColor();
bool Tick();
bool detectSwapPieceCollision();
int randomColorIndex();
void addLine();
void clearBoard();
void clearLines();
void drawLines();
void drawScore();
void drawTile(int x, int y, Color c);
void generatePiece();
void placePiece();
void positionPiece();
void rotatePiece();
void sceneBoard();
void sceneTitle();
void startGame();

// structs
struct GamePiece
{
    int blocks[4][4];
    int x;
    int y;
}

piece, swapPiece;

// it's game time!
int main(void)
{
    InitWindow(screenWidth, screenHeight, "QuadBlox");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while(!WindowShouldClose())     // Detect window close button or ESC key
    {
        switch(state)
        {
            case 0:
                // start off at the title screen
                sceneTitle();
                break;
            case 1:
                sceneBoard();
                break;
                // an unknown state  was found revert to title
            default:
                state = 0;
                break;
        }
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void sceneTitle()
{
    if(IsKeyDown(KEY_ENTER))
    {
        startGame();
        state += 1;
    }
    BeginDrawing();
    ClearBackground(BLUE);
    DrawText("Welcome to QuadBlox", 50, 200, fontSizeTitle, LIGHTGRAY);
    DrawText("Press[ENTER]", 100, 250, fontSizeTitle, LIGHTGRAY);
    EndDrawing();
}

void sceneBoard()
{
    BeginDrawing();
    ClearBackground(BLUE);
    
    drawScore();
    drawLines();

    if(IsKeyPressed(KEY_UP))
    {
        rotatePiece();
    }
    if(IsKeyPressed(KEY_LEFT))
    {
        swapPiece = piece;
        swapPiece.x -= 1;
        if(!detectSwapPieceCollision())
        {
            piece = swapPiece;
        }
    }
    if(IsKeyPressed(KEY_RIGHT))
    {
        swapPiece = piece;
        swapPiece.x += 1;
        if(!detectSwapPieceCollision())
        {
            piece = swapPiece;
        }
    }
    if(Tick() || IsKeyPressed(KEY_DOWN))
    {
        swapPiece = piece;
        swapPiece.y += 1;
        if(detectSwapPieceCollision())
        {
            placePiece();
            generatePiece();
        }
        else
        {
            piece = swapPiece;
        }
    }
    // draw board outline
    for(int y = 0; y < boardHeight; y++)
    {
        // Draw outline
        drawTile(-1, y, LIGHTGRAY);
        drawTile(boardWidth, y, LIGHTGRAY);
    }
    for(int x = -1; x <= boardWidth; x++)
    {
        drawTile(x, boardHeight, LIGHTGRAY);
    }
    // draw board
    for(int x = 0; x < boardWidth; x++)
    {
        for(int y = 0; y < boardHeight; y++)
        {
            if(board[x][y]) drawTile(x, y, colorTable[board[x][y]]);
        }
    }
    // draw piece
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            if(piece.blocks[x][y])
            {
                drawTile(piece.x + x, piece.y + y, colorTable[piece.blocks[x][y]]);
            }
        }
    }
    EndDrawing();
}

void drawTile(int x, int y, Color c)
{
    int dx = boardXOffset + tileSize + x * tileSize;
    int dy = boardYOffset + y * tileSize;
    // draw black bacground of the tile allowing for an outline of varying width
    DrawRectangle(dx, dy, tileSize, tileSize, BLACK);
    // fill the rectangle with our desired color inside the black background
    DrawRectangle(dx + tileOutlineSize, dy + tileOutlineSize, tileSize - tileOutlineSize * 2, tileSize - tileOutlineSize * 2, c);
}

void clearBoard()
{
    memset(board, 0, sizeof board);
    for(int y = 1; y <= 5; y++)
    {
        addLine();
    }
    // board[4][4] = randomColorIndex();
    // board[5][5] = randomColorIndex();
}

void generatePiece()
{
    memset(piece.blocks, 0, sizeof piece.blocks);
    int blockColorIndex = randomColorIndex();
    piece.x = 5;
    piece.y = 0;
    switch(GetRandomValue(1, 7))
    {
        case 1: // straight line
            piece.blocks[1][0] = blockColorIndex;
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[1][2] = blockColorIndex;
            piece.blocks[1][3] = blockColorIndex;
            break;
        case 2: // square
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[1][2] = blockColorIndex;
            piece.blocks[2][1] = blockColorIndex;
            piece.blocks[2][2] = blockColorIndex;
            break;
        case 3: // T
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[2][1] = blockColorIndex;
            piece.blocks[3][1] = blockColorIndex;
            piece.blocks[2][2] = blockColorIndex;
            break;
        case 4: // s
            piece.blocks[2][0] = blockColorIndex;
            piece.blocks[3][0] = blockColorIndex;
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[2][1] = blockColorIndex;
            break;
        case 5: // backwards s
            piece.blocks[1][0] = blockColorIndex;
            piece.blocks[2][0] = blockColorIndex;
            piece.blocks[2][1] = blockColorIndex;
            piece.blocks[3][1] = blockColorIndex;
            break;
        case 6: // L
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[1][2] = blockColorIndex;
            piece.blocks[1][3] = blockColorIndex;
            piece.blocks[2][3] = blockColorIndex;
            break;
        case 7: // backwards L
            piece.blocks[1][1] = blockColorIndex;
            piece.blocks[1][2] = blockColorIndex;
            piece.blocks[1][3] = blockColorIndex;
            piece.blocks[2][1] = blockColorIndex;
            break;
    }
    positionPiece();
}

void positionPiece()
{
    // left shift the piece
    while(!piece.blocks[0][0] && !piece.blocks[0][1] && !piece.blocks[0][2] && !piece.blocks[0][3])
    {
        for(int i = 1; i < 4; i++)
        {
            piece.blocks[i - 1][0] = piece.blocks[i][0];
            piece.blocks[i - 1][1] = piece.blocks[i][1];
            piece.blocks[i - 1][2] = piece.blocks[i][2];
            piece.blocks[i - 1][3] = piece.blocks[i][3];
        }
        piece.blocks[3][0] = 0;
        piece.blocks[3][1] = 0;
        piece.blocks[3][2] = 0;
        piece.blocks[3][3] = 0;
    }
    // up shift the piece
    while(!piece.blocks[0][0] && !piece.blocks[1][0] && !piece.blocks[2][0] && !piece.blocks[3][0])
    {
        for(int i = 1; i < 4; i++)
        {
            piece.blocks[0][i - 1] = piece.blocks[0][i];
            piece.blocks[1][i - 1] = piece.blocks[1][i];
            piece.blocks[2][i - 1] = piece.blocks[2][i];
            piece.blocks[3][i - 1] = piece.blocks[3][i];
        }
        piece.blocks[0][3] = 0;
        piece.blocks[1][3] = 0;
        piece.blocks[2][3] = 0;
        piece.blocks[3][3] = 0;
    }
}

void rotatePiece()
{
    struct GamePiece backup = piece;
    swapPiece.x = piece.x;
    swapPiece.y = piece.y;
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            swapPiece.blocks[3 - x][y] = piece.blocks[y][x];
        }
    }
    piece = swapPiece;
    positionPiece();
    swapPiece = piece;
    if(detectSwapPieceCollision())
    {
        piece = backup;
    }
}

bool detectSwapPieceCollision()
{
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            if(swapPiece.blocks[x][y])
            {
                int tx = swapPiece.x + x;
                int ty = swapPiece.y + y;
                // check for piece on board collision
                if(board[tx][ty]) return true;
                // check for boundry collision
                if(tx >= boardWidth) return true;
                if(tx < 0) return true;
                if(ty >= boardHeight) return true;
            }
        }
    }
    return false;
}

void placePiece()
{
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            if(piece.blocks[x][y])
            {
                int tx = piece.x + x;
                int ty = piece.y + y;
                // check for piece on board collision
                board[tx][ty] = piece.blocks[x][y];
            }
        }
    }
    clearLines();
}

void startGame()
{
    clearBoard();
    generatePiece();
    lastTick = GetTime();
}

Color randomColor()
{
    return colorTable[randomColorIndex()];
}

int randomColorIndex()
{
    return GetRandomValue(1, 8);
}

void addLine()
{
    for(int y = 1; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            board[x][y - 1] = board[x][y];
        }
    }
    int xSkip = GetRandomValue(0, boardWidth - 1);
    int y = boardHeight - 1;
    for(int x = 0; x < boardWidth; x++)
    {
        if(x != xSkip)
        {
            board[x][y] = randomColorIndex();
        }
        else
        {
            board[x][y] = 0;
        }
    }
}

bool Tick()
{
    if(GetTime() - lastTick >= tickRate)
    {
        lastTick = GetTime();
        return true;
    }
    else
    {
        return false;
    }
}

void clearLines()
{
    int linesCleared = 0;
    bool clearLine;

    for(int y = boardHeight - 1; y > 0; y--)
    {
        clearLine = true;
        for(int x = 0; x < boardWidth; x++)
        {
            if(!board[x][y]) clearLine = false;
        }

        if(clearLine)
        {
            linesCleared++;
            for(int ys = y; ys > 1; ys--)
            {
                for(int xs = 0; xs < boardWidth; xs++)
                {
                    board[xs][ys] = board[xs][ys - 1];
                }
            }
            y++; // check this line again as we shifted the board down
        }
    }

    if(linesCleared == 1) score += 1;
    if(linesCleared == 2) score += 3;
    if(linesCleared == 3) score += 6;
    if(linesCleared == 4) score += 12;
    
    lines += linesCleared;

}

void drawScore()
{
    DrawText("Score", boardXOffset + tileSize * (boardWidth + 3), boardYOffset, fontSizeGame, LIGHTGRAY);
    int length = snprintf( NULL, 0, "%d", score );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", score );
    DrawText(str, boardXOffset + tileSize * (boardWidth + 3), boardYOffset + tileSize * 2, fontSizeGame, WHITE);
    free(str);    
}

void drawLines()
{
    DrawText("Lines", boardXOffset + tileSize * (boardWidth + 3), boardYOffset + tileSize * 5, fontSizeGame, LIGHTGRAY);
    int length = snprintf( NULL, 0, "%d", lines );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", lines );
    DrawText(str, boardXOffset + tileSize * (boardWidth + 3), boardYOffset + tileSize * 7, fontSizeGame, WHITE);
    free(str);
}
