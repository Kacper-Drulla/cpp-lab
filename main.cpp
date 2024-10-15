#include "raylib.h"
#include <iostream>
#include <string>

using namespace std;

// Struct declaration
typedef struct IntVector2 {
    int x;
    int y;
}IntVector2;

// Class declaration
class Entity {
    public:
     int hp;
     Vector2 position;
    
    public:
     virtual void UpdatePosition() {}
};

class Player : Entity {
    public:
     Player(int hp, Vector2 position) {
        this->hp = hp;
        this->position = position;
     }
     void GetInput() {

     }
};

class GameSettings {
    private:
     int screenWidth;
     int screenHeight;
     string windowTitle;
     int targetFps;
    
    public:
     GameSettings(int screenWidth, int screenHeight, string windowTitle, int targetFps) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->windowTitle = windowTitle;
        this->targetFps = targetFps;
     }
     GameSettings() = default;

     IntVector2 GetScreenDim() {
        return IntVector2{screenWidth, screenHeight};
     }

     string GetWindowTitle() {
        return windowTitle;
     }

     int GetTargetFps() {
        return targetFps;
     }   
};

class GameProcess {
    private:
     GameSettings gameSettings;

    public:
     GameProcess(GameSettings gameSettings) {
        this->gameSettings = gameSettings;
     }

     void InitGame() {
        IntVector2 screenDim = gameSettings.GetScreenDim();
        
        int screenWidth = screenDim.x;
        int screenHeight = screenDim.y;
        
        InitWindow(screenWidth, screenHeight, gameSettings.GetWindowTitle().c_str());
        SetTargetFPS(gameSettings.GetTargetFps());
     }
     
     void Run() {
        while (!WindowShouldClose()) {
            Update(GetFrameTime());
            Draw();
        }
     }
    
    private:
     void Update(float deltaTime) {}

     void Draw() {
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        DrawText("Welcome to raylib!", GetScreenWidth() / 2, GetScreenHeight() / 2, 20, LIGHTGRAY);
        
        EndDrawing();
     }

};

int main() {
    GameSettings gameSettings(600, 600, "Window Title", 60);
    GameProcess gameProcess(gameSettings);

    gameProcess.InitGame();
    gameProcess.Run();

    return 0;
}