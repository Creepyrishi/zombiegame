# Some Kind of Game

A top-down zombie shooter built with C and [raylib](https://www.raylib.com/).

## Play Online

[https://somekindofgame.vercel.app](https://somekindofgame.vercel.app)

## Controls

| Key | Action |
|---|---|
| WASD | Move |
| Left Click | Shoot |
| Right Click | Reload |
| T | Start / Restart |

Survive waves of enemies. Each wave gets harder — enemies have more health.

## Build Locally

### Prerequisites
- [raylib](https://github.com/raysan5/raylib) (v6.0+)
- GCC or Clang

### Compile
```bash
gcc main.c animation.c -o game -lraylib -lGL -lm -lpthread -ldl -lrt
./game
```

### Web Build
Requires [Emscripten](https://emscripten.org/).
```bash
make web
```
Output goes to `web/` — deployable to any static host.
