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

The only thing to change in the project settings is the dependency to wxWidgets. Fix the **include** and **library** path to your own wxWidgets 2.9.4 built.

##Help
Forum: http://forum.youka.de