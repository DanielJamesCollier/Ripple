echo Compiling Ripple

clang-cl /std:c17 /Fe:ripple.exe main.c cd.c dir.c splash.c common.c new.c del.c gui.c mkdir.c clear.c
if %ERRORLEVEL% equ 0 (
  echo Compilation successful.
) else (
  echo Compilation failed.
)
