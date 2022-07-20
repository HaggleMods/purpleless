@echo off
echo Updating submodules...
git submodule update --init --remote --jobs=16
echo Generating project files...
call "tools/premake5.exe" vs2022 --file="lua/purpleless.lua"