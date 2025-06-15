#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define WIDTH 30
#define HEIGHT 20
#define GAME_SPEED 150
#define MAX_HIGH_SCORES 5

// Color definitions
enum Colors {
    COLOR_BLACK = 0,
    COLOR_DARK_BLUE = FOREGROUND_BLUE,
    COLOR_DARK_GREEN = FOREGROUND_GREEN,
    COLOR_DARK_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
    COLOR_DARK_RED = FOREGROUND_RED,
    COLOR_DARK_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    COLOR_DARK_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
    COLOR_GRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    COLOR_DARK_GRAY = FOREGROUND_INTENSITY,
    COLOR_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    COLOR_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
    COLOR_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    COLOR_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    COLOR_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};

// Game structures
typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position body[WIDTH * HEIGHT];
    int length;
    char direction;
} Snake;

typedef struct {
    Position position;
    bool exists;
} Food;

typedef struct {
    char name[50];
    int score;
} HighScore;

// Function declarations
void SetColor(int color);
void ClearScreen();
void DrawBorder();
void DrawSnake();
void DrawFood();
void DrawInfo();
void DrawTitle();
void GenerateObstacles();
void MoveObstacle();
void LevelUp();
void ProcessInput();
void UpdateGame();
void GenerateFood();
void GameOver();
void SaveHighScores();
void LoadHighScores();
void AddHighScore();
void ShowHighScores();
void ShowInstructions();
void ShowMainMenu();
void StartGame();
void DrawGame();

// Game state
HANDLE console;
Snake snake;
Food food;
int score;
bool gameOver;
bool isPaused;
HighScore highScores[MAX_HIGH_SCORES];
int numHighScores = 0;
int level = 1;
int obstacleX[5], obstacleY[5];
int movingObstacleX = -1, movingObstacleY = -1;
int movingObstacleDir = 1; // 1 = right, -1 = left

// ================= MAIN FUNCTION =================
int main() {
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned int)time(NULL));
    LoadHighScores();
    
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursorInfo);
    
    ShowMainMenu();
    
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(console, &cursorInfo);
    return 0;
}

// ================= GAME FUNCTIONS =================
void SetColor(int color) {
    SetConsoleTextAttribute(console, color);
}

void ClearScreen() {
    COORD topLeft = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    GetConsoleScreenBufferInfo(console, &csbi);
    FillConsoleOutputCharacter(console, ' ', csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    FillConsoleOutputAttribute(console, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    SetConsoleCursorPosition(console, topLeft);
}

void GenerateObstacles() {
    for (int i = 0; i < 5; i++) {
        bool validPos = false;
        while (!validPos) {
            obstacleX[i] = rand() % WIDTH;
            obstacleY[i] = rand() % HEIGHT;
            validPos = true;
            
            // Don't spawn on snake
            for (int j = 0; j < snake.length; j++) {
                if (obstacleX[i] == snake.body[j].x && obstacleY[i] == snake.body[j].y) {
                    validPos = false;
                    break;
                }
            }
            
            // Don't spawn on food
            if (obstacleX[i] == food.position.x && obstacleY[i] == food.position.y) {
                validPos = false;
            }
        }
    }
}

void MoveObstacle() {
    if (level == 3) {
        movingObstacleX += movingObstacleDir;
        
        // Bounce off walls
        if (movingObstacleX <= 0 || movingObstacleX >= WIDTH-1) {
            movingObstacleDir *= -1;
            movingObstacleY = (movingObstacleY + 1) % HEIGHT;
        }
    }
}

void DrawBorder() {
    SetColor(level == 1 ? COLOR_BLUE : (level == 2 ? COLOR_MAGENTA : COLOR_CYAN));
    
    // Top border
    printf("\033[1;1H");
    for (int x = 0; x < WIDTH + 2; x++) printf("#");
    
    // Side borders
    for (int y = 0; y < HEIGHT; y++) {
        printf("\033[%d;1H", y + 2);
        printf("#");
        printf("\033[%d;%dH", y + 2, WIDTH + 2);
        printf("#");
    }
    
    // Bottom border
    printf("\033[%d;1H", HEIGHT + 2);
    for (int x = 0; x < WIDTH + 2; x++) printf("#");
}

void DrawSnake() {
    SetColor(level == 1 ? COLOR_GREEN : (level == 2 ? COLOR_CYAN : COLOR_YELLOW));
    printf("\033[%d;%dH", snake.body[0].y + 2, snake.body[0].x + 2);
    printf("@");
    
    for (int i = 1; i < snake.length; i++) {
        printf("\033[%d;%dH", snake.body[i].y + 2, snake.body[i].x + 2);
        printf("O");
    }
}

void DrawFood() {
    SetColor(COLOR_RED);
    printf("\033[%d;%dH", food.position.y + 2, food.position.x + 2);
    printf("$");
}

void DrawInfo() {
    SetColor(COLOR_WHITE);
    printf("\033[%d;1H", HEIGHT + 3);
    printf("Score: %d | Level: %d %s| Controls: WASD | P: Pause", 
           score, level, level == 3 ? "(MOVING X) " : "");
    
    if (isPaused) {
        SetColor(COLOR_YELLOW);
        printf("\033[%d;%dH", HEIGHT / 2 + 2, WIDTH / 2 - 4);
        printf("=== PAUSED ===");
    }
}

void DrawTitle() {
    SetColor(COLOR_GREEN);
    printf("\n");
    printf("   _____ _   _   _  _  _____ _____ \n");
    printf("  / ____| \\ | | | || |/ ____|_   _|\n");
    printf(" | (___ |  \\| | | || | |  __  | |  \n");
    printf("  \\___ \\| . ` | |__   _| | |_ | | |  \n");
    printf("  ____) | |\\  |    | | | |__| |_| |_ \n");
    printf(" |_____/|_| \\_|    |_| \\_____|_____|\n");
    printf("\n");
}

void LevelUp() {
    if (score >= 30 && level == 1) {
        level = 2;
        GenerateObstacles();
        SetColor(COLOR_YELLOW);
        printf("\033[%d;%dH", HEIGHT / 2 + 2, WIDTH / 2 - 8);
        printf("=== LEVEL 2 UNLOCKED! ===");
        Sleep(1000);
    } 
    else if (score >= 40 && level == 2) {
        level = 3;
        movingObstacleX = WIDTH/2;
        movingObstacleY = HEIGHT/2;
        SetColor(COLOR_YELLOW);
        printf("\033[%d;%dH", HEIGHT / 2 + 2, WIDTH / 2 - 10);
        printf("=== LEVEL 3 - MOVING OBSTACLE! ===");
        Sleep(1500);
    }
}

void ProcessInput() {
    if (_kbhit()) {
        char key = _getch();
        switch (tolower(key)) {
            case 'a': if (snake.direction != 'R') snake.direction = 'L'; break;
            case 'd': if (snake.direction != 'L') snake.direction = 'R'; break;
            case 'w': if (snake.direction != 'D') snake.direction = 'U'; break;
            case 's': if (snake.direction != 'U') snake.direction = 'D'; break;
            case 'p': isPaused = !isPaused; break;
            case 'x': gameOver = true; break;
        }
    }
}

void UpdateGame() {
    if (isPaused) return;
    
    MoveObstacle();
    
    // Move snake body
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    
    // Move head
    switch (snake.direction) {
        case 'L': snake.body[0].x--; break;
        case 'R': snake.body[0].x++; break;
        case 'U': snake.body[0].y--; break;
        case 'D': snake.body[0].y++; break;
    }
    
    // Wall collision
    if (snake.body[0].x < 0 || snake.body[0].x >= WIDTH ||
        snake.body[0].y < 0 || snake.body[0].y >= HEIGHT) {
        gameOver = true;
    }
    
    // Self collision
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
            gameOver = true;
        }
    }
    
    // Obstacle collision (Level 2+)
    if (level >= 2) {
        for (int i = 0; i < 5; i++) {
            if (snake.body[0].x == obstacleX[i] && snake.body[0].y == obstacleY[i]) {
                gameOver = true;
                break;
            }
        }
    }
    
    // Moving obstacle collision (Level 3)
    if (level == 3 && snake.body[0].x == movingObstacleX && 
        snake.body[0].y == movingObstacleY) {
        gameOver = true;
    }
    
    // Food collection
    if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
        snake.length++;
        score += 10;
        GenerateFood();
        LevelUp();
    }
}

void GenerateFood() {
    bool validPosition = false;
    while (!validPosition) {
        food.position.x = rand() % WIDTH;
        food.position.y = rand() % HEIGHT;
        validPosition = true;
        
        // Don't spawn on snake
        for (int i = 0; i < snake.length; i++) {
            if (food.position.x == snake.body[i].x && food.position.y == snake.body[i].y) {
                validPosition = false;
                break;
            }
        }
        
        // Don't spawn on obstacles (Level 2+)
        if (level >= 2) {
            for (int i = 0; i < 5; i++) {
                if (food.position.x == obstacleX[i] && food.position.y == obstacleY[i]) {
                    validPosition = false;
                    break;
                }
            }
        }
        
        // Don't spawn on moving obstacle (Level 3)
        if (level == 3 && food.position.x == movingObstacleX && 
            food.position.y == movingObstacleY) {
            validPosition = false;
        }
    }
    food.exists = true;
}

void DrawGame() {
    // Clear play area
    SetColor(COLOR_BLACK);
    for (int y = 1; y <= HEIGHT; y++) {
        printf("\033[%d;2H", y + 1);
        for (int x = 0; x < WIDTH; x++) printf(" ");
    }
    
    // Draw obstacles (Level 2+)
    if (level >= 2) {
        SetColor(COLOR_RED);
        for (int i = 0; i < 5; i++) {
            printf("\033[%d;%dH", obstacleY[i] + 2, obstacleX[i] + 2);
            printf("X");
        }
    }
    
    // Draw moving obstacle (Level 3)
    if (level == 3) {
        SetColor(COLOR_MAGENTA);
        printf("\033[%d;%dH", movingObstacleY + 2, movingObstacleX + 2);
        printf("X");
    }
    
    DrawSnake();
    DrawFood();
    DrawInfo();
    printf("\033[%d;1H", HEIGHT + 5);
}

void LoadHighScores() {
    FILE* file = fopen("highscores.dat", "rb");
    if (file) {
        fread(&numHighScores, sizeof(int), 1, file);
        fread(highScores, sizeof(HighScore), numHighScores, file);
        fclose(file);
    }
}

void SaveHighScores() {
    FILE* file = fopen("highscores.dat", "wb");
    if (file) {
        fwrite(&numHighScores, sizeof(int), 1, file);
        fwrite(highScores, sizeof(HighScore), numHighScores, file);
        fclose(file);
    }
}

void AddHighScore() {
    if (numHighScores < MAX_HIGH_SCORES || score > highScores[MAX_HIGH_SCORES - 1].score) {
        ClearScreen();
        DrawTitle();
        SetColor(COLOR_YELLOW);
        printf("   === NEW HIGH SCORE ===\n");
        printf("   Your score: %d\n\n", score);
        
        HighScore newScore;
        newScore.score = score;
        SetColor(COLOR_WHITE);
        printf("   Enter your name: ");
        scanf("%49s", newScore.name);
        
        // Insert in correct position
        int i = numHighScores - 1;
        while (i >= 0 && score > highScores[i].score) {
            if (i + 1 < MAX_HIGH_SCORES) {
                highScores[i + 1] = highScores[i];
            }
            i--;
        }
        
        if (numHighScores < MAX_HIGH_SCORES) {
            numHighScores++;
        }
        
        highScores[i + 1] = newScore;
        SaveHighScores();
    }
}

void ShowHighScores() {
    ClearScreen();
    DrawTitle();
    SetColor(COLOR_CYAN);
    printf("   === TOP PLAYERS ===\n\n");
    
    if (numHighScores == 0) {
        SetColor(COLOR_GRAY);
        printf("   No high scores yet!\n");
    } else {
        // Gold, Silver, Bronze colors for top 3
        for (int i = 0; i < numHighScores; i++) {
            if (i == 0) SetColor(COLOR_YELLOW);        // Gold
            else if (i == 1) SetColor(COLOR_GRAY);     // Silver
            else if (i == 2) SetColor(COLOR_DARK_YELLOW); // Bronze
            else SetColor(COLOR_WHITE);
            
            printf("   %2d. %-20s %5d\n", i + 1, highScores[i].name, highScores[i].score);
            
            // Add separator after top 3
            if (i == 2) printf("   -----------------------------\n");
        }
    }
    
    SetColor(COLOR_WHITE);
    printf("\n   Press any key to continue...");
    _getch();
}

void ShowInstructions() {
    ClearScreen();
    DrawTitle();
    SetColor(COLOR_CYAN);
    printf("   === INSTRUCTIONS ===\n\n");
    SetColor(COLOR_WHITE);
    printf("   Control the snake using WASD keys:\n");
    printf("     W - Move Up\n");
    printf("     A - Move Left\n");
    printf("     S - Move Down\n");
    printf("     D - Move Right\n\n");
    printf("   Other controls:\n");
    printf("     P - Pause/Unpause\n");
    printf("     X - Quit Game\n\n");
    printf("   Avoid walls, yourself, and obstacles\n");
    printf("   Eat food ($) to grow (+10 points)\n");
    printf("   Level 2: 30+ points (static obstacles)\n");
    printf("   Level 3: 40+ points (moving obstacle)\n\n");
    SetColor(COLOR_YELLOW);
    printf("   Press any key to return to menu...");
    _getch();
}

void ShowMainMenu() {
    char choice;
    do {
        ClearScreen();
        DrawTitle();
        SetColor(COLOR_WHITE);
        printf("   1. Start Game\n");
        printf("   2. View Top Players\n");
        printf("   3. View Instructions\n");
        printf("   4. Exit\n\n");
        printf("   Enter your choice: ");
        
        choice = _getch();
        switch (choice) {
            case '1': StartGame(); break;
            case '2': ShowHighScores(); break;
            case '3': ShowInstructions(); break;
            case '4': break;
            default:
                SetColor(COLOR_RED);
                printf("\n   Invalid choice! Press any key...");
                _getch();
        }
    } while (choice != '4');
}

void StartGame() {
    // Initialize game
    gameOver = false;
    isPaused = false;
    score = 0;
    level = 1;
    movingObstacleX = -1;
    movingObstacleY = -1;
    
    // Initialize snake
    snake.length = 3;
    snake.direction = 'R';
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = WIDTH / 2 - i;
        snake.body[i].y = HEIGHT / 2;
    }
    
    GenerateFood();
    ClearScreen();
    DrawBorder();
    
    // Main game loop
    while (!gameOver) {
        DrawGame();
        ProcessInput();
        if (!isPaused) UpdateGame();
        Sleep(GAME_SPEED - (level * 20)); // Faster in higher levels
    }
    
    GameOver();
}

void GameOver() {
    ClearScreen();
    DrawTitle();
    SetColor(COLOR_RED);
    printf("   === GAME OVER ===\n\n");
    SetColor(COLOR_WHITE);
    printf("   Final Score: %d\n", score);
    printf("   Level Reached: %d\n\n", level);
    
    AddHighScore();
    SetColor(COLOR_YELLOW);
    printf("   Press any key to return to menu...");
    _getch();
}