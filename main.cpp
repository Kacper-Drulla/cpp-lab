#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <string>

using namespace std;

#define G 400
#define PLAYER_JUMP_SPD 350.0f

// Struct declaration
typedef struct IntVector2 {
   int x;
   int y;
} IntVector2;

typedef struct EnvItem {
   Rectangle rect;
   bool isBlocking;
   Color color;   
} EnvItem;

// Class declaration
class Entity {
   public:
      int hp;
      Vector2 position;

   public:
      virtual void UpdatePosition() {}
};

class Player : public Entity {
   public:
      float speed;
      float jumpSpeed;
      bool canJump;
      Rectangle playerRect;
   
   public:
      Player(int hp, Vector2 position, float speed, float jumpSpeed) {
         this->hp = hp;
         this->position = position;
         this->speed = speed;
         this->canJump = false;
         this->playerRect = Rectangle{position.x - 20, position.y - 40, 40.0f, 40.0f};
         this->jumpSpeed = jumpSpeed;
      }
      Player() = default;

      void GetInput() {}
};

class GameResources {
   public:
      Player player;
      Camera2D camera;
      EnvItem envItems[5];

   public:
      GameResources(Player player, Camera2D camera) {
         envItems[0] = {{0, 0, 1000, 400}, false, LIGHTGRAY};
         envItems[1] = {{ 0, 400, 1000, 200 }, true, GRAY };
         envItems[2] = {{ 300, 200, 400, 10 }, true, GRAY };
         envItems[3] = {{ 250, 300, 100, 10 }, true, GRAY };
         envItems[4] = {{ 650, 300, 100, 10 }, true, GRAY };
         
         this->player = player;
         this->camera = camera;
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
         return IntVector2{ screenWidth, screenHeight };
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
         Player player(20, Vector2{ 400., 280.0 }, 300, 300);
         IntVector2 screenDim = gameSettings.GetScreenDim();

         Camera2D camera = { 0 };
         camera.target = player.position;
         camera.offset = Vector2{ (float)screenDim.x / 2.0f, (float)screenDim.y / 2.0f };
         camera.rotation = 0.0f;
         camera.zoom = 1.0f;

         GameResources gameResources(player, camera);

         while (!WindowShouldClose())
         {
            Update(GetFrameTime(), gameResources);
            Draw(gameResources);
         }
      }

   private:
      void Update(float deltaTime, GameResources& gameResources) {
         // Camera Update
         IntVector2 screenDim = gameSettings.GetScreenDim();
         static float minSpeed = 40;
         static float minEffectLength = 10;
         static float fractionSpeed = 1.0f;

         gameResources.camera.offset = Vector2{ screenDim.x / 2.0f, screenDim.y / 2.0f };
         Vector2 diff = Vector2Subtract(gameResources.player.position, gameResources.camera.target);
         float length = Vector2Length(diff);

         if (length > minEffectLength) {
            float speed = fmaxf(fractionSpeed * length, minSpeed);
            gameResources.camera.target = Vector2Add(
               gameResources.camera.target,
               Vector2Scale(diff, speed * deltaTime / length)
            );
         }

         // Player Movement
         if (IsKeyDown(KEY_A)) {
            gameResources.player.position.x -= 100 * deltaTime;
         }
         if (IsKeyDown(KEY_D)) {
            gameResources.player.position.x += 100 * deltaTime;
         }
         if (IsKeyDown(KEY_SPACE) && gameResources.player.canJump) {
            gameResources.player.speed = -gameResources.player.jumpSpeed;
            gameResources.player.canJump = false;
         }

         bool hitObstacle = false;
         for (EnvItem item : gameResources.envItems) {
            if (item.isBlocking && 
               item.rect.x <= gameResources.player.position.x &&
               item.rect.x + item.rect.width >= gameResources.player.position.x &&
               item.rect.y >= gameResources.player.position.y &&
               item.rect.y <= gameResources.player.position.y + gameResources.player.speed * deltaTime) 
            {   
               hitObstacle = true;
               gameResources.player.speed = 0.0f;
               gameResources.player.position.y = item.rect.y;
               break;
            }            
         }

         if (!hitObstacle) {
            gameResources.player.position.y += gameResources.player.speed * deltaTime;
            gameResources.player.speed += G*deltaTime;
            gameResources.player.canJump = false;
         } else {
            gameResources.player.canJump = true;
         }

         //Update Rect
         gameResources.player.playerRect.x = gameResources.player.position.x - 20;
         gameResources.player.playerRect.y = gameResources.player.position.y - 40;             
      }

      void Draw(GameResources& gameResources) {
         BeginDrawing();

         ClearBackground(RAYWHITE);

            BeginMode2D(gameResources.camera);

            for (EnvItem item : gameResources.envItems) {
               DrawRectangleRec(item.rect, item.color);
            }

            DrawRectangleRec(gameResources.player.playerRect, RED);

            DrawCircleV(gameResources.player.position, 5.0f, GOLD);

            EndMode2D();

         EndDrawing();
      }
};

int main() {
   GameSettings gameSettings(800, 450, "Window Title", 60);
   GameProcess gameProcess(gameSettings);

   gameProcess.InitGame();
   gameProcess.Run();

   return 0;
}