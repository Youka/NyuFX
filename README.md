# NyuFX
---
##Definition
**NyuFX** is an application to create ASS (Advanced Substation Alpha) files with karaoke effects.

Editors for working with Lua and ASS files, text & progressbar output fields and some other features are part of the graphical user interface for comfortable effect creation.

The environment of Lua, the scripting language for effect design, is extended by functions and variables to make NyuFX more than a text manipulation program. 
Pre-calculated values and graphic-system functions allow advanced effects without much effort.

It's recommend to learn Lua and ASS before usage, but even without it's possible to use NyuFX because of easy learning with some examples.

##Build
NyuFX has a project file for MSVC++9 (see ***src/NyuFX.vcproj***).

* Open it.
* Change the dependency to ***wxWidgets*** in the NyuFX project settings. Fix the *include*, *library* and *resources* path to your own wxWidgets 2.9.4 built.
* Build the project map.


For the installer you need NSIS to execute the script ***installer/installer.nsi***, but before add *ASSDraw3.exe* and *ASSDraw3.chm* (comes with Aegisub) to complete required files.

##Help
Forum: http://forum.youka.de