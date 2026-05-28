#include "raylib.h"
#include <stdatomic.h>
#include <stdio.h>

int main(void)
{
    InitWindow(800, 450, "Dungeon Of Jhur");
    HideCursor();
    InitAudioDevice();
    printf("Audio device ready: %d\n", IsAudioDeviceReady());
    
    Texture2D customMouse = LoadTexture("assets/sprites/cursor.png");
    Sound fireSound = LoadSound("assets/sounds/gun_fire.wav");
    Sound reloadSound = LoadSound("assets/sounds/reload.wav");
    printf("Sound frames: %d\n", fireSound.frameCount);

    SetTargetFPS(60);

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
            PlaySound(reloadSound);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            printf("RIGHT click detected\n Firing...\n");
            PlaySound(fireSound);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            printf("MIDDLE click detected\n");
            PlaySound(fireSound);
        }
        
        ClearBackground(DARKGRAY);
        DrawText("Raylib is working!", 250, 200, 30, RAYWHITE);
        DrawText("Click any mouse button!", 200, 250, 20, RAYWHITE);
        

        
        // Drawinng Mouse cursor is nessary just before EndDrawing() for overlapping issues
        DrawTexture(customMouse, (int)mousePos.x, (int)mousePos.y, WHITE);
        EndDrawing();
    }

    UnloadSound(fireSound);
    UnloadTexture(customMouse);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
