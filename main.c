#include "raylib.h"
#include <stdatomic.h>
#include <stdio.h>


void reload(int *bulletCount, Sound reloadSound) {
    *bulletCount = 10;
    PlaySound(reloadSound);
}

void characterControl(Vector2 *playerPos) {
    // Character Control
    if (IsKeyDown(KEY_W)) {
        playerPos->y -= 5; // The arrow operator is equivalent to (*playerPos).y -= 5;
    }
    if (IsKeyDown(KEY_S)) {  
        playerPos->y += 5;
    }
    if (IsKeyDown(KEY_A)) {
        playerPos->x -= 5;
    }
    if (IsKeyDown(KEY_D)) {
        playerPos->x += 5;
    }
}

int main(void)
{
    InitWindow(800, 450, "Dungeon Of Jhur");
    HideCursor();
    InitAudioDevice();
    printf("Audio device ready: %d\n", IsAudioDeviceReady());
    
    Texture2D customMouse = LoadTexture("assets/sprites/cursor.png");
    Texture2D player = LoadTexture("assets/sprites/character/test.png");
    Texture2D enemy = LoadTexture("assets/sprites/enemy/Zombie Woman/test.png");

    //audio
    Sound fireSound = LoadSound("assets/sounds/gun_fire.wav");
    Sound reloadSound = LoadSound("assets/sounds/reload.wav");
    Sound bgZombie = LoadSound("assets/sounds/bg_zombie.wav");
    Sound playerDeath = LoadSound("assets/sounds/emotional_damage.wav");
    Sound enemyDeath = LoadSound("assets/sounds/enemy_death.wav");
    Sound enemySpawn = LoadSound("assets/sounds/enemy_spwan.wav");
    
    
    // Initialize positions
    Vector2 playerPos = {50, 50};
    Vector2 enemyPos = {60, 50};

    float enemySpeed = 2.5;
    
    int playerHealth = 100;
    int bulletCount = 0;

    SetTargetFPS(60);
    reload(&bulletCount, reloadSound);

    while (!WindowShouldClose())
    {   
        Vector2 mousePos = GetMousePosition();
        
        BeginDrawing();

        if (!IsAudioDeviceReady()) {
            DrawText("Audio device failed!", 100, 100, 20, RED);
        }

        if (fireSound.frameCount == 0) {
            DrawText("Sound failed to load!", 100, 130, 20, RED);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            printf("LEFT click detected\n Reloading sound...\n");
            reload(&bulletCount, reloadSound);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            printf("RIGHT click detected\n Firing...\n");
            if (bulletCount > 0) {
                bulletCount -= 1;
                PlaySound(fireSound);
            } else {
                reload(&bulletCount, reloadSound);
            }
        }
        

        ClearBackground(DARKGRAY);

        // Character Control
        characterControl(&playerPos);


        // enemy movement
        if (enemyPos.x > playerPos.x) {
            enemyPos.x -= enemySpeed;
        }
        if (enemyPos.x < playerPos.x) {
            enemyPos.x += enemySpeed;
        }
        if (enemyPos.y > playerPos.y) {
            enemyPos.y -= enemySpeed;
        }
        if (enemyPos.y < playerPos.y) {
            enemyPos.y += enemySpeed;
        }
        
        

        // Draw player
        DrawTexture(player, (int)playerPos.x, (int)playerPos.y, WHITE);
        
        // Draw enemy
        DrawTexture(enemy, (int)enemyPos.x, (int)enemyPos.y, WHITE);
        
        // Drawinng Mouse cursor is nessary just before EndDrawing() for overlapping issues
        DrawTexture(customMouse, (int)mousePos.x, (int)mousePos.y, WHITE);
    
        EndDrawing();
    }

    UnloadSound(fireSound);
    UnloadSound(reloadSound);
    UnloadSound(bgZombie);
    UnloadSound(playerDeath);
    UnloadSound(enemyDeath);
    UnloadSound(enemySpawn);
    UnloadTexture(customMouse);
    UnloadTexture(player);
    UnloadTexture(enemy);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
