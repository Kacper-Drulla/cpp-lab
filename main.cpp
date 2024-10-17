#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define G 200
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
class Player {
   public:
      int hp;

      float maxSpeed;
      float friction;
      float acceleration;
      
      //Jump related
      bool canJump;
      float jumpSpeed;
      float speedSpeed;

      Rectangle playerRect;
      Vector2 position;
      Vector2 velocity;

   public:
      Player(int hp, Vector2 position, float maxSpeed, float jumpSpeed, float acceleration, float friction) {
         this->hp = hp;
         this->position = position;
         this->maxSpeed = maxSpeed;
         this->canJump = false;
         this->playerRect = Rectangle{position.x, position.y, 20.0f, 40.0f};
         this->jumpSpeed = jumpSpeed;
         this->acceleration = acceleration;
         this->friction = friction;
         this->velocity = Vector2{0, 0};
         this->speedSpeed = maxSpeed;
      }
      Player() = default;

      void PlayerMove(float deltaTime, vector<EnvItem> envItems) {
         PlayerJump(envItems, deltaTime);

         Vector2 inputVector = Vector2{ 0,0 };
         
         if (IsKeyDown(KEY_A)) {
            inputVector.x = -1;
         }
         if (IsKeyDown(KEY_D)) {
            inputVector.x = 1;
         }

         if (inputVector.x != 0) {
            velocity.x = Lerp(velocity.x, inputVector.x * maxSpeed, acceleration);
         } else {
            velocity.x = Lerp(velocity.x, 0.0, friction);
         }
         
         position = Vector2Add(position, Vector2{velocity.x * deltaTime, velocity.y * deltaTime});
      }

      private:
         void PlayerJump(vector<EnvItem> envItems, float deltaTime) {
            if (IsKeyDown(KEY_SPACE) && canJump) {
               speedSpeed = -jumpSpeed;
               canJump = false;
            }
            
            bool hitObstacle = false;
            for (EnvItem item : envItems) {
               if (item.isBlocking && 
                  item.rect.x <= position.x &&
                  item.rect.x + item.rect.width >= position.x &&
                  item.rect.y >= position.y &&
                  item.rect.y <= position.y + speedSpeed * deltaTime) 
               {   
                  hitObstacle = true;
                  position.y = item.rect.y;
                  break;
               }            
            }

            if (!hitObstacle) {
               position.y += speedSpeed * deltaTime;
               speedSpeed += G * deltaTime;
               canJump = false;
            } else {
               canJump = true;
            }
         }


};

class GameResources {
   public:
      Player player;
      Camera2D camera;
      vector<EnvItem> envItems;

   public:
      GameResources(Player player, Camera2D camera) {
         envItems.push_back({{0, 0, 1000, 400}, false, LIGHTGRAY});
         envItems.push_back({{ 0, 400, 1000, 200 }, true, GRAY });
         envItems.push_back({{ 300, 200, 400, 10 }, true, GRAY });
         envItems.push_back({{ 250, 300, 100, 10 }, true, GRAY });
         envItems.push_back({{ 650, 300, 100, 10 }, true, GRAY });
         
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

      void Run() {
         Player player(20, Vector2{ 400., 210.0 }, 150, 220, 0.2, 0.1);
         IntVector2 screenDim = gameSettings.GetScreenDim();

         Camera2D camera = { 0 };
         camera.target = player.position;
         camera.offset = Vector2{ (float)screenDim.x / 2.0f, (float)screenDim.y / 2.0f };
         camera.rotation = 0.0f;
         camera.zoom = 1.0f;

         GameResources gameResources(player, camera);

         this->InitGame();

         while (!WindowShouldClose())
         {
            Update(GetFrameTime(), gameResources);
            Draw(gameResources);
         }
      }

   private:
      void InitGame() {
         IntVector2 screenDim = gameSettings.GetScreenDim();

         int screenWidth = screenDim.x;
         int screenHeight = screenDim.y;

         InitWindow(screenWidth, screenHeight, gameSettings.GetWindowTitle().c_str());
         SetTargetFPS(gameSettings.GetTargetFps());
      }

      void Update(float deltaTime, GameResources& gameResources) {
         this->CameraMovement(gameResources, deltaTime);

         gameResources.player.PlayerMove(deltaTime, gameResources.envItems);

         // Player Movement
         // if (IsKeyDown(KEY_A)) {
         //    gameResources.player.position.x -= 100 * deltaTime;
         // }
         // if (IsKeyDown(KEY_D)) {
         //    gameResources.player.position.x += 100 * deltaTime;
         // }
         // if (IsKeyDown(KEY_SPACE) && gameResources.player.canJump) {
         //    gameResources.player.maxSpeed = -gameResources.player.jumpSpeed;
         //    gameResources.player.canJump = false;
         // }

         // bool hitObstacle = false;
         // for (EnvItem item : gameResources.envItems) {
         //    if (item.isBlocking && 
         //       item.rect.x <= gameResources.player.position.x &&
         //       item.rect.x + item.rect.width >= gameResources.player.position.x &&
         //       item.rect.y >= gameResources.player.position.y &&
         //       item.rect.y <= gameResources.player.position.y + gameResources.player.maxSpeed * deltaTime) 
         //    {   
         //       hitObstacle = true;
         //       gameResources.player.position.y = item.rect.y;
         //       break;
         //    }            
         // }

         // if (!hitObstacle) {
         //    gameResources.player.position.y += gameResources.player.speed * deltaTime;
         //    // gameResources.player.speed += G*deltaTime;
         //    gameResources.player.canJump = false;
         // } else {
         //    gameResources.player.canJump = true;
         // }

         //Update Rect
         gameResources.player.playerRect.x = gameResources.player.position.x - 10;
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

      // Helper functions:
      void CameraMovement(GameResources& gameResources, float deltaTime) {
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
      }
};

int main() {
   GameSettings gameSettings(800, 450, "Window Title", 60);
   GameProcess gameProcess(gameSettings);

   gameProcess.Run();

   return 0;
}