# Tobor (Beta)

## Overview


Tobor is an implementation of the game [Ricochet Robots](https://en.wikipedia.org/wiki/Ricochet_Robots).
![grafik](https://github.com/Necktschnagge/tobor-games/assets/22790899/705700d6-eabc-4a73-827a-fc3a9c263258)

### Rules
The goal of the game is to move the target piece (*here the green ball*) to the target cell on the board (*here the cell painted in green*) within the smallest amount of steps you can find.
You may move the target piece or any other piece too.
Pieces move in one of the four directions *NORTH, EAST, SOUTH, WEST*.
However in one step a piece always moves as far as possible until hitting any obstacle.
Obstacles on the board are walls on the one hand and other pieces on the other hand.
One move in one direction is countet as a step.
The steps of all pieces are summed together to obtain the total number of steps needed.

So in the example screenshot above, an optimal solution would be to move the blue piece to the east, followed by the green piece to the south and to the west.
This is the only optimal solution here and it needs three steps in total. 

### The implementation's goal
* enable different numbers of pieces (e.g. 1 to 20) (_DONE_)
* offer a greater variety of boards to play on than other implementations. (_The ENGINE ALREADY SUPPORTS OTHER BOARDS BUT WE DID NOT YET ADD A NEW BOARD GENERATOR_)
* provide an efficient solver which prints out all optimal solutions in a way of first combining them into equivalence classes and then showing a representant with greatest prettiness-rate for each class. (_DONE, BUT SOLVER EFFICIENCY NEEDS TO BE IMPROVED_)
* Collect some statistical data abou the computational problem. (_UNTOUCHED_)

### **We will add a more detailed description here once we left Beta status.**


## Get Started For Development
### Install Dependencies
   * Windows:
      * Install **cmake**
      * Install the latest C++ compiler with **Visual Studio**.
      * Install **Qt6** or Qt5 (Qt6 is recommended). We recommend to [download the official Installer of **Qt Creator**](https://www.qt.io/product/development-tools). Alternatively, to install Qt from command line using **aqt**, see our [Azure Pipeline configuration](https://github.com/Necktschnagge/tobor-games/blob/main/azure-pipelines.yml) (not recommended for Desktop Development).
      * Remark: You may use the MinGW toolchain shipped with the Qt Creator IDE. However we do not recommend this. Instead make sure that you have installed the MSVC 2019 component of your selected Qt version. For this use Qt Maintenance Tool.
      * Check that system environment variables are set to enable cmake to find Qt when running `*--create-project.[ps1|sh|bat]` script at later steps, e.g.:
         * `QT_DIR = C:\Qt`
         * `PATH = C:\Qt\6.5.0\msvc2019_64\bin;...`
   * Linux:
      * Install cmake, gcc/g++, Qt (Qt6 is recommended, Qt5 is supported)
      * Installing Qt [using the official Installer of **Qt Creator**](https://www.qt.io/product/development-tools) is recommended.
      * Alternatively, you may use the system's package manager:
          * for Qt5 (e.g. *Ubuntu 20.04*) run
            
            `apt-get install build-essential cmake qtbase5-dev libqt5svg5-dev`
          * for Qt6 (e.g. *Ubuntu 22.04*) run
          
            `apt-get install build-essential cmake libgl1-mesa-dev qt6-base-dev libqt6svg6-dev`
      * See our [Azure Pipeline configuration](https://github.com/Necktschnagge/tobor-games/blob/main/azure-pipelines.yml) for details.
   * MacOS:
      * *See instructions for Linux. You may use clang instead of g++.*
      * See our [Azure Pipeline configuration](https://github.com/Necktschnagge/tobor-games/blob/main/azure-pipelines.yml) for details.
   * All OS:
      * Install **Doxygen and graphviz** to enable documentation creation locally
### Setup Local Repository
   * Clone this repository.
   * `cd` into directory `script/
   * Run `*-clean-init.[ps1|sh|bat]` to update (in particular *initialize*) the git submodules
   * Run `*--create-project.[ps1|sh|bat]` to create a project for your custom toolchain
   * Run `*-build.[ps1|sh|bat]` to build your entire CMake Project, including multiple targets
   
   * On Windows you may use `wos-win-open-sln.ps1` to open your project in Visual Studio
   * You may use Qt Creator to open a cmake project instead of running `*--create-project.[ps1|sh|bat]` scripts and following.

### Qt6 versus Qt5
   * We recommend using the latest version **Qt6**. However we support Qt5 as well.
   * To build your project with Qt5, use the flag `-DUSE_QT6:BOOL=FALSE` for creating projects with cmake instead of just running script `*--create-project.[ps1|sh|bat]`. The complete command to create a project using Qt5 is
```
cmake -S . -B ./build/ -DUSE_QT6:BOOL=FALSE
```
   * When opening a cmake project with Qt Creator, you may select your toolchain while clicking through the project generation assistant.
