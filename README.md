# SNAKE-GAME
A SIMPLE SNAKE GAME (named SN4GI) WITH 3 LEVELS OF DIFFICULTY.
# Snake Game in C - Console-Based Classic Arcade

Overview
A complete implementation of the classic Snake game written in pure C, featuring:  
- Multi-level progression (increasing difficulty)  
- Obstacles & moving hazards
- High score system* with file persistence  
- Colorful console UI*with keyboard controls  
- Windows API for smooth rendering  

# Features
 # 1 Core Gameplay
- 🐍 *Snake movement* with WASD controls  
- 🍎  *Food spawning* with collision avoidance  
- 💀 *Three collision types*:  
  - Wall crashes  
  - Self-bites  
  - Obstacle hits  

 # 2. Progressive Difficulty 
- Level 1: Basic snake (no obstacles)  
- Level 2 (50+ pts)*: Static obstacles (`X`)  
- Level 3 (100+ pts)*: Moving obstacle (bounces unpredictably)  

# 3. Visual & UI* 
Color-coded elements:  
  - Snake: Green → Cyan → Yellow (per level)  
  - Food: Red (`$`)  
  - Obstacles: Magenta/Red  
-  Real-time HUD*
-  showing score, level, and controls  

# 4. High Score System
- Binary file storage (`highscores.dat`)  
- Top 5 scoresw ith player names  
- uto-sortingof new high scores  

# 5. Technical Highlights
- Windows API for:  
  - Text coloring (`SetConsoleTextAttribute`)  
  - Cursor control (`CONSOLE_CURSOR_INFO`)  
- ANSI escapes for fast cursor positioning  
- Efficient game loop (~6.6 FPS)  

---

*🛠️ Compilation & Usage* 

 **Requirements**  
- *Windows OS*(uses `<conio.h>` and Windows API)  
- *GCC/MinGW* or any C compiler  

Build & Run
```bash
gcc snake_game.c -o snake_game
./snake_game
```
# Controls 
| Key | Action |  
|-----|--------|  
| W| Move Up |  
| A | Move Left |  
| S | Move Down |  
| D | Move Right |  
| P | Pause Game |  
| X*| Quit |  

---

# Educational Value 


# 1. Console Manipulation
   - Windows API vs. ANSI escapes  
   - Cursor positioning and color control  

# 2. Game Architecture
   - Game loop design (`ProcessInput → Update → Draw`)  
   - State management (pause, game over, levels)  

# 3. Algorithms
   - Snake movement (body-follow mechanics)  
   - Collision detection (AABB for walls/obstacles)  
   - Random placement with constraints (food spawning)  

# 4. File I/O
   - Binary file operations (`fwrite`/`fread`)  
   - Struct serialization for high scores  
This was all bout my project.


