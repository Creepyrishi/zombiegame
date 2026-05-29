#include "raylib.h"
#include "animation.h"

#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_BULLETS 50
#define BULLET_SPEED 10
#define BULLET_RADIUS 3


typedef enum PlayerDirection
{
    DIR_DOWN,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT
} PlayerDirection;

typedef enum PlayerState
{
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_SHOOTING,
    PLAYER_RELOADING,
    PLAYER_DEAD
} PlayerState;

typedef struct Bullet
{
    Vector2 direction;
    float velocity;
    Vector2 position;
    int active;
} Bullet;

void FireBullet(Bullet bullets[], Vector2 start, Vector2 target)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].active = 1;

            //Calculate Direction
            Vector2 direction = {target.x - start.x, target.y - start.y};
            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
            direction.x /= length;
            direction.y /= length;
            bullets[i].direction = direction;

            //Calculate Velocity
            bullets[i].velocity = BULLET_SPEED;

            //Position
            bullets[i].position.x = start.x;
            bullets[i].position.y = start.y;
            
            return;
        }
    }
}

void reload(int *bulletCount, Sound reloadSound)
{
    *bulletCount = 10;
    PlaySound(reloadSound);
}

int characterControl(Vector2 *playerPos, PlayerDirection *direction)
{
    int isMoving = 0;

    if (IsKeyDown(KEY_W))
    {
        playerPos->y -= 5;
        *direction = DIR_UP;
        isMoving = 1;
    }

    if (IsKeyDown(KEY_S))
    {
        playerPos->y += 5;
        *direction = DIR_DOWN;
        isMoving = 1;
    }

    if (IsKeyDown(KEY_A))
    {
        playerPos->x -= 5;
        *direction = DIR_LEFT;
        isMoving = 1;
    }

    if (IsKeyDown(KEY_D))
    {
        playerPos->x += 5;
        *direction = DIR_RIGHT;
        isMoving = 1;
    }

    return isMoving;
}

SpriteAnimation *GetRunAnimation(
    PlayerDirection direction,
    SpriteAnimation *up,
    SpriteAnimation *down,
    SpriteAnimation *left,
    SpriteAnimation *right
)
{
    switch (direction)
    {
        case DIR_UP:
            return up;

        case DIR_DOWN:
            return down;

        case DIR_LEFT:
            return left;

        case DIR_RIGHT:
            return right;

        default:
            return down;
    }
}

SpriteAnimation *GetShootAnimation(
    PlayerDirection direction,
    SpriteAnimation *up,
    SpriteAnimation *down,
    SpriteAnimation *left,
    SpriteAnimation *right
)
{
    switch (direction)
    {
        case DIR_UP:
            return up;

        case DIR_DOWN:
            return down;

        case DIR_LEFT:
            return left;

        case DIR_RIGHT:
            return right;

        default:
            return down;
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon Of Solta");
    HideCursor();

    InitAudioDevice();
    printf("Audio device ready: %d\n", IsAudioDeviceReady());

    Texture2D customMouse = LoadTexture("assets/sprites/cursor.png");

    // Player Sprites
    Texture2D playerIdleTexture = LoadTexture("assets/sprites/Player/Idle.png");

    Texture2D playerRunUPTexture = LoadTexture("assets/sprites/Player/Run/up.png");
    Texture2D playerRunDOWNTexture = LoadTexture("assets/sprites/Player/Run/down.png");
    Texture2D playerRunLEFTTexture = LoadTexture("assets/sprites/Player/Run/left.png");
    Texture2D playerRunRIGHTTexture = LoadTexture("assets/sprites/Player/Run/right.png");

    Texture2D playerShotUPTexture = LoadTexture("assets/sprites/Player/Shot/up.png");
    Texture2D playerShotDOWNTexture = LoadTexture("assets/sprites/Player/Shot/down.png");
    Texture2D playerShotLEFTTexture = LoadTexture("assets/sprites/Player/Shot/left.png");
    Texture2D playerShotRIGHTTexture = LoadTexture("assets/sprites/Player/Shot/right.png");

    // Zombie Woman Sprites
    Texture2D zombieWomanIdleTexture = LoadTexture("assets/sprites/enemy/Zombie Woman/Idle.png");
    Texture2D zombieWomanWalkTexture = LoadTexture("assets/sprites/enemy/Zombie Woman/Walk.png");
    Texture2D zombieWomanRunTexture = LoadTexture("assets/sprites/enemy/Zombie Woman/Run.png");
    Texture2D zombieWomanDeadTexture = LoadTexture("assets/sprites/enemy/Zombie Woman/Dead.png");
    Texture2D zombieWomanAttackTexture = LoadTexture("assets/sprites/enemy/Zombie Woman/Attack_2.png");

    // Player animations
    SpriteAnimation _playerIdleAnimation =
        CreateSpriteAnimationStrip(playerIdleTexture, 12, 254, 254, 1);

    SpriteAnimation _playerRunUPAnimation =
        CreateSpriteAnimationStrip(playerRunUPTexture, 12, 254, 254, 24);

    SpriteAnimation _playerRunDOWNAnimation =
        CreateSpriteAnimationStrip(playerRunDOWNTexture, 12, 254, 254, 24);

    SpriteAnimation _playerRunLEFTAnimation =
        CreateSpriteAnimationStrip(playerRunLEFTTexture, 12, 254, 254, 24);

    SpriteAnimation _playerRunRIGHTAnimation =
        CreateSpriteAnimationStrip(playerRunRIGHTTexture, 12, 254, 254, 24);

    SpriteAnimation _playerShootUPAnimation =
        CreateSpriteAnimationStrip(playerShotUPTexture, 60, 254, 254, 10);

    SpriteAnimation _playerShootDOWNAnimation =
        CreateSpriteAnimationStrip(playerShotDOWNTexture, 60, 254, 254, 10);

    SpriteAnimation _playerShootLEFTAnimation =
        CreateSpriteAnimationStrip(playerShotLEFTTexture, 60, 254, 254, 10);

    SpriteAnimation _playerShootRIGHTAnimation =
        CreateSpriteAnimationStrip(playerShotRIGHTTexture, 60, 254, 254, 10);

    // Zombie animations
    SpriteAnimation _zombieWomanIdleAnimation =
        CreateSpriteAnimationStrip(zombieWomanIdleTexture, 8, 96, 96, 5);

    SpriteAnimation _zombieWomanWalkAnimation =
        CreateSpriteAnimationStrip(zombieWomanWalkTexture, 8, 96, 96, 6);

    SpriteAnimation _zombieWomanRunAnimation =
        CreateSpriteAnimationStrip(zombieWomanRunTexture, 12, 96, 96, 8);

    SpriteAnimation _zombieWomanDeadAnimation =
        CreateSpriteAnimationStrip(zombieWomanDeadTexture, 8, 96, 96, 5);

    SpriteAnimation _zombieWomanAttackAnimation =
        CreateSpriteAnimationStrip(zombieWomanAttackTexture, 10, 96, 96, 4);

    // Audio
    Sound fireSound = LoadSound("assets/sounds/gun_fire.wav");
    Sound reloadSound = LoadSound("assets/sounds/reload.wav");
    Sound bgZombie = LoadSound("assets/sounds/bg_zombie.wav");
    Sound playerDeath = LoadSound("assets/sounds/emotional_damage.wav");
    Sound enemyDeath = LoadSound("assets/sounds/enemy_death.wav");
    Sound enemySpawn = LoadSound("assets/sounds/enemy_spwan.wav");

    // Positions
    Vector2 playerPos = { 50, 50 };
    Vector2 enemyPos = { 300, 200 };

    float enemySpeed = 1.7f;
    Bullet bullets[MAX_BULLETS] = {0};
    int playerHealth = 100;
    int bulletCount = 0;

    PlayerState playerState = PLAYER_IDLE;
    PlayerDirection playerDirection = DIR_DOWN;

    SetTargetFPS(60);

    reload(&bulletCount, reloadSound);

    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();

        // If player is shooting, wait until shooting animation finishes
        if (playerState == PLAYER_SHOOTING)
        {
            SpriteAnimation *currentShootAnimation = GetShootAnimation(
                playerDirection,
                &_playerShootUPAnimation,
                &_playerShootDOWNAnimation,
                &_playerShootLEFTAnimation,
                &_playerShootRIGHTAnimation
            );

            if (IsSpriteAnimationFinished(*currentShootAnimation))
            {
                playerState = PLAYER_IDLE;
            }
        }
        else if (playerState != PLAYER_DEAD)
        {
            int isMoving = characterControl(&playerPos, &playerDirection);

            // Left mouse reloads
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                playerState = PLAYER_RELOADING;
                reload(&bulletCount, reloadSound);
                playerState = PLAYER_IDLE;
            }

            // Right mouse shoots
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                if (bulletCount > 0)
                {
                    bulletCount--;

                    SpriteAnimation *currentShootAnimation = GetShootAnimation(
                        playerDirection,
                        &_playerShootUPAnimation,
                        &_playerShootDOWNAnimation,
                        &_playerShootLEFTAnimation,
                        &_playerShootRIGHTAnimation
                    );

                    Vector2 startBulletPos = {playerPos.x, playerPos.y};

                    FireBullet(bullets, startBulletPos,  mousePos);
                    
                    ResetSpriteAnimation(currentShootAnimation);

                    playerState = PLAYER_SHOOTING;
                    PlaySound(fireSound);
                }
                else
                {
                    playerState = PLAYER_RELOADING;
                    reload(&bulletCount, reloadSound);
                    playerState = PLAYER_IDLE;
                }
            }

            else if (isMoving)
            {
                playerState = PLAYER_RUNNING;
            }

            else
            {
                playerState = PLAYER_IDLE;
            }
        }

        // Zombie movement toward player
        if (enemyPos.x > playerPos.x)
        {
            enemyPos.x -= enemySpeed;
        }

        if (enemyPos.x < playerPos.x)
        {
            enemyPos.x += enemySpeed;
        }

        if (enemyPos.y > playerPos.y)
        {
            enemyPos.y -= enemySpeed;
        }

        if (enemyPos.y < playerPos.y)
        {
            enemyPos.y += enemySpeed;
        }
        

        Rectangle playerTextureDest = {
            (int)playerPos.x,
            (int)playerPos.y,
            128,
            128
        };

        Rectangle zombieWomanTextureDest = {
            (int)enemyPos.x,
            (int)enemyPos.y,
            96,
            96
        };
        Vector2 origin = { 0, 0 };

        BeginDrawing();

        ClearBackground(DARKGRAY);

        if (!IsAudioDeviceReady())
        {
            DrawText("Audio device failed!", 100, 100, 20, RED);
        }

        if (fireSound.frameCount == 0)
        {
            DrawText("Sound failed to load!", 100, 130, 20, RED);
        }

        //Update Bullets and Draw
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].position.x += bullets[i].velocity* bullets[i].direction.x;
                bullets[i].position.y += bullets[i].velocity * bullets[i].direction.y;

                DrawCircle(bullets[i].position.x, bullets[i].position.y, BULLET_RADIUS, YELLOW);

                //Check if bullet is out of bounds
                if (bullets[i].position.x < 0 || bullets[i].position.x > SCREEN_WIDTH || bullets[i].position.y < 0 || bullets[i].position.y > SCREEN_HEIGHT)
                {
                    bullets[i].active = 0;
                }
            }
        }

        // Draw zombie as 2D animated sprite
        DrawSpriteAnimationPro(
            _zombieWomanWalkAnimation,
            zombieWomanTextureDest,
            origin,
            0,
            WHITE
        );

        // Draw player based on state and direction
        switch (playerState)
        {
            case PLAYER_IDLE:
            {
                DrawSpriteAnimationPro(
                    _playerIdleAnimation,
                    playerTextureDest,
                    origin,
                    0,
                    WHITE
                );
            } break;

            case PLAYER_RUNNING:
            {
                SpriteAnimation *currentRunAnimation = GetRunAnimation(
                    playerDirection,
                    &_playerRunUPAnimation,
                    &_playerRunDOWNAnimation,
                    &_playerRunLEFTAnimation,
                    &_playerRunRIGHTAnimation
                );

                DrawSpriteAnimationPro(
                    *currentRunAnimation,
                    playerTextureDest,
                    origin,
                    0,
                    WHITE
                );
            } break;

            case PLAYER_SHOOTING:
            {
                SpriteAnimation *currentShootAnimation = GetShootAnimation(
                    playerDirection,
                    &_playerShootUPAnimation,
                    &_playerShootDOWNAnimation,
                    &_playerShootLEFTAnimation,
                    &_playerShootRIGHTAnimation
                );

                DrawSpriteAnimationPro(
                    *currentShootAnimation,
                    playerTextureDest,
                    origin,
                    0,
                    WHITE
                );
            } break;

            case PLAYER_RELOADING:
            {
                DrawSpriteAnimationPro(
                    _playerIdleAnimation,
                    playerTextureDest,
                    origin,
                    0,
                    WHITE
                );
            } break;

            case PLAYER_DEAD:
            {
                DrawSpriteAnimationPro(
                    _playerIdleAnimation,
                    playerTextureDest,
                    origin,
                    0,
                    WHITE
                );
            } break;
        }

        

        Rectangle PlayerHitbox = {
            playerPos.x + 50,
            playerPos.y + 20,
            23,
            53
        };

        Rectangle EnemyHitbox = {
            enemyPos.x + 30,
            enemyPos.y + 30,
            23,
            53
        };

        // DrawRectangleRec(EnemyHitbox, GRAY);
        // DrawRectangleRec(PlayerHitbox, GRAY);
        
        if (CheckCollisionRecs(PlayerHitbox, EnemyHitbox)) {
            if (playerHealth > 0) {
                playerHealth -= 3;
            } else {
                playerHealth = 0;
                playerState = PLAYER_DEAD;
            }
        }

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active && CheckCollisionCircleRec(bullets[i].position, BULLET_RADIUS, EnemyHitbox)) {
                // Do some damage to the enemy
                playerHealth += 3;
                bullets[i].active = 0;
            }
        }

        DrawText(TextFormat("Bullets: %d", bulletCount), 10, 10, 20, WHITE);
        DrawText(TextFormat("Health: %d", playerHealth), 10, 35, 20, WHITE);

        // Draw mouse cursor last so it appears above everything
        DrawTexture(customMouse, (int)mousePos.x, (int)mousePos.y, WHITE);

        EndDrawing();
    }

    // Dispose player animations
    DisposeSpriteAnimation(_playerIdleAnimation);

    DisposeSpriteAnimation(_playerRunUPAnimation);
    DisposeSpriteAnimation(_playerRunDOWNAnimation);
    DisposeSpriteAnimation(_playerRunLEFTAnimation);
    DisposeSpriteAnimation(_playerRunRIGHTAnimation);

    DisposeSpriteAnimation(_playerShootUPAnimation);
    DisposeSpriteAnimation(_playerShootDOWNAnimation);
    DisposeSpriteAnimation(_playerShootLEFTAnimation);
    DisposeSpriteAnimation(_playerShootRIGHTAnimation);

    // Dispose zombie animations
    DisposeSpriteAnimation(_zombieWomanIdleAnimation);
    DisposeSpriteAnimation(_zombieWomanWalkAnimation);
    DisposeSpriteAnimation(_zombieWomanRunAnimation);
    DisposeSpriteAnimation(_zombieWomanDeadAnimation);
    DisposeSpriteAnimation(_zombieWomanAttackAnimation);

    // Unload sounds
    UnloadSound(fireSound);
    UnloadSound(reloadSound);
    UnloadSound(bgZombie);
    UnloadSound(playerDeath);
    UnloadSound(enemyDeath);
    UnloadSound(enemySpawn);

    // Unload textures
    UnloadTexture(customMouse);

    UnloadTexture(playerIdleTexture);

    UnloadTexture(playerRunUPTexture);
    UnloadTexture(playerRunDOWNTexture);
    UnloadTexture(playerRunLEFTTexture);
    UnloadTexture(playerRunRIGHTTexture);

    UnloadTexture(playerShotUPTexture);
    UnloadTexture(playerShotDOWNTexture);
    UnloadTexture(playerShotLEFTTexture);
    UnloadTexture(playerShotRIGHTTexture);

    UnloadTexture(zombieWomanIdleTexture);
    UnloadTexture(zombieWomanWalkTexture);
    UnloadTexture(zombieWomanRunTexture);
    UnloadTexture(zombieWomanDeadTexture);
    UnloadTexture(zombieWomanAttackTexture);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}