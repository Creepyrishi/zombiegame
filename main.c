#include "raylib.h"
#include "animation.h"
#include <complex.h>
#include <stdatomic.h>
#include <stdio.h>


void reload(int *bulletCount, Sound reloadSound) {
    *bulletCount = 10;
    PlaySound(reloadSound);
}

int characterControl(Vector2 *playerPos) {
    // Character Control
    int isMoving = 0;
    if (IsKeyDown(KEY_W)) {
        playerPos->y -= 5;
        isMoving = 1;
    }
    if (IsKeyDown(KEY_S)) {  
        playerPos->y += 5;
        isMoving = 1;
    }
    if (IsKeyDown(KEY_A)) {
        playerPos->x -= 5;
        isMoving = 1;
    }
    if (IsKeyDown(KEY_D)) {
        playerPos->x += 5;
        isMoving = 1;
    }
    return isMoving;
}

int main(void)
{
    InitWindow(800, 450, "Dungeon Of Jhur");
    HideCursor();
    InitAudioDevice();
    printf("Audio device ready: %d\n", IsAudioDeviceReady());
    
    Texture2D customMouse = LoadTexture("assets/sprites/cursor.png");
    Texture2D enemy = LoadTexture("assets/sprites/enemy/Zombie Woman/test.png");

    // Player Sprites
    Texture2D playerIdleTexture = LoadTexture("assets/sprites/character/Idle_2.png");
    Texture2D playerRunTexture = LoadTexture("assets/sprites/character/Run.png");
    Texture2D playerDeadTexture = LoadTexture("assets/sprites/character/Dead.png");
    Texture2D playerShotTexture = LoadTexture("assets/sprites/character/Shot.png");
    Texture2D playerReloadTexture = LoadTexture("assets/sprites/character/Recharge.png");

    // Player sprite animation
    SpriteAnimation _playerIdleAnimation = CreateSpriteAnimation(playerIdleTexture, 9, (Rectangle[]) {
		(Rectangle){0, 0, 128, 128},
		(Rectangle){128, 0, 128, 128},
		(Rectangle){256, 0, 128, 128},
		(Rectangle){384, 0, 128, 128},
		(Rectangle){512, 0, 128, 128},
		(Rectangle){640, 0, 128, 128},
		(Rectangle){768, 0, 128, 128},
		(Rectangle){896, 0, 128, 128},
		(Rectangle){1024, 0, 128, 128},
		(Rectangle){1152, 0, 128, 128},
	}, 10);

    SpriteAnimation _playerRunAnimation = CreateSpriteAnimation(playerRunTexture, 14, (Rectangle[]) {
		(Rectangle){0, 0, 128, 128},
		(Rectangle){128, 0, 128, 128},
		(Rectangle){256, 0, 128, 128},
		(Rectangle){384, 0, 128, 128},
		(Rectangle){512, 0, 128, 128},
		(Rectangle){640, 0, 128, 128},
		(Rectangle){768, 0, 128, 128},
		(Rectangle){896, 0, 128, 128},
		(Rectangle){1024, 0, 128, 128},
		(Rectangle){1152, 0, 128, 128},
	}, 10);

    SpriteAnimation _playerReloadAnimation = CreateSpriteAnimation(playerReloadTexture, 15, (Rectangle[]) {
		(Rectangle){0, 0, 128, 128},
		(Rectangle){128, 0, 128, 128},
		(Rectangle){256, 0, 128, 128},
		(Rectangle){384, 0, 128, 128},
		(Rectangle){512, 0, 128, 128},
		(Rectangle){640, 0, 128, 128},
		(Rectangle){768, 0, 128, 128},
		(Rectangle){896, 0, 128, 128},
		(Rectangle){1024, 0, 128, 128},
		(Rectangle){1152, 0, 128, 128},
	}, 10);

    SpriteAnimation _playerShotAnimation = CreateSpriteAnimation(playerShotTexture, 25, (Rectangle[]) {
		(Rectangle){0, 0, 128, 128},
		(Rectangle){128, 0, 128, 128},
		(Rectangle){256, 0, 128, 128},
		(Rectangle){384, 0, 128, 128},
	}, 4);

    SpriteAnimation _playerDeadAnimation = CreateSpriteAnimation(playerDeadTexture, 8, (Rectangle[]) {
		(Rectangle){0, 0, 128, 128},
		(Rectangle){128, 0, 128, 128},
		(Rectangle){256, 0, 128, 128},
		(Rectangle){384, 0, 128, 128},
		(Rectangle){512, 0, 128, 128},
		(Rectangle){640, 0, 128, 128},
		(Rectangle){768, 0, 128, 128},
		(Rectangle){896, 0, 128, 128},
	}, 8);

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
    
    // State tracking: 0 = idle, 1 = running, 2 = shooting, 3 = reloading, 4 = dead
    int playerState = 0;
    int lastState = -1;
    
    while (!WindowShouldClose())
    {   
        Vector2 mousePos = GetMousePosition();
        
        ClearBackground(DARKGRAY);

        if (!IsAudioDeviceReady()) {
            DrawText("Audio device failed!", 100, 100, 20, RED);
        }

        if (fireSound.frameCount == 0) {
            DrawText("Sound failed to load!", 100, 130, 20, RED);
        }

        // Reset animation when entering a new state
        if (playerState != lastState) {
            lastState = playerState;
            if (playerState == 2) {
                ResetSpriteAnimation(&_playerShotAnimation);
            } else if (playerState == 3) {
                ResetSpriteAnimation(&_playerReloadAnimation);
            }
        }

        // Handle state transitions
        if (playerState == 2 && IsSpriteAnimationFinished(_playerShotAnimation)) {
            // Shooting finished, go back to idle
            playerState = 0;
            lastState = -1;
        } else if (playerState == 3 && IsSpriteAnimationFinished(_playerReloadAnimation)) {
            // Reloading finished, go back to idle
            playerState = 0;
            lastState = -1;
        } else if (playerState != 2 && playerState != 3) {
            // Only update state if not in shooting or reloading animation
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                playerState = 3;  // Reloading
                reload(&bulletCount, reloadSound);
            } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (bulletCount > 0) {
                    playerState = 2;  // Shooting
                    bulletCount -= 1;
                    PlaySound(fireSound);
                } else {
                    playerState = 3;  // Reloading
                    reload(&bulletCount, reloadSound);
                }
            } else if (characterControl(&playerPos)) {
                playerState = 1;  // Running
            } else {
                playerState = 0;  // Idle
            }
        }

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

        Rectangle playerTextureDest = (Rectangle){(int)playerPos.x, (int)playerPos.y, 128, 128};
        Vector2 origin = { 0 };

        BeginDrawing();

        // Draw based on current state
        switch (playerState) {
            case 0:  // Idle
                DrawSpriteAnimationPro(_playerIdleAnimation, playerTextureDest, origin, 0, WHITE);
                break;
            case 1:  // Running
                DrawSpriteAnimationPro(_playerRunAnimation, playerTextureDest, origin, 0, WHITE);
                break;
            case 2:  // Shooting
                DrawSpriteAnimationPro(_playerShotAnimation, playerTextureDest, origin, 0, WHITE);
                break;
            case 3:  // Reloading
                DrawSpriteAnimationPro(_playerReloadAnimation, playerTextureDest, origin, 0, WHITE);
                break;
            case 4:  // Dead
                DrawSpriteAnimationPro(_playerDeadAnimation, playerTextureDest, origin, 0, WHITE);
                break;
        }
        
        // Draw enemy
        DrawTexture(enemy, (int)enemyPos.x, (int)enemyPos.y, WHITE);
        // Drawing Mouse cursor is necessary just before EndDrawing() for overlapping issues
        DrawTexture(customMouse, (int)mousePos.x, (int)mousePos.y, WHITE);
        EndDrawing();
    }
    
    DisposeSpriteAnimation(_playerIdleAnimation);
    DisposeSpriteAnimation(_playerRunAnimation);
    DisposeSpriteAnimation(_playerReloadAnimation);
    DisposeSpriteAnimation(_playerShotAnimation);
    DisposeSpriteAnimation(_playerDeadAnimation);
    
    
    UnloadSound(fireSound);
    UnloadSound(reloadSound);
    UnloadSound(bgZombie);
    UnloadSound(playerDeath);
    UnloadSound(enemyDeath);
    UnloadSound(enemySpawn);
    UnloadTexture(customMouse);
    UnloadTexture(playerIdleTexture);
    UnloadTexture(playerRunTexture);
    UnloadTexture(playerDeadTexture);
    UnloadTexture(playerShotTexture);
    UnloadTexture(playerReloadTexture);
    UnloadTexture(enemy);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
