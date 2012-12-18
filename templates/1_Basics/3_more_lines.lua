--[[
Let's go a bit further:
Using a generic for-loop with function 'ipairs' we iterate through elements of lines table.
It's similar to that:

for li=1, #lines do
	local line = lines[li]
	...
end

First we're creating a line copy of each line for overwriting.
The start time will be incremented by 2000ms.
Finally these time-shifted lines will be written to "out.ass".
]]
for li, line in ipairs(lines) do
	local l = table.copy(line)
	l.start_time = line.start_time + 2000
	io.write_line(l)
end