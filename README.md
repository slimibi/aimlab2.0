# Aim Trainer Pro

A professional aim trainer inspired by Aim Lab with a stunning space theme. Improve your accuracy and reaction time in an immersive environment.

## Features
- Precision training: click on dynamic, colorful targets  
- 45-second intense sessions with scoring based on target size  
- Persistent scoreboard with automatic save  
- Space-themed visuals: stars, rotating planet, particle explosions  
- Optional background music and audio control  

## Controls
- **Menu:** Arrow keys to navigate, Enter/Space to select  
- **Game:** Left click to shoot, Esc to return to menu  
- **End Screen:** Space to return to main menu  

## Prerequisites
- CSFML library installed  
- GCC (C99), Make utility  

## Installation
```
bash
git clone <repo-url>
cd duck_hunt
make install_deps   # Install CSFML dependencies
make run            # Build and run
```

## Gameplay
- Navigate menu → select "PLAY" → 5-second countdown → 45-second session  
- Hit targets: smaller targets = more points  
- Scores automatically saved  

## Code Structure
- `main.c` — Entry point and events  
- `game.c` — Core game logic and rendering  
- `duck.c` — Target/alien management  
- `utils.c` — Helper functions and effects  
- `duck_hunt.h` — Structs and function prototypes  

## License
Educational use. Modify and distribute freely.

