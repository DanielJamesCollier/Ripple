#!/bin/bash

# Compile main.c using clang
echo "Compiling main.c..."
clang --std=c17 main.c cd.c dir.c splash.c common.c new.c del.c gui.c mkdir.c -o ripple

# Check if compilation was successful
if [ $? -eq 0 ]; then
  echo "Compilation successful."
else
  echo "Compilation failed."
fi

