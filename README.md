# File Manager

A desktop file manager built in C++ using wxWidgets. I built this to get hands-on with C++17, native GUI programming, and working directly with the filesystem API — areas I wanted to explore beyond higher-level languages.

## Features

- Browse the filesystem with an editable path bar
- Open files with the system default application
- Create directories, rename, and delete files/directories (with confirmation)
- Copy and move files via clipboard-style copy/cut/paste
- Detailed listing: name, type, size, and last modified date
- Status bar feedback for all operations

**Keyboard shortcuts:**

| Action | Shortcut |
|---|---|
| Open | Ctrl+O |
| New Directory | Ctrl+N |
| Rename | Ctrl+R |
| Delete | Del |
| Copy | Ctrl+C |
| Cut | Ctrl+X |
| Paste | Ctrl+V |
| Refresh | F5 |
| Exit | Ctrl+Q |

## Requirements

- C++17 or later
- wxWidgets 3.0+ (3.2 recommended)
- macOS or Linux

Install wxWidgets on macOS:
```bash
brew install wxwidgets
```

On Ubuntu/Debian:
```bash
sudo apt install libwxgtk3.2-dev
```

## Build & Run

```bash
git clone https://github.com/Mananjoshi2/file-manager.git
cd file-manager
make
./filemanager
```

Clean build:
```bash
make clean && make
```

## Project Structure

```
├── FileManagerApp.h/.cpp    # wxApp subclass — entry point and initialization
├── FileManagerFrame.h/.cpp  # Main frame — UI layout, event handling, file ops
└── Makefile
```
