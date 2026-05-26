# Glyph

A terminal-based text-editor written in C using the ncurses library.

## Current Development Status

Glyph is currently an actively-evolving project focused on re-creating the core mechanics of modern text-editors without relying on high-level abstractions.

The latest version includes:
- Safe multi-line cursor navigation,
- Dynamic text-insertion,
- Line splitting,
- Back-space merging,
- In-between character insertion,
- Full-screen re-rendering using ncurses,
- Internal line-length tracking for accurate movement-constraints

## Features Implemented So Far

### Cursor Navigation
- Horizontal cursor movement using arrow-keys
- Vertical navigation across multiple lines
- Safe boundary checks to prevent invalid movement
- Smart cursor-snapping when moving between two lines of un-equal lengths

### Text Editing
- Character insertion at arbitrary positions
- Mid-line insertion with right-shifting logic
- Dynamic text-buffer updates
- Printable ASCII-input handling

### Multi-line Editing
- Enter key creates new lines automatically
- Remaining characters split on-to the next line
- Automatic line shifting with-in the text-buffer

### Back-space Handling
- Character deletion inside lines
- Line-merging when back-spacing at column zero
- Up-ward shifting of remaining lines after merge
- Preservation of cursor consistency during merges

### Rendering System
- Full-screen re-draw every frame
- ncurses-based rendering pipeline
- Manual cursor re-positioning
- Dynamic terminal-refresh loop

## Tech Stack
- **Language:** C
- **Terminal UI:** ncurses
- **Core Concepts Explored:**
  - Text-Buffers
  - File Descriptors
  - Cursor Management
  - Terminal Rendering
  - Buffer Manipulation
  - Processed Keyboard Input
  - Dynamic Screen Re-painting

 ## Building and Running

 1. Install ncurses
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

2. Compile
```bash
gcc 05SafeNavigation.c -lncurses
```

3. Run
```bash
./a.out
```

## Motivation

Glyph was built as a systems programming exercise to better understand how real-world editors internally manage
- Terminal rendering
- Text-buffers
- Cursor logic
- Interactive keyboard-driven interfaces

Instead of depending on existing frameworks, the project intentionally re-implements these machanics from scratch using C and ncurses.

## Future Goals
- File I/O support
- Syntax highlighting
- Vertical scrolling
- Search functionality
- Clip-board support
- Multiple buffers
- Vim-style modal editing
- Split-pane windows
