# Raylib Setup Guide for Ubuntu

## Overview
Build Raylib from source with CMake. Raylib ships with ready-to-use Makefiles and CMake support, making this the cleanest approach for Linux.

## Step 1: Install Build Tools and Dependencies

```bash
sudo apt update
sudo apt install build-essential git cmake pkg-config
sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
```

**Why you need these:**

| Package | Purpose |
|---------|---------|
| `build-essential` | GCC compiler, linker, Make |
| `git` | Download Raylib source |
| `cmake` | Build Raylib cleanly |
| `mesa-common-dev`, `libx11-dev`, `libxrandr-dev`, `libxi-dev`, `xorg-dev`, `libgl1-mesa-dev`, `libglu1-mesa-dev` | Window, OpenGL, keyboard, mouse input |
| `libasound2-dev` | Audio support |

## Step 2: Download Raylib

```bash
cd ~
git clone https://github.com/raysan5/raylib.git
cd raylib
```

Checkout the stable v6.0 release (released April 23, 2026):

```bash
git checkout 6.0
```

## Step 3: Build and Install Raylib

```bash
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
sudo ldconfig
```

**Why `sudo ldconfig`:** Tells Linux where the new library lives so it can be found at runtime.

## Step 4: Verify Installation

Check Raylib version:

```bash
pkg-config --modversion raylib
```

You should see `6.0.0` or similar.

Check compile flags:

```bash
pkg-config --libs --cflags raylib
```

## Step 5: Test with a Minimal Program

Create a test directory:

```bash
mkdir ~/raylib-test
cd ~/raylib-test
```

Create `main.c`:

```c
#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "raylib works on Ubuntu!");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib is working!", 250, 200, 30, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

## Step 6: Compile and Run

```bash
cc main.c -o game $(pkg-config --libs --cflags raylib)
./game
```

If a window opens with "Raylib is working!" text, the setup is complete.

## Troubleshooting

If compilation succeeds but running fails with:

```
error while loading shared libraries: libraylib.so
```

Run:

```bash
sudo ldconfig
./game
```

## Next Steps

Once Raylib is working, focus on understanding the **game loop**—it's the heartbeat of your engine:

```c
while (!WindowShouldClose())
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // Draw your game here
    EndDrawing();
}
```
