for pi, pixel in ipairs(convert.image_to_pixels("templates\\Tests\\Functions\\noise.png")) do
	print(string.format("Pos: %d|%d\nRGBA: %d %d %d %d\n", pixel.x, pixel.y, pixel.r, pixel.g, pixel.b, pixel.a))
end