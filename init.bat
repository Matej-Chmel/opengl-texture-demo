@echo off

set scriptDir=%~dp0
set scriptDir=%scriptDir:~0, -1%
cd %scriptDir%

git submodule update --init --recursive --remote
cd %scriptDir%\any-project-setup
py -m src.zipDownloader.main %scriptDir%\res\cfg\libs.json
