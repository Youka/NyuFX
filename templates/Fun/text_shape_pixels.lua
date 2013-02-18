-- Line to draw
local line = lines[1]
local l = table.copy(line)

-- Text
l.effect = "text"
l.text = string.format("{\\bord0}%s", line.text)
io.write_line(l)

-- Shape
l.effect = "shape"
l.text = string.format("{\\bord0\\an7\\pos(%.3f,%.3f)\\p4}%s", line.left, line.top + line.height, convert.text_to_shape(line.text, line.styleref))
io.write_line(l)

-- Pixels
l.effect = "pixel"
for _, pixel in ipairs(convert.text_to_pixels(line.text, line.styleref, line.left % 1, (line.top + line.height * 2) % 1)) do
	local x, y = math.floor(line.left) + pixel.x, math.floor(line.top + line.height * 2) + pixel.y
	local a = pixel.a == 255 and "" or "\\1a" .. convert.a_to_ass(pixel.a)
	l.text = string.format("{\\bord0\\an7\\pos(%d,%d)%s\\p1}m 0 0 l 1 0 1 1 0 1", x, y, a)
	io.write_line(l)
end