local line = table.copy(lines[1])
line.style = "p" --Style for pixels with f.e. \an7, \bord0, \shad0, ...
local rect = shape.rectangle(1,1)
for pi, pixel in ipairs( convert.shape_to_pixels("m 0 0 l 30 0 30 30 0 30") ) do
	local x, y = math.round(line.left + pixel.x), math.round(line.top + pixel.y)
	local alpha = (pixel.a == 255 and "") or ("\\1a" .. convert.a_to_ass(pixel.a))
	line.text = string.format("{\\pos(%d,%d)%s\\p1}%s", x, y, alpha, rect)
	io.write_line(line)
end