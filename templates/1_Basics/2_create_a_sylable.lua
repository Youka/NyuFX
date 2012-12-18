--[[
This script creates your first dialog line in "out.ass".
The magic function is 'io.write_line', which converts a dialog line in table form
back to text form and appends it to "out.ass".
To show the first manipulation, the line text was changed to text of first sylable.
It's not a good idea doing it this way because the original line text is overwritten.
Instead always create a copy of line to save the original.
]]
lines[1].text = lines[1].syls[1].text
io.write_line( lines[1] )