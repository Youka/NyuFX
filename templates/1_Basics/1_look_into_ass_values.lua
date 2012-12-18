--[[
This script visualizes which ASS values you got from input ASS file.
Execute it with "in.ass" and open "ASS_listing.txt" in this folder.
You'll discover how ASS contents, like video resolution, styles, lines etc.
are stored into Lua tables.
It's important to understand it, because these Lua tables and their elements
are exactly the values you're working with the whole time to create karaoke effects.
]]
local log_file = "templates\\1_Basics\\ASS_listing.txt"

print( "meta\n" .. table.tostring(meta) , log_file)
print( "styles\n" .. table.tostring(styles) , log_file)
print( "lines\n" .. table.tostring(lines) , log_file)
