-- Create image table
local image = table.create(480*100*4, 3)
image.width = 480
image.height = 100
image.has_alpha = true
for i = 1, image.width * image.height * 4 do
	image[i] = 0
end
-- Get text pixels
local ctx = tgdi.create_context()
local pixel_palette = ctx:add_path("Hello world!", "Times New Roman", 90*8, true, false, false, false, 1):get_pixels(true)
-- Draw pixels to image
local x, y = 10, 4
for pixel_y = 0, pixel_palette.height-1 do
	for pixel_x = 0, pixel_palette.width-1 do
		local pixel_i = 1 + pixel_y * pixel_palette.width + pixel_x
		local pixel_alpha = pixel_palette[pixel_i]
		-- Pixel visible?
		if pixel_alpha > 0 then
			local image_x, image_y = x + pixel_x, y + pixel_y
			-- Pixel in image?
			if image_x >= 0 and image_x < image.width and image_y >= 0 and image_y < image.height then
				local image_i = 1 + image_y * image.width * 4 + image_x * 4
				-- Set pixel color in image
				local y_pct = image_y / (image.height-1)
				image[image_i] = y_pct * 255	-- Red
				image[image_i+1] = (1-y_pct) * 255	-- Green
				image[image_i+2] = math.sin(y_pct * math.pi) * 255	-- Blue
				image[image_i+3] = pixel_alpha	-- Alpha
			end
		end
	end
end
-- Save image
tgdi.save_png(image, "templates\\Fun\\text.png")