local line = lines[1]
local l = table.copy(line)

--Original line
l.actor = "line"
l.text = string.format("{\\pos(%.3f,%.3f)}%s", line.x, line.y, line.text)
io.write_line(l)

--Line shape
l.actor = "shape"
l.text = string.format("{\\an7\\pos(%.3f,%.3f)\\p4}%s", line.left, line.top + line.height, convert.text_to_shape(line.text, line.styleref))
io.write_line(l)

--Line pixels
l.actor = "pixel"
l.style = "p"
for pi, pixel in ipairs( convert.text_to_pixels(line.text, line.styleref) ) do
	local x, y = math.round(line.left) + pixel.x, math.round(line.top) + pixel.y
	local a = pixel.a == 255 and "" or "\\1a" .. convert.a_to_ass(pixel.a)
	l.text = string.format("{\\pos(%d,%d)%s\\p1}m 0 0 l 1 0 1 1 0 1", x, y + 2 * line.height, a)
	io.write_line(l)
end