local line = table.copy(lines[1])
line.style = "p" --Style for pixels with f.e. \an7, \bord0, \shad0, ...
local img = convert.image_to_pixels("templates\\3_Function tests\\shield.png")
local rect = shape.rectangle(1,1)
for pi, pixel in ipairs(img) do
	if pixel.a > 0 then
		local x, y = 300 + pixel.x, 200 + pixel.y
		local color = convert.rgb_to_ass(pixel.r, pixel.g, pixel.b)
		local alpha = (pixel.a == 255 and "") or ("\\1a" .. convert.a_to_ass(pixel.a))
		line.text = string.format("{\\pos(%d,%d)\\1c%s%s\\p1}%s", x, y, color, alpha, rect)
		io.write_line(line)
	end
end