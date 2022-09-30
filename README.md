# cmake_cpp_template

This is a **CMake C++** project template.

You may use it when starting an own project instead of starting your CMake setup with great effort from scratch.

It is inteded to work on multiple platforms:
* Windows / Visual Studio / MSVC compiler
* MacOs / clang compiler
* Ubuntu / gcc/g++

## Setup your own repository using this template as your convenient basis

1. Create your own repository (without any commits, i.e. especially without readme, license, etc) on e.g. github.com
2. On your computer clone your own repository: `git clone https://server.domain/url/to/your/repo`
3. Execute the following git commands locally:
   * `git remote add template https://github.com/Necktschnagge/cmake_cpp_template`
   * `git fetch template`
   * `git switch -c main template/main`
   * `git push --set-upstream origin main`
4. Edit the script `init_project_and_script_files.sh` at the root of your repository. There you need to set:
   * a project name
   * the uri to origin
5. Run `init_project_and_script_files.sh` from shell on MacOS or Linux. For Windows, you probably have *git bash* installed, which you can use to execute the script properly.

Now you will see staged changes. Commit them to the branch `main`.

Now you are done and can start developing your C++ code. Go to the directory `script/`. There you find all scripts to
* clean your working space
* let CMake create a project for your platform
* let CMake build the project created previously
* do other stuff...
