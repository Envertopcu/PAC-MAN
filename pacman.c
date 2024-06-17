#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 840
#define SCREEN_HEIGHT 600
#define CELL_SIZE 40
#define MAZE_HEIGHT 15
#define MAZE_WIDTH 21
#define WALL_COLOR DARKGRAY
#define PATH_COLOR RAYWHITE
#define PELLET_COLOR BLUE
#define BACKGROUND_COLOR RAYWHITE

typedef struct {
    Vector2 position;
    Vector2 direction;
    float radius;
    Color color;
    bool active;
} Entity;

typedef struct {
    Vector2 position;
    Vector2 direction;
    float radius;
    Color color;
    int lives;
} Pacman;



bool IsWall(float x, float y);
void DrawStartScreen();
void DrawDifficultySelection();
void DrawAboutScreen();
void PauseScreen();
void DrawGameOver();
void DrawCongratulations();
void SelectMaze();
void InitGame();
void MoveEntity_pacman(Pacman* entity);
void MoveEntity(Entity* entity, float ghostSpeed) ;
void InitPacman();
void DrawLives();
void RespawnPacman();
void UpdateGame();
void DrawGame();


float ghostSpeedEasy = 3.0f;
float ghostSpeedMedium = 4.0f;
float ghostSpeedHard = 5.0f;
int(*selectedMaze)[MAZE_WIDTH];
int score = 0;


Pacman pacman;
Entity ghosts[5];

typedef enum { START_SCREEN, DIFFICULTY_SELECTION, GAME, ABOUT_SCREEN, GAME_OVER, CONGRATULATIONS_SCREEN, PAUSE_SCREEN } GameState;
GameState gameState = START_SCREEN;
int selectedDifficulty = 0;

Rectangle buttons[] = {
    { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 200, 40 },
    { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 200, 40 },
    { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 200, 40 },
    { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 160, 200, 40 },
};


int mazeEasy[MAZE_HEIGHT][MAZE_WIDTH] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
    { 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

int mazeMedium[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int mazeHard[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};




int main() {
    InitAudioDevice();
    InitGame();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman");
    SetTargetFPS(60);
    Music music = LoadMusicStream("audioimage/pacmanfever.mp3");
    PlayMusicStream(music);
    while (!WindowShouldClose()) {
        UpdateGame();
        UpdateMusicStream(music);
        DrawGame();
    }
    CloseWindow();

    return 0;
}



bool IsWall(float x, float y) {
    int cellX = x / CELL_SIZE;
    int cellY = y / CELL_SIZE;
    

    return selectedMaze[cellY][cellX] != 0 && selectedMaze[cellY][cellX] != 2;
}

void DrawStartScreen() {
    Texture2D background;
    background = LoadTexture("audioimage/start.png");
    DrawTexture(background, 0, 0, WHITE);
    DrawText("Press ENTER to continue", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to continue", 20) / 2, SCREEN_HEIGHT / 2 + 280, 20, WHITE);
}

void DrawDifficultySelection() {
    Texture2D background;
    background = LoadTexture("audioimage/start.png");
    DrawTexture(background, 0, 0, WHITE);
    DrawText("Select difficulty:", SCREEN_WIDTH / 2 - MeasureText("Select difficulty:", 20) / 2, SCREEN_HEIGHT / 2 - 70, 20, WHITE);

    for (int i = 0; i < 3; i++) {
        DrawRectangleRec(buttons[i], WHITE);
        DrawText(TextFormat("%s", (i == 0) ? "Easy" : ((i == 1) ? "Medium" : "Hard")), buttons[i].x + 10, buttons[i].y + 10, 20, BLACK);
    }

    DrawRectangleRec(buttons[3], WHITE);
    DrawText("About", buttons[3].x + 10, buttons[3].y + 10, 20, BLACK);
}

void DrawAboutScreen() {
    Texture2D aboutbackground;
    aboutbackground = LoadTexture("audioimage/about.png");
    DrawTexture(aboutbackground, 0, 0, WHITE);

}

void PauseScreen() {
    Texture2D pauseBackground = LoadTexture("audioimage/pause.png");
    DrawTexture(pauseBackground, 0, 0, WHITE);
}

void DrawGameOver() {
    Texture2D gameOverBackground = LoadTexture("audioimage/gameover.png");
    DrawTexture(gameOverBackground, 0, 0, WHITE);
}

void DrawCongratulations() {
    Texture2D congratulationsBackground = LoadTexture("audioimage/congratulations.png");
    DrawTexture(congratulationsBackground, 0, 0, WHITE);
}


void SelectMaze() {
    switch (selectedDifficulty) {
    case 0:
        selectedMaze = mazeEasy;
        break;
    case 1:
        selectedMaze = mazeMedium;
        break;
    case 2:
        selectedMaze = mazeHard;
        break;
    default:
        selectedMaze = mazeEasy;
    }
}

void InitGame() {
    SelectMaze();
    int ghostCount = 4;

    pacman.position = (Vector2){ CELL_SIZE * 1.5f, CELL_SIZE * 1.5f };
    pacman.radius = CELL_SIZE * 0.4f;
    pacman.direction = (Vector2){ 0, 0 };
    pacman.color = YELLOW;
    pacman.lives = 3;


    for (int i = 0; i < ghostCount; i++) {
        
        do {
            ghosts[i].position = (Vector2){ CELL_SIZE * (GetRandomValue(1, MAZE_WIDTH - 1) + 0.5f), CELL_SIZE * (GetRandomValue(1, MAZE_HEIGHT - 1) + 0.5f) };
        } while (IsWall(ghosts[i].position.x, ghosts[i].position.y));

        ghosts[i].radius = CELL_SIZE * 0.4f;
        ghosts[i].direction = (Vector2){ -5, 0 }; // Hayaletler başlangıçta sola doğru hareket etsin
        ghosts[i].color = RED;
        ghosts[i].active = true;
    }


    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (selectedMaze[y][x] == 0) {
                selectedMaze[y][x] = 2; 
                score++; 
            }
        }
    }
    score = -1; 

}

void MoveEntity_pacman(Pacman* entity) {
    float nextX = entity->position.x + entity->direction.x;
    float nextY = entity->position.y + entity->direction.y;


    if (IsWall(nextX - entity->radius, entity->position.y)) {
        nextX = entity->position.x; 
    }

    if (IsWall(nextX + entity->radius, entity->position.y)) {
        nextX = entity->position.x; 
    }

    if (IsWall(entity->position.x, nextY - entity->radius)) {
        nextY = entity->position.y;
    }

    if (IsWall(entity->position.x, nextY + entity->radius)) {
        nextY = entity->position.y; 
    }

    if (!IsWall(nextX, nextY)) {
        entity->position.x = nextX;
        entity->position.y = nextY;
    }
}

void MoveEntity(Entity* entity, float ghostSpeed) {
    float nextX = entity->position.x + entity->direction.x;
    float nextY = entity->position.y + entity->direction.y;
   
    if (!IsWall(nextX + pacman.radius, nextY + pacman.radius) && !IsWall(nextX - pacman.radius, nextY - pacman.radius)) {
        entity->position.x = nextX;
        entity->position.y = nextY;
    }
    else {
        bool wallFound = true;
        while (wallFound) {
            int randDir = GetRandomValue(0, 3);
            switch (randDir) {
            case 0: 
                if (!IsWall(entity->position.x + CELL_SIZE, entity->position.y)) {
                    entity->direction = (Vector2){ ghostSpeed, 0 };
                    wallFound = false;
                }
                break;
            case 1:
                if (!IsWall(entity->position.x - CELL_SIZE, entity->position.y)) {
                    entity->direction = (Vector2){ -ghostSpeed, 0 };
                    wallFound = false;
                }
                break;
            case 2: 
                if (!IsWall(entity->position.x, entity->position.y + CELL_SIZE)) {
                    entity->direction = (Vector2){ 0, ghostSpeed };
                    wallFound = false;
                }
                break;
            case 3: 
                if (!IsWall(entity->position.x, entity->position.y - CELL_SIZE)) {
                    entity->direction = (Vector2){ 0, -ghostSpeed };
                    wallFound = false;
                }
                break;
            }
        }
    }
}

void InitPacman() {
    pacman.position = (Vector2){ CELL_SIZE * 1.5f, CELL_SIZE * 1.5f };
    pacman.radius = CELL_SIZE * 0.4f;
    pacman.direction = (Vector2){ 0, 0 };
    pacman.color = YELLOW;
}

void DrawLives() {
    DrawText(TextFormat("Lives: %d", pacman.lives), SCREEN_WIDTH - 120, 10, 20, BLACK);
}



void RespawnPacman() {

    pacman.position = (Vector2){ CELL_SIZE * 1.5f, CELL_SIZE * 1.5f };
    pacman.direction = (Vector2){ 0, 0 };
}

void UpdateGame() {
    switch (gameState) {
    case START_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = DIFFICULTY_SELECTION;
        }
        break;
    case DIFFICULTY_SELECTION:
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), buttons[0])) {
                selectedDifficulty = 0;
                InitGame();
                gameState = GAME;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), buttons[1])) {
                selectedDifficulty = 1;
                InitGame();
                gameState = GAME;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), buttons[2])) {
                selectedDifficulty = 2;
                InitGame();
                gameState = GAME;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), buttons[3])) {
                gameState = ABOUT_SCREEN;
            }
        }
        break;

    case GAME:
        if (IsKeyDown(KEY_RIGHT)) {
            pacman.direction = (Vector2){ 5, 0 };
        }
        else if (IsKeyDown(KEY_LEFT)) {
            pacman.direction = (Vector2){ -5, 0 };
        }
        else if (IsKeyDown(KEY_DOWN)) {
            pacman.direction = (Vector2){ 0, 5 };
        }
        else if (IsKeyDown(KEY_UP)) {
            pacman.direction = (Vector2){ 0, -5 };
        }
        MoveEntity_pacman(&pacman);

        for (int i = 0; i < 5; i++) {
            if (ghosts[i].active) {
                if (selectedDifficulty == 0) {
                    MoveEntity(&ghosts[i], ghostSpeedEasy);
                }
                else if (selectedDifficulty == 1) {
                    MoveEntity(&ghosts[i], ghostSpeedMedium);
                }
                else if (selectedDifficulty == 2) {
                    MoveEntity(&ghosts[i], ghostSpeedHard);
                }
                if (CheckCollisionCircles(pacman.position, pacman.radius, ghosts[i].position, ghosts[i].radius)) {
                    pacman.lives--;
                    if (pacman.lives <= 0) {
                        gameState = GAME_OVER;
                    }
                    else {
                        RespawnPacman();
                    }
                }
            }
        }
        
        int thresholdScore;
        switch (selectedDifficulty) {
        case 0:
            thresholdScore = 144; 
            break;
        case 1:
            thresholdScore = 165;
            break;
        case 2:
            thresholdScore = 154;
            break;
        default:
            thresholdScore = 0;
        }
        if (score >= thresholdScore) {
            gameState = CONGRATULATIONS_SCREEN;
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        DrawText("Lives:", SCREEN_WIDTH - 120, 10, 20, BLACK);
        for (int i = 0; i < pacman.lives; i++) {
            DrawCircle(SCREEN_WIDTH - 60 + i * 20, 30, 7, YELLOW);
        }
        break;
    case ABOUT_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = START_SCREEN;
        }
        break;
    case GAME_OVER:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = START_SCREEN;
            RespawnPacman();
            InitGame();
        }
        break;
    case CONGRATULATIONS_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = START_SCREEN;
            RespawnPacman();
            InitGame();
        }
        break;
    case PAUSE_SCREEN:
        PauseScreen();
        break;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);

    switch (gameState) {
    case START_SCREEN:
        DrawStartScreen();
        break;
    case DIFFICULTY_SELECTION:
        DrawDifficultySelection();
        break;
    case GAME:

        for (int y = 0; y < MAZE_HEIGHT; y++) {
            for (int x = 0; x < MAZE_WIDTH; x++) {
                Rectangle cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                if (selectedMaze[y][x] == 1) {
                    DrawRectangleLinesEx(cellRect, CELL_SIZE / 2, DARKBLUE);
                }
                else {
                    DrawRectangleRec(cellRect, BLACK);
                    if (selectedMaze[y][x] == 2) {
                        DrawCircle(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 10, YELLOW);


                        if (CheckCollisionCircles(pacman.position, pacman.radius, (Vector2) { x* CELL_SIZE + CELL_SIZE / 2, y* CELL_SIZE + CELL_SIZE / 2 }, CELL_SIZE / 10)) {
                            selectedMaze[y][x] = 0; 
                            score++; 
                        }
                        if (IsKeyPressed(KEY_P)) {
                            gameState = PAUSE_SCREEN;
                        }
                    }
                }
            }
        }

        DrawCircleV(pacman.position, pacman.radius, pacman.color);

        for (int i = 0; i < 5; i++) {
            if (ghosts[i].active) {
                DrawCircleV(ghosts[i].position, ghosts[i].radius, ghosts[i].color);
            }
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        DrawText("Lives:", SCREEN_WIDTH - 120, 10, 20, BLACK);
        for (int i = 0; i < pacman.lives; i++) {
            DrawCircle(SCREEN_WIDTH - 60 + i * 20, 30, 7, YELLOW);
        }
        break;
    case ABOUT_SCREEN:
        DrawAboutScreen();
        break;
    case GAME_OVER:
        DrawGameOver();
        break;
    case  CONGRATULATIONS_SCREEN:
        DrawCongratulations();
        break;
    case PAUSE_SCREEN:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = GAME;
        }
        else if (IsKeyPressed(KEY_R)) {
            gameState = GAME;
            InitGame();
        }
        else if (IsKeyPressed(KEY_M)) {
            gameState = START_SCREEN;
        }
        break;
    }

    EndDrawing();
}


