#!/bin/bash
# Build and run the program using the example CMakeLists.txt. The program is
# named after the first argument and the source code is located at /COMP371

PROJECT_NAME="$1"
SOURCE_PATH=/COMP371

# List source paths
echo "Listing ${SOURCE_PATH} Contents..."
ls -Rl ${SOURCE_PATH}/*

# Build
cmake -S /examples -B /build -DPROJECT_NAME="${PROJECT_NAME}" -DSOURCE_PATH="${SOURCE_PATH}"
cmake --build /build

# Run
/build/"${PROJECT_NAME}"
