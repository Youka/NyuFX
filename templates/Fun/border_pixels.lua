-- Line to convert to pixels
local line = lines[1]

-- Text fill shape
local text_shape = convert.text_to_shape(line.text, line.styleref)
-- Shift text fill shape by text position subpixel offset (for correct antialiasing calculation by pixel conversion)
local x_shift, y_shift = math.floor(line.left % 1 * 8), math.floor(line.top % 1 * 8)
text_shape = shape.filter(text_shape, function(x, y)
	return x+x_shift, y+y_shift
end)
-- Text outline shape
local text_outline_shape = shape.tooutline(shape.split(text_shape), line.styleref.outline * 8)
-- Text fill & outline pixels
local text_pixels = convert.shape_to_pixels(text_shape)
local text_outline_pixels = convert.shape_to_pixels(text_outline_shape)
-- Subtract fill pixels from outline pixels to get border pixels
local text_border_pixels, text_border_pixels_n = {}, 0
for _, out_pixel in ipairs(text_outline_pixels) do
	local found_overlap = false
	for _, in_pixel in ipairs(text_pixels) do
		if out_pixel.x == in_pixel.x and out_pixel.y == in_pixel.y and in_pixel.a == 255 then	-- Pixels overlap and fill pixel is opaque
			found_overlap = true
			break
		end
	end
	if not found_overlap then
		text_border_pixels_n = text_border_pixels_n + 1
		text_border_pixels[text_border_pixels_n] = out_pixel
	end
end

-- Border pixels output
for _, pixel in ipairs(text_border_pixels) do
	local x, y = math.floor(line.left) + pixel.x, math.floor(line.top) + pixel.y
	local alpha = convert.a_to_ass(pixel.a)
	line.text = string.format("{\\an7\\pos(%d,%d)\\1a%s\\1c%s\\bord0\\p1}m 0 0 l 1 0 1 1 0 1", x, y, alpha, line.styleref.color3)
	io.write_line(line)
end
-- Fill pixels output
for _, pixel in ipairs(text_pixels) do
	local x, y = math.floor(line.left) + pixel.x, math.floor(line.top) + pixel.y
	local alpha = convert.a_to_ass(pixel.a)
	line.text = string.format("{\\an7\\pos(%d,%d)\\1a%s\\1c%s\\bord0\\p1}m 0 0 l 1 0 1 1 0 1", x, y, alpha, line.styleref.color1)
	io.write_line(line)
end