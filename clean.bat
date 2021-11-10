@echo off

set scriptDir=%~dp0
set scriptDir=%scriptDir:~0, -1%
cd %scriptDir%\any-project-setup

py -m src.cleaner.main %scriptDir%\res\cfg\cleaner.json
