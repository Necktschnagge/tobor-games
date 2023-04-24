#!/bin/bash

# EDIT THE FOLLOWING LINES!


# Give your project a good name!
project_name="Tobor"

# Tell where your remote is, be aware of escaping `/`!:
sources_url="https:\/\/github.com\/Necktschnagge\/cmake-qt-prototype"


# HERE YOU ARE DONE!
#--------------------------------------------------------


# Your project name will be inserted at the right spots:
sed -i -E "s/___template___project___name___/${project_name}/g" script/*.sh script/*.ps1 script/*.bat CMakeLists.txt azure-pipelines.yml

# Your remote location will be inserted at the right spots:
sed -i -E "s/___template___project___url___/${sources_url}/g" CMakeLists.txt

# Changes will be staged. Commiting them is up to you.
git update-index --chmod +x script/*.sh
git reset --mixed
git add *
