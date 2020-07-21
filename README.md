# Print File System

Terminal based interactive filesystem browser that prints the selected entry's absolute path.
To be combined with scripts to create a more useful tool out of it.

# Building

## Prerequisites

* Bash
* Make
* G++ (C++17)
* Ncurses

Ubuntu/Debian:

```txt
sudo apt-get install libncurses5-dev
```

Arch/Manjaro:

```txt
sudo pacman -S ncurses
```

## Build

```txt
make clean all
```

## Installation

Install:

```
sudo make install
```

Uninstall:

```
sudo make uninstall
```

# Usage

```
Usage: pfs [OPTIONS] [PATH]

Interactive filesystem browser that prints the entry's absolute path

Options:
  -h   Print this and quit
  -m   Max parent height
  -v   Print version and quit

Navigation:
  Return   Print entry path and quit
  Up       Select above entry in list
  Down     Select below entry in list
  Left     Step into directory
  Right    Step back to parent directory
  Q        Quit without printing anything
  R        Refresh entry list

Will default to current working directory if no path is provided
```

# Issues

Here are some known issues:

* Colors are forced, and if the terminal does not support them, it will not start.
* Application will crash when attempting to step into a directory which the user does not have rights to.
* Flag `-m` does not validate its path, which could result in a crash.
* Windows and OSX have not been tested - it is unknown whether it even compiles.
* Arrow keys supported keyboards only.
* Listed entries are not sorted in any way.

**Note:** If a crash were to happen, open the application again and quit to fully terminate ncurses which restores your terminal's proper state.

# Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) on how to contribute.

# License

See [LICENSE.txt](LICENSE.txt) for project license.
