#!/bin/bash

# Compile main.c using clang
echo "Compiling main.c..."
clang main.c cd.c dir.c splash.c -o ripple

# Check if compilation was successful
if [ $? -eq 0 ]; then
  echo "Compilation successful."
else
  echo "Compilation failed."
fi

