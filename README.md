# cs3050
CMP_SCI_3050 Final Project - Navigate two robots to points in a maze, avoiding collisions

## Running and Interpreting Output

The program takes a single text file as a commmand line argument, which contains the world. The program will output the map of the world to the console after every step a robot takes. The path of the first robot will be denoted by A's and the path of the second robot will be denoted by B's.

### Error Messages 

Any error encountered by the program will be outputted to the console with an accompanying error message. The program utilizes errno.h from the C standard library for some of these encountered errors. Others are handled by libhowderek.

## Before you begin

First, you need to make sure you have the necesarry working C development environment

  - On Windows, [install the Visual Studio Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools), [Clang for Windows](http://releases.llvm.org/4.0.1/LLVM-4.0.1-win64.exe) (direct download), and [CMake](https://cmake.org/download/)

  - On macOS, [install XCode](https://itunes.apple.com/us/app/xcode/id497799835) and [Homebrew](https://brew.sh/).
    After the above are installed:
    ```bash
    xcode-select --install
    brew tap caskroom/cask
    brew cask install cmake
    brew install
    ```

  - On Linux, install your operating system's development packages:
    - Ubuntu/Debian: `sudo apt-get update && sudo apt-get install build-essential`
    - Fedora: `sudo dnf install @development-tools`

Second, you should install [Qt Creator](https://www1.qt.io/download-open-source/) (on macOS `brew cask install qt-creator`) to speed development. 

If you choose to do so, then:

  - [Setup Qt Creator with your compiler](http://doc.qt.io/qtcreator/creator-tool-chains.html)
  - [Setup Qt Creator with CMake](http://doc.qt.io/qtcreator/creator-project-cmake.html)
    - You may need to go to *Preferences -> Build & Run -> Kits* and configure your kit to use your CMake version and set the generator to "Unix Makefiles" and the extra generator to CodeBlocks
    - on macOS, the CMake binary will be found in the CMake.app package at `/Applications/CMake.app/Contents/bin/cmake`

## Working on the project

To work on this project, first you need to clone this repository (`git clone git@github.com:cddjj/cs3050.git`)

If you are using Qt Creator, open the project's CMakeLists.txt. You will need to configure the project for your computer. The default options should are fine.

Otherwise, open a terminal in the project's directory and run `cmake CMakeLists.txt`

Now you are ready to start working on the project. Edit whichever files you'd like to work on, and build the project. In Qt Creator, this is done by pressing the large green start button in the bottom left corner. Without Qt Creator, run `make robotpath` in the base directory of the project.