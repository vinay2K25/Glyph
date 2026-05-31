# Glyph

A terminal-based text-editor written in C using the ncurses, focused on re-building real editor internals from scratch.

## Current Development Status

Glyph is currently an actively-evolving project focused on re-creating the core mechanics of modern text-editors without relying on high-level abstractions.

The latest version now supports persistent file editing through a complete I/O work-flow, allowing users to:
- Open existing files,
- Create new files,
- Edit contents dynamically,
- Save changes directly from inside the editor

The latest version includes:
- Safe multi-line cursor navigation,
- Dynamic text-insertion,
- Line splitting,
- Back-space merging,
- In-between character insertion,
- Full-screen re-rendering using ncurses,
- Internal line-length tracking for accurate movement-constraints,
- Viewport-based vertical scrolling,
- Dynamic visible-row rendering,
- Cursor-tracking across off-screen content,
- Dynamic terminal-resize support,
- Unsaved-changes tracking,
- Exit confirmation dialogs,
- Adaptive ncurses-based UI elements,
- Viewport-based horizontal scrolling,
- Dynamic visible-column rendering,
- Horizontal cursor-tracking across off-screen content,
- Two-dimensional viewport management,
- Dynamic line-number rendering,
- Gutter-based editor layout,
- Colorized line-number display

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
- Status-bar rendering
- Real-time cursor position tracking
- Dynamic terminal-resize handling
- Viewport-aware rendering
- Adaptive content margins
- Gutter-based rendering layout
- Independent rendering of editor meta-data

### File Persistence
- File-name prompt window during start-up
- Automatic loading of existing file contents
- Creation of new files if they do not exist
- Ctrl + S support for saving files
- Ctrl + Q support for graceful exit
- Real-time save status messages
- Persistent editing work-flow similar to real text-editors
- Unsaved-changes detection
- Exit confirmation for modified but unsaved files

### Viewport Scrolling
- Vertical scrolling through large files
- Dynamic top-visible-row tracking
- Rendering only visible-portions of the text-buffer
- Smooth cursor-following behaviour
- Prevents cursor overlap with status-bar
- Scroll-aware cursor positioning

### User Interface
- Dynamically centered dialog windows
- Responsive layout based on terminal size
- Adaptive left-margin rendering
- Color-coded status and warning messages
- Visual unsaved-changes indicator
- Improved status-bar design

### Horizontal Scrolling
- Horizontal navigation through long-lines
- Dynamic left-visible-column tracking
- Rendering only visible-portions of long lines
- Automatic viewport adjustment while typing
- Viewport-aware cursor positioning
- Smooth left/right scrolling behaviour

### Line Numbers
- Dynamic line-number rendering
- Dedicated gutter region
- Viewport aware numbering,
- Colorized line-number display
- Automatic numbering for loaded files
- Line-number updates during insertion and deletion

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
make
```

3. Run
```bash
./glyph
```

4. Usage
- Enter file-name when prompted
- Use arrow keys to navigate
- Press:
  - Ctrl + S - Save file
  - Ctrl + Q - Quit editor

## Motivation

Glyph was built as a systems programming exercise to better understand how real-world editors internally manage
- Terminal rendering
- Text-buffers
- Cursor logic
- Interactive keyboard-driven interfaces
- File I/O handling

Instead of depending on existing frameworks, the project intentionally re-implements these machanics from scratch using C and ncurses.

## Future Goals
- Syntax highlighting
- Search functionality
- Clip-board support
- Multiple buffers
- Vim-style modal editing
- Split-pane windows