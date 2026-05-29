#include "raylib.h"
#include "animation.h"

#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_BULLETS 50
#define BULLET_SPEED 10
#define BULLET_RADIUS 3

#define MAX_ENEMIES 5
#define MIN_WAVE_ENEMIES 2
#define MAX_WAVE_ENEMIES 5
#define WAVE_SPAWN_DURATION 7.0f

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

typedef struct Enemy
{
    Vector2 position;
    int health;
} Enemy;

void FireBullet(Bullet bullets[], Vector2 start, Vector2 target)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            Vector2 direction = { target.x - start.x, target.y - start.y };
            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

            if (length == 0)
            {
                return;
            }

            direction.x /= length;
            direction.y /= length;

            bullets[i].active = 1;
            bullets[i].direction = direction;
            bullets[i].velocity = BULLET_SPEED;
            bullets[i].position = start;

            return;
        }
    }
}

void SpawnEnemy(Enemy enemies[], Vector2 position, int health)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].health <= 0)
        {
            enemies[i].position = position;
            enemies[i].health = health;
            return;
        }
    }
}

void ClearEnemies(Enemy enemies[])
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].health = 0;
    }
}

int CountAliveEnemies(Enemy enemies[])
{
    int count = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].health > 0)
        {
            count++;
        }
    }

    return count;
}

void SortSpawnTimes(float spawnTimes[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (spawnTimes[j] > spawnTimes[j + 1])
            {
                float temp = spawnTimes[j];
                spawnTimes[j] = spawnTimes[j + 1];
                spawnTimes[j + 1] = temp;
            }
        }
    }
}

void SpawnOneRandomEnemy(Enemy enemies[], int enemyHealth)
{
    Vector2 randomPos = {
        GetRandomValue(0, SCREEN_WIDTH - 96),
        GetRandomValue(0, SCREEN_HEIGHT - 96)
    };

    SpawnEnemy(enemies, randomPos, enemyHealth);
}

void PrepareEnemyWave(
    Enemy enemies[],
    int *waveEnemyCount,
    int *enemiesSpawnedThisWave,
    int *isSpawningWave,
    float *waveSpawnTimer,
    float spawnTimes[]
)
{
    ClearEnemies(enemies);

    *waveEnemyCount = GetRandomValue(MIN_WAVE_ENEMIES, MAX_WAVE_ENEMIES);
    *enemiesSpawnedThisWave = 0;
    *isSpawningWave = 1;
    *waveSpawnTimer = 0.0f;

    for (int i = 0; i < *waveEnemyCount; i++)
    {
        spawnTimes[i] = GetRandomValue(0, (int)(WAVE_SPAWN_DURATION * 100)) / 100.0f;
    }

    SortSpawnTimes(spawnTimes, *waveEnemyCount);
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

    float enemySpeed = 1.7f;

    Bullet bullets[MAX_BULLETS] = { 0 };
    Enemy enemies[MAX_ENEMIES] = { 0 };

    int playerHealth = 100;
    int bulletCount = 0;

    int waveNumber = 1;
    int enemyHealthLevel = 1;

    int waveEnemyCount = 0;
    int enemiesSpawnedThisWave = 0;
    int isSpawningWave = 0;
    float waveSpawnTimer = 0.0f;
    float spawnTimes[MAX_WAVE_ENEMIES] = { 0 };

    PlayerState playerState = PLAYER_IDLE;
    PlayerDirection playerDirection = DIR_DOWN;

    Vector2 origin = { 0, 0 };

    SetTargetFPS(60);

    reload(&bulletCount, reloadSound);

    PrepareEnemyWave(
        enemies,
        &waveEnemyCount,
        &enemiesSpawnedThisWave,
        &isSpawningWave,
        &waveSpawnTimer,
        spawnTimes
    );

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

                    Vector2 startBulletPos = {
                        playerPos.x + 64,
                        playerPos.y + 64
                    };

                    FireBullet(bullets, startBulletPos, mousePos);

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

        // Spawn enemies one by one within 4 seconds
        if (playerState != PLAYER_DEAD && isSpawningWave)
        {
            waveSpawnTimer += GetFrameTime();

            while (
                enemiesSpawnedThisWave < waveEnemyCount &&
                waveSpawnTimer >= spawnTimes[enemiesSpawnedThisWave]
            )
            {
                SpawnOneRandomEnemy(enemies, enemyHealthLevel);
                PlaySound(enemySpawn);

                enemiesSpawnedThisWave++;
            }

            if (enemiesSpawnedThisWave >= waveEnemyCount)
            {
                isSpawningWave = 0;
            }
        }

        // Zombie movement toward player
        if (playerState != PLAYER_DEAD)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].health > 0)
                {
                    if (enemies[i].position.x > playerPos.x)
                    {
                        enemies[i].position.x -= enemySpeed;
                    }

                    if (enemies[i].position.x < playerPos.x)
                    {
                        enemies[i].position.x += enemySpeed;
                    }

                    if (enemies[i].position.y > playerPos.y)
                    {
                        enemies[i].position.y -= enemySpeed;
                    }

                    if (enemies[i].position.y < playerPos.y)
                    {
                        enemies[i].position.y += enemySpeed;
                    }
                }
            }
        }

        Rectangle playerTextureDest = {
            playerPos.x,
            playerPos.y,
            128,
            128
        };

        Rectangle PlayerHitbox = {
            playerPos.x + 50,
            playerPos.y + 20,
            23,
            53
        };

        // Update bullets
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].position.x += bullets[i].velocity * bullets[i].direction.x;
                bullets[i].position.y += bullets[i].velocity * bullets[i].direction.y;

                if (
                    bullets[i].position.x < 0 ||
                    bullets[i].position.x > SCREEN_WIDTH ||
                    bullets[i].position.y < 0 ||
                    bullets[i].position.y > SCREEN_HEIGHT
                )
                {
                    bullets[i].active = 0;
                }
            }
        }

        // Player collision with enemies and bullet collision with enemies
        if (playerState != PLAYER_DEAD)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].health > 0)
                {
                    Rectangle EnemyHitbox = {
                        enemies[i].position.x + 30,
                        enemies[i].position.y + 30,
                        23,
                        53
                    };

                    // Enemy touches player
                    if (CheckCollisionRecs(PlayerHitbox, EnemyHitbox))
                    {
                        if (playerHealth > 0)
                        {
                            playerHealth -= 3;
                        }

                        if (playerHealth <= 0)
                        {
                            playerHealth = 0;
                            playerState = PLAYER_DEAD;
                            PlaySound(playerDeath);
                        }
                    }

                    // Bullet hits enemy
                    for (int j = 0; j < MAX_BULLETS; j++)
                    {
                        if (
                            bullets[j].active &&
                            CheckCollisionCircleRec(
                                bullets[j].position,
                                BULLET_RADIUS,
                                EnemyHitbox
                            )
                        )
                        {
                            enemies[i].health -= 1;
                            bullets[j].active = 0;

                            if (enemies[i].health <= 0)
                            {
                                enemies[i].health = 0;
                                PlaySound(enemyDeath);
                            }

                            break;
                        }
                    }
                }
            }
        }

        // If all enemies are dead and the wave finished spawning, create next wave
        if (
            playerState != PLAYER_DEAD &&
            !isSpawningWave &&
            enemiesSpawnedThisWave >= waveEnemyCount &&
            CountAliveEnemies(enemies) == 0
        )
        {
            playerHealth = 100;

            waveNumber++;
            enemyHealthLevel++;

            PrepareEnemyWave(
                enemies,
                &waveEnemyCount,
                &enemiesSpawnedThisWave,
                &isSpawningWave,
                &waveSpawnTimer,
                spawnTimes
            );
        }

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

        // Draw bullets
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                DrawCircle(
                    bullets[i].position.x,
                    bullets[i].position.y,
                    BULLET_RADIUS,
                    YELLOW
                );
            }
        }

        // Draw all living zombies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].health > 0)
            {
                Rectangle zombieWomanTextureDest = {
                    enemies[i].position.x,
                    enemies[i].position.y,
                    96,
                    96
                };

                DrawSpriteAnimationPro(
                    _zombieWomanWalkAnimation,
                    zombieWomanTextureDest,
                    origin,
                    0,
                    WHITE
                );
            }
        }

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

                DrawText("YOU DIED", 300, 250, 50, RED);
            } break;
        }

        DrawText(TextFormat("Bullets: %d", bulletCount), 10, 10, 20, WHITE);
        DrawText(TextFormat("Health: %d", playerHealth), 10, 35, 20, WHITE);
        DrawText(TextFormat("Wave: %d", waveNumber), 10, 60, 20, WHITE);

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