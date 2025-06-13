@echo off
REM build_web.bat for Windows
call D:\Unity\emsdk\emsdk_env.bat

REM Check what files we have
REM echo Checking source files:
REM dir src\*.c
REM dir src\util\*.c

REM echo Checking assets:
REM dir src\assets\

REM Clean previous build
if exist asteroids.html del asteroids.html
if exist asteroids.js del asteroids.js
if exist asteroids.wasm del asteroids.wasm
if exist asteroids.data del asteroids.data

REM Collect all .c files
set "SOURCES="
for %%F in (src\*.c src\util\*.c) do set "SOURCES=%SOURCES% %%F"

REM Build for web
emcc -O3 ^
  -s USE_SDL=3 ^
  -s WASM=1 ^
  -s ALLOW_MEMORY_GROWTH=1 ^
  -s MAX_WEBGL_VERSION=2 ^
  -s MIN_WEBGL_VERSION=2 ^
  -s ASYNCIFY ^
  -s TOTAL_STACK=2097152 ^
  -s INITIAL_MEMORY=33554432 ^
  -s EXPORTED_FUNCTIONS="[\"_main\"]" ^
  -s EXPORTED_RUNTIME_METHODS="[\"ccall\",\"cwrap\"]" ^
  --preload-file src/assets@/assets ^
  -o asteroids.html ^
  src/main.c src/asteroid.c src/audio.c src/button.c src/text.c src/util/array_list.c -lm

REM Check if build succeeded
if exist asteroids.html (
    echo Build successful! Files created:
    dir asteroids.*
    echo.
    echo To test locally, run a web server:
    echo python -m http.server 8000
    echo Then open http://localhost:8000/asteroids.html
) else (
    echo Build failed!
    exit /b 1
)