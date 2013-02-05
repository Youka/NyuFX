for pi, pixel in ipairs(convert.shape_to_pixels("m 0 -24 l 0 24 b 32 24 32 -24 0 -24")) do
	print(string.format("Pos: %d|%d\nAlpha: %d\n", pixel.x, pixel.y, pixel.a))
end