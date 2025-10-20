# Bitboard Checkers

A two-player 8x8 checkers game implementation in C with bitboard representation and save/load functionality.

## Features

- Full checkers rules implementation
- Forced captures with multi-jump sequences
- King promotion and movement
- Win detection (material advantage and no legal moves)
- Save/load system with auto-timestamped files
- Bitboard architecture for efficient piece storage
- Cross-platform support (Windows, Linux, macOS)

## Requirements

- GCC or any C compiler
- Terminal/Command Prompt
- Operating System: Windows, Linux, or macOS

## Quick Start

### Clone the Repository
```bash
git clone https://github.com/O-Ayyad/Bitboard-Checkers.git
cd Bitboard-Checkers
```

### Build & Run

**Linux/macOS:**
```bash
gcc main.c utils.c game_board.c -o BitBoard-Checkers
./BitBoard-Checkers
```

**Windows:**
```cmd
gcc main.c utils.c game_board.c -o BitBoard-Checkers.exe
BitBoard-Checkers.exe
```

**Make:**
```bash
make run
````

## How to Play

### Game Controls

**Selecting a Piece:**
- Enter coordinates in format: `A1`, `b2`, `4G`, `7h` (case-insensitive)

**Special Commands:**
- `SAVE` - Save current game state
- `EXIT` - Exit game
- `New Game` - Start fresh when loading saves

### Game Rules

1. **Movement:**
   - Red pieces (Player 1) move up the board
   - Black pieces (Player 2) move down the board
   - Regular pieces move diagonally forward one square
   - Kings can move diagonally in any direction

2. **Captures:**
   - Jump over opponent pieces to capture them
   - Captures are mandatory when available
   - Multiple captures in sequence are required

3. **King Promotion:**
   - Red pieces reaching row 8 become kings
   - Black pieces reaching row 1 become kings
   - Kings are displayed as uppercase letters (R/B)

4. **Winning:**
   - Capture all opponent pieces, or
   - Block opponent from making any legal moves

## Project Structure
```
checkers-game/
├── main.c              # Entry point, save/load system
├── utils.c             # Display and input handling
├── game_board.c        # Core game logic, move validation
├── game_board.h        # Header file with structs and declarations
├── README.md          # This file
└── saves/             # Auto-generated save directory
```

## Building Options

### Basic Build
```bash
gcc main.c utils.c game_board.c -o game
```

### Optimized Build
```bash
gcc -O3 -Wall -Wextra main.c utils.c game_board.c -o game
```

### Debug Build
```bash
gcc -g -Wall -Wextra main.c utils.c game_board.c -o game
```

## Technical Details

### Bitboard Representation

The game uses four 32-bit integers to represent the board state, with each bit representing one of the 32 playable (dark) squares:
```
[ ] [28] [ ] [29] [ ] [30] [ ] [31]
[24] [ ] [25] [ ] [26] [ ] [27] [ ]
[ ] [20] [ ] [21] [ ] [22] [ ] [23]
[16] [ ] [17] [ ] [18] [ ] [19] [ ]
[ ] [12] [ ] [13] [ ] [14] [ ] [15]
[8]  [ ] [9]  [ ] [10] [ ] [11] [ ]
[ ] [4]  [ ] [5]  [ ] [6]  [ ] [7]
[0]  [ ] [1]  [ ] [2]  [ ] [3]  [ ]
```

### Performance

- Move generation: O(1) for regular pieces, O(n) for kings
- Board evaluation: O(32) - checks all possible piece positions
- Memory usage: ~40 bytes per game state
