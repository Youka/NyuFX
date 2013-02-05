for pi, pixel in ipairs(convert.text_to_pixels(lines[1].text, lines[1].styleref)) do
	print(string.format("Pos: %d|%d\nAlpha: %d\n", pixel.x, pixel.y, pixel.a))
end