-- BASE
function pack(...)
	return arg
end

-- CONVERT
convert = {}

function convert.a_to_ass(a)
	if type(a) ~= "number" then
		error("number expected", 2)
	elseif a < 0 or a > 255 then
		return nil
	end
	return string.format("&H%02x&", 255-a)
end

function convert.ass_to_a(str)
	if type(str) ~= "string" then
		error("string expected", 2)
	end
	local a = str:match("&H(%x%x)&")
	if not a then
		return nil
	else
		return 255-tonumber(a, 16)
	end
end

function convert.rgb_to_ass(r,g,b)
	if type(r) ~= "number" or type(g) ~= "number" or type(b) ~= "number" then
		error("number, number and number expected", 2)
	elseif r < 0 or r > 255 or g < 0 or g > 255 or b < 0 or b > 255 then
		return nil
	end
	return string.format("&H%02x%02x%02x&", b, g, r)
end

function convert.ass_to_rgb(str)
	if type(str) ~= "string" then
		error("string expected", 2)
	end
	local b, g, r = str:match("&H(%x%x)(%x%x)(%x%x)&")
	if not (b and g and r) then
		return nil
	else
		return tonumber(r, 16), tonumber(g, 16), tonumber(b, 16)
	end
end

function convert.shape_to_pixels(shape)
	-- Argument check
	if type(shape) ~= "string" then
		error("string expected", 2)
	end
	-- Result data
	local pixel_palette
	local shift_x, shift_y
	-- Safe execution
	local success = pcall(function()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Shift shape in valid bitmap section (shifts are 8-fold = subpixel save)
		local min_x, min_y
		shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
			min_x = min_x and math.min(min_x, x) or x
			min_y = min_y and math.min(min_y, y) or y
		end)
		shift_x, shift_y = min_x >= 0 and min_x % 8 - min_x or 8-(-min_x % 8) - min_x, min_y >= 0 and min_y % 8 - min_y or 8-(-min_y % 8) - min_y
		shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
			return string.format("%d %d", x + shift_x, y + shift_y)
		end)
		-- Get pixels
		ctx:add_path(shape)
		pixel_palette = ctx:get_pixels(true)
	end)
	if not success then
		error("invalid shape", 2)
	end
	-- Convert pixel palette to pixel table with right-shifted positions and return
	local pixels, pixels_n = {}, 0
	shift_x, shift_y = shift_x / -8, shift_y / -8
	for pi, alpha in ipairs(pixel_palette) do
		if alpha > 0 then
			pixels_n = pixels_n + 1
			pixels[pixels_n] = {a = alpha, x = (pi-1) % pixel_palette.width + shift_x, y = math.floor((pi-1) / pixel_palette.width) + shift_y}
		end
	end
	return pixels
end

function convert.text_to_shape(text, style)
	-- Flat arguments check
	if type(text) ~= "string" or type(style) ~= "table" then
		error("string and style table expected", 2)
	end
	-- Result data
	local shape
	-- Safe execution
	local success = pcall(function()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Text with spacing
		if style.spacing > 0 then
			local shape_collection, shape_collection_n = table.create(text:ulen(),0), 0
			local current_x = 0
			for uchar_i, uchar in text:uchars() do
				ctx:add_path(uchar, style.fontname, style.fontsize * 64, style.bold, style.italic, style.underline, style.strikeout, style.encoding)
				shape = ctx:get_path()
				ctx:abort_path()
				shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
					return string.format("%d %d", x + current_x, y)
				end)
				shape_collection_n = shape_collection_n + 1
				shape_collection[shape_collection_n] = shape
				current_x = current_x + ctx:text_extents(uchar, style.fontname, style.fontsize * 64, style.bold, style.italic, style.underline, style.strikeout, style.encoding) + style.spacing * 64
			end
			shape = table.concat(shape_collection, " ")
		-- Text without spacing
		else
			ctx:add_path(text, style.fontname, style.fontsize * 64, style.bold, style.italic, style.underline, style.strikeout, style.encoding)
			shape = ctx:get_path()
		end
		-- Scale correctly
		local scale_x, scale_y = style.scale_x / 100 / 8, style.scale_y / 100 / 8
		shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
			return string.format("%d %d", x * scale_x, y * scale_y)
		end)
	end)
	if not success then
		error("invalid text or style table", 2)
	end
	return shape
end

function convert.text_to_pixels(text, style, off_x, off_y)
	-- Flat arguments check
	if type(text) ~= "string" or type(style) ~= "table" or
	not(
		(off_x == nil and off_y == nil) or
		(type(off_x) == "number" and type(off_y) == "number")
	) then
		error("string, style table and optional number and number expected", 2)
	elseif off_x and (off_x < 0 or off_x >= 1 or off_y < 0 or off_y >= 1) then
		error("offset has to be in range 0<=x<1", 2)
	end
	-- Result data
	local pixels
	-- Safe execution
	local success = pcall(function()
		if not text:find("[^%s]") then
			pixels = {}
		elseif off_x and (off_x > 0 or off_y > 0) then
			off_x, off_y = math.floor(off_x * 8), math.floor(off_y * 8)
			pixels = convert.shape_to_pixels(
				convert.text_to_shape(text, style):gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
					return string.format("%d %d", x+off_x, y+off_y)
				end)
			)
		else
			pixels = convert.shape_to_pixels(convert.text_to_shape(text, style))
		end
	end)
	if not success then
		error("invalid text or style table", 2)
	end
	return pixels
end

function convert.image_to_pixels(image)
	if type(image) ~= "string" then
		error("string expected", 2)
	end
	-- Image data
	local pixel_palette
	-- Safe execution
	local success = pcall(function()
		-- Get image
		pixel_palette = tgdi.load_image(image)
	end)
	if not success then
		error("invalid image", 2)
	end
	-- Convert pixel palette to RGBA pixel table
	local pixels, pixels_n = table.create(pixel_palette.width * pixel_palette.height, 0), 0
	if pixel_palette.has_alpha then
		for y = 0, pixel_palette.height-1 do
			for x = 0, pixel_palette.width-1 do
				local i = 1 + y * pixel_palette.width * 4 + x * 4
				local a = pixel_palette[i+3]
				if a > 0 then
					local r = pixel_palette[i]
					local g = pixel_palette[i+1]
					local b = pixel_palette[i+2]
					pixels_n = pixels_n + 1
					pixels[pixels_n] = {r = r, g = g, b = b, a = a, x = x, y = y}
				end
			end
		end
	else
		for y = 0, pixel_palette.height-1 do
			for x = 0, pixel_palette.width-1 do
				local i = 1 + y * pixel_palette.width * 3 + x * 3
				local r = pixel_palette[i]
				local g = pixel_palette[i+1]
				local b = pixel_palette[i+2]
				pixels_n = pixels_n + 1
				pixels[pixels_n] = {r = r, g = g, b = b, a = 255, x = x, y = y}
			end
		end
	end
	return pixels
end

-- MATH
function math.bezier(pct, p)
	if type(pct) ~= "number" or type(p) ~= "table" then
		error("number and table expected", 2)
	elseif pct < 0 or pct > 1 or #p < 2 then
		error("invalid arguments", 2)
	end
	for i, v in ipairs(p) do
		if type(v[1]) ~= "number" or type(v[2]) ~= "number" or (type(v[3]) ~= "number" and type(v[3]) ~= "nil") then
			error("invalid table content", 2)
		end
	end
	--Factorial
	local function fac(n)
		local k = 1
		if n > 1 then
			for i=2, n do
				k = k * i
			end
		end
		return k
	end
	--Binomial coefficient
	local function bin(i, n)
		return fac(n) / (fac(i) * fac(n-i))
	end
	--Bernstein polynom
	local function bernstein(pct, i, n)
		return bin(i, n) * pct^i * (1 - pct)^(n - i)
	end
	--Calculate coordinate
	local point = {0, 0, 0}
	local n = #p - 1
	for i=0, n do
		local bern = bernstein(pct, i, n)
		point[1] = point[1] + p[i+1][1] * bern
		point[2] = point[2] + p[i+1][2] * bern
		point[3] = point[3] + (p[i+1][3] or 0) * bern
	end
	return point
end

function math.distance( w, h, d )
	if type(w) ~= "number" or type(h) ~= "number" or (type(d) ~= "number" and type(d) ~= "nil") then
		error("number, number and optional number expected", 2)
	end
	if d then
		return math.sqrt(w^2 + h^2 + d^2)
	else
		return math.sqrt(w^2 + h^2)
	end
end

function math.ellipse(x, y, w, h, a)
	if type(x) ~= "number" or
	type(y) ~= "number" or
	type(w) ~= "number" or
	type(h) ~= "number" or
	type(a) ~= "number" then
		error("number, number, number, number and number expected", 2)
	end
	local ra = math.rad(a)
	return x + w/2 * math.cos(ra), y + h/2 * math.sin(ra)
end

function math.randomsteps(start, ends, step)
	if type(start) ~= "number" or
	type(ends) ~= "number" or
	type(step) ~= "number" or
	start > ends or
	step <= 0 then
		error("valid number, number and number expected", 2)
	end
	return start + math.random(0, math.floor((ends - start) / step)) * step
end

function math.randomway()
	return math.random(0,1) * 2 - 1
end

function math.rotate(p, axis, angle)
	if type(p) ~= "table" or #p ~= 3 or type(axis) ~= "string" or type(angle) ~= "number" then
		error("table, string and number expected", 2)
	elseif axis ~= "x" and axis ~= "y" and axis ~= "z" then
		error("invalid axis", 2)
	elseif type(p[1]) ~= "number" or type(p[2]) ~= "number" or type(p[3]) ~= "number" then
		error("invalid table content", 2)
	end
	local ra = math.rad(angle)
	if axis == "x" then
		return {
			p[1],
			math.cos(ra)*p[2] - math.sin(ra)*p[3],
			math.sin(ra)*p[2] + math.cos(ra)*p[3]
		}
	elseif axis == "y" then
		return {
			math.cos(ra)*p[1] + math.sin(ra)*p[3],
			p[2],
			-math.sin(ra)*p[1] + math.cos(ra)*p[3]
		}
	else
		return {
			math.cos(ra)*p[1] - math.sin(ra)*p[2],
			math.sin(ra)*p[1] + math.cos(ra)*p[2],
			p[3]
		}
	end
end

function math.round(num)
	if type(num) ~= "number" then
		error("number expected", 2)
	end
	return math.floor(num + 0.5)
end

function math.trim(num, starts, ends)
	if type(starts) ~= "number" or
	type(ends) ~= "number" or
	type(num) ~= "number" then
		error("number, number and number expected", 2)
	end
	return (num < starts and starts) or (num > ends and ends) or num
end

-- SHAPE
shape = {}

function shape.bounding(shape)
	if type(shape) ~= "string" or (type(as_region) ~= "boolean" and as_region ~= nil) then
		error("string and optional boolean expected", 2)
	end
	-- Result data
	local min_x, min_y, max_x, max_y
	-- Safe execution
	local success = pcall(function()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Get bounding
		shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
			return string.format("%d %d", x * 8, y * 8)
		end)
		ctx:add_path(shape)
		min_x, min_y, max_x, max_y = ctx:path_box()
		min_x, min_y, max_x, max_y = math.floor(min_x/8), math.floor(min_y/8), math.floor(max_x/8), math.floor(max_y/8)
	end)
	if not success then
		error("invalid shape", 2)
	end
	return min_x, min_y, max_x, max_y
end

function shape.ellipse(w, h)
	if type(w) ~= "number" or type(h) ~= "number" then
		error("number and number expected", 2)
	end
	local w2, h2 = w/2, h/2
	return string.format("m %d %d b %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
						0, h2,	-- move
						0, h2, 0, 0, w2, 0,	-- curve 1
						w2, 0, w, 0, w, h2,	-- curve 2
						w, h2, w, h, w2, h,	-- curve 3
						w2, h, 0, h, 0, h2	-- curve 4
	)
end

function shape.filter(shape, filter)
	if type(shape) ~= "string" or type(filter) ~= "function" then
		error("string and function expected", 2)
	end
	local new_shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
		local new_x, new_y = filter(tonumber(x), tonumber(y))
		if type(new_x) == "number" and type(new_y) == "number" then
			return string.format("%d %d", new_x, new_y)
		end
	end)
	return new_shape
end

function shape.glance(edges, inner_size, outer_size)
	if type(edges) ~= "number" or type(inner_size) ~= "number" or type(outer_size) ~= "number" or edges < 2 then
		error("valid 3 numbers expected", 2)
	end
	-- Build shape
	local shape, shape_n = {string.format("m 0 %d b", -outer_size)}, 1
	local inner_p, outer_p
	for i = 1, edges do
		--Inner edge
		inner_p = math.rotate({0, -inner_size, 0}, "z", ((i-0.5) / edges)*360)
		--Outer edge
		outer_p = math.rotate({0, -outer_size, 0}, "z", (i / edges)*360)
		-- Add curve
		shape_n = shape_n + 1
		shape[shape_n] = string.format(" %d %d %d %d %d %d", inner_p[1], inner_p[2], inner_p[1], inner_p[2], outer_p[1], outer_p[2])
	end
	shape = table.concat(shape)
	-- Shift to positive numbers
	local min_x, min_y = 0, 0
	shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
		min_x, min_y = math.min(min_x, x), math.min(min_y, y)
	end)
	shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
		return string.format("%d %d", x-min_x, y-min_y)
	end)
	-- Return result
	return shape
end

function shape.heart(size, offset)
	if type(size) ~= "number" or type(offset) ~= "number" then
		error("number and number expected", 2)
	end
	-- Build shape from template
	local shape = string.gsub("m 15 30 b 27 22 30 18 30 14 30 8 22 0 15 10 8 0 0 8 0 14 0 18 3 22 15 30", "%d+", function(num)
		num = num / 30 * size
		return math.round(num)
	end)
	-- Shift mid point of heart vertically
	shape = shape:gsub("(m %-?%d+ %-?%d+ b %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ %-?%d+ )(%-?%d+)", function(head, y)
		return string.format("%s%d", head, y + offset)
	end)
	-- Return result
	return shape
end

function shape.move(shape, x, y)
	if type(shape) ~= "string" or type(x) ~= "number" or type(y) ~= "number" then
		error("string, number and number expected", 2)
	end
	local new_shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(px, py)
		return string.format("%d %d", px + x, py + y)
	end)
	return new_shape
end

function shape.rectangle(w, h)
	if type(w) ~= "number" or type(h) ~= "number" then
		error("number and number expected", 2)
	end
	return string.format("m 0 0 l %d 0 %d %d 0 %d 0 0", w, w, h, h)
end

function shape.ring(out_r, in_r)
	if type(out_r) ~= "number" or type(in_r) ~= "number" or in_r >= out_r then
		error("valid number and number expected", 2)
	end
	local out_r2, in_r2 = out_r*2, in_r*2
	local off = out_r - in_r
	return string.format("m %d %d b %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d m %d %d b %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		0, out_r,	-- outer move
		0, out_r, 0, 0, out_r, 0,	-- outer curve 1
		out_r, 0, out_r2, 0, out_r2, out_r,	-- outer curve 2
		out_r2, out_r, out_r2, out_r2, out_r, out_r2,	-- outer curve 3
		out_r, out_r2, 0, out_r2, 0, out_r,	-- outer curve 4
		off, off+in_r,	-- inner move
		off, off+in_r, off, off+in_r2, off+in_r, off+in_r2,	-- inner curve 1
		off+in_r, off+in_r2, off+in_r2, off+in_r2, off+in_r2, off+in_r,	-- inner curve 2
		off+in_r2, off+in_r, off+in_r2, off, off+in_r, off,	-- inner curve 3
		off+in_r, off, off, off, off, off+in_r	-- inner curve 4
	)
end

function shape.split(shape, len)
	if type(shape) ~= "string" or (type(len) ~= "number" and len ~= nil) then
		error("string and optional number expected", 2)
	elseif len and len <= 0 then
		error("invalid length", 2)
	end
	-- Curves to lines
	local success = pcall(function()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Flatten path
		ctx:add_path(shape)
		ctx:flatten_path()
		shape = ctx:get_path()
	end)
	if not success then
		error("invalid shape", 2)
	end
	-- Split lines to shorter segments
	if len then
		local line_mode, last_point = false
		shape = shape:gsub("([ml]?)(%s*)(%-?%d+)%s+(%-?%d+)", function(typ, space, x, y)
			-- Update line mode
			if typ == "m" then
				line_mode = false
			elseif typ == "l" then
				line_mode = true
			end
			-- Line(s) buffer
			local lines
			-- LineTo with previous point?
			if line_mode and last_point then
				local rel_x, rel_y = x-last_point.x, y-last_point.y
				local distance = math.distance(rel_x, rel_y)
				-- Can split?
				if distance > len then
					lines = {typ .. space}
					local lines_n = 1
					local dist_rest = distance % len
					for cur_dist = dist_rest > 0 and dist_rest or len, distance, len do
						local pct = cur_dist / distance
						lines_n = lines_n + 1
						lines[lines_n] = string.format("%d %d ", last_point.x + rel_x * pct, last_point.y + rel_y * pct)
					end
					lines = table.concat(lines):sub(1,-2)
				else
					lines = string.format("%s%s%d %d", typ, space, x, y)
				end
			else
				lines = string.format("%s%s%d %d", typ, space, x, y)
			end
			-- Save point for next one
			last_point = {x = x, y = y}
			-- Return new line(s)
			return lines
		end)
	end
	-- Return result
	return shape
end

function shape.star(edges, inner_size, outer_size)
	if type(edges) ~= "number" or type(inner_size) ~= "number" or type(outer_size) ~= "number" or edges < 2 then
		error("valid 3 numbers expected", 2)
	end
	-- Build shape
	local shape, shape_n = {string.format("m 0 %d l", -outer_size)}, 1
	local inner_p, outer_p
	for i = 1, edges do
		-- Inner edge
		inner_p = math.rotate({0, -inner_size, 0}, "z", ((i-0.5) / edges)*360)
		-- Outer edge
		outer_p = math.rotate({0, -outer_size, 0}, "z", (i / edges)*360)
		-- Add lines
		shape_n = shape_n + 1
		shape[shape_n] = string.format(" %d %d %d %d", inner_p[1], inner_p[2], outer_p[1], outer_p[2])
	end
	shape = table.concat(shape)
	-- Shift to positive numbers
	local min_x, min_y = 0, 0
	shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
		min_x, min_y = math.min(min_x, x), math.min(min_y, y)
	end)
	shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", function(x, y)
		return string.format("%d %d", x-min_x, y-min_y)
	end)
	-- Return result
	return shape
end

function shape.tooutline(shape, size)
	if type(shape) ~= "string" or type(size) ~= "number" then
		error("string and number expected", 2)
	end
	-- Collect figures
	local figures, figures_n = {}, 0
	local figure, figure_n = {}, 0
	shape:gsub("(%a?)%s*(%-?%d+)%s+(%-?%d+)", function(typ, x, y)
		-- Check point type
		if typ ~= "m" and typ ~= "l" and typ ~= "" then
			error("shape have to contain only \"moves\" and \"lines\"", 4)
		end
		-- Last figure finished?
		if typ == "m" and figure_n ~= 0 then
			-- Enough figure points?
			if figure_n < 3 then
				error("at least one figure hasn't enough points", 4)
			end
			-- Save figure
			figures_n = figures_n + 1
			figures[figures_n] = figure
			figure = {}
			figure_n = 0
		end
		-- Add point to current figure
		figure_n = figure_n + 1
		figure[figure_n] = {x, y}
	end)
	-- Insert last figure
	if figure_n ~= 0 then
		-- Enough figure points?
		if figure_n < 3 then
			error("at least one figure hasn't enough points", 2)
		end
		-- Save figure
		figures_n = figures_n + 1
		figures[figures_n] = figure
		figure = {}
		figure_n = 0
	end
	-- Remove double points (recreate figures)
	for fi = 1, figures_n do
		local old_figure, old_figure_n = figures[fi], #figures[fi]
		local new_figure, new_figure_n = table.create(old_figure_n, 0), 0
		for pi, point in ipairs(old_figure) do
			local pre_point
			if pi == 1 then
				pre_point = old_figure[old_figure_n]
			else
				pre_point = old_figure[pi-1]
			end
			if not (point[1] == pre_point[1] and point[2] == pre_point[2]) then
				new_figure_n = new_figure_n + 1
				new_figure[new_figure_n] = point
			end
		end
		figures[fi] = new_figure
	end
	-- Vector functions for further calculations
	local function vec_sizer(vec, size)
		local len = math.distance(vec[1], vec[2])
		if len == 0 then
			return {0, 0}
		else
			return {vec[1] / len * size, vec[2] / len * size}
		end
	end
	local vecz = {0, 0, 1}
	local function vec_ortho(vec)
		local vec3d = {vec[1], vec[2], 0}
		return {
			vec3d[2] * vecz[3] - vec3d[3] * vecz[2],
			vec3d[3] * vecz[1] - vec3d[1] * vecz[3]
		}
	end
	local function vec_cross_deg(vec1, vec2)
		local degree = math.deg(
				math.acos(
					(vec1[1] * vec2[1] + vec1[2] * vec2[2]) /
					(math.distance(vec1[1], vec1[2]) * math.distance(vec2[1], vec2[2]))
				)
		)
		local dir = vec1[1]*vec2[2] - vec1[2]*vec2[1]
		return	dir > 0 and degree or dir < 0 and -degree or 0
	end
	-- Stroke figures
	local stroke_figures = {table.create(figures_n, 0),table.create(figures_n, 0)}	-- inner + outer
	local stroke_subfigures_i = 0
	-- Through figures
	for fi, figure in ipairs(figures) do
		stroke_subfigures_i = stroke_subfigures_i + 1
		-- One pass for inner, one for outer outline
		for i = 1, 2 do
			-- Outline buffer
			local outline, outline_n = {}, 0
			-- Point iteration order = inner or outer outline
			local loop_begin, loop_end, loop_steps
			if i == 1 then
				loop_begin, loop_end, loop_steps = #figure, 1, -1
			else
				loop_begin, loop_end, loop_steps = 1, #figure, 1
			end
			-- Iterate through figure points
			for pi = loop_begin, loop_end, loop_steps do
				-- Collect current, previous and next point
				local point = figure[pi]
				local pre_point, post_point
				if i == 1 then
					if pi == 1 then
						pre_point = figure[pi+1]
						post_point = figure[#figure]
					elseif pi == #figure then
						pre_point = figure[1]
						post_point = figure[pi-1]
					else
						pre_point = figure[pi+1]
						post_point = figure[pi-1]
					end
				else
					if pi == 1 then
						pre_point = figure[#figure]
						post_point = figure[pi+1]
					elseif pi == #figure then
						pre_point = figure[pi-1]
						post_point = figure[1]
					else
						pre_point = figure[pi-1]
						post_point = figure[pi+1]
					end
				end
				-- Calculate orthogonal vectors to both neighbour points
				local o_vec1 = vec_sizer( vec_ortho({point[1]-pre_point[1], point[2]-pre_point[2]}), size )
				local o_vec2 = vec_sizer( vec_ortho({post_point[1]-point[1], post_point[2]-point[2]}), size )
				-- Calculate degree & circumference between orthogonal vectors
				local degree = vec_cross_deg(o_vec1, o_vec2)
				local circ = math.abs(math.rad(degree)) * size
				-- Add first edge point
				outline_n = outline_n + 1
				outline[outline_n] = {math.floor(point[1] + o_vec1[1]), math.floor(point[2] + o_vec1[2])}
				-- Round edge needed?
				local max_circ = 2
				if circ > max_circ then
					local circ_rest = circ % max_circ
					for cur_circ = circ_rest > 0 and circ_rest or max_circ, circ, max_circ do
						local curve_vec = math.rotate({o_vec1[1], o_vec1[2], 0}, "z", cur_circ / circ * degree)
						outline_n = outline_n + 1
						outline[outline_n] = {math.floor(point[1] + curve_vec[1]), math.floor(point[2] + curve_vec[2])}
					end
				end
			end
			-- Insert inner or outer outline
			stroke_figures[i][stroke_subfigures_i] = outline
		end
	end
	-- Convert stroke figures to shape
	local stroke_shape, stroke_shape_n = {}, 0
	for fi = 1, figures_n do
		-- Closed inner outline to shape
		local inner_outline = stroke_figures[1][fi]
		for pi, point in ipairs(inner_outline) do
			stroke_shape_n = stroke_shape_n + 1
			stroke_shape[stroke_shape_n] = string.format("%s%d %d", pi == 1 and "m " or pi == 2 and "l " or "", point[1], point[2])
		end
		stroke_shape_n = stroke_shape_n + 1
		stroke_shape[stroke_shape_n] = string.format("%d %d", inner_outline[1][1], inner_outline[1][2])
		-- Closed outer outline to shape
		local outer_outline = stroke_figures[2][fi]
		for pi, point in ipairs(outer_outline) do
			stroke_shape_n = stroke_shape_n + 1
			stroke_shape[stroke_shape_n] = string.format("%s%d %d", pi == 1 and "m " or pi == 2 and "l " or "", point[1], point[2])
		end
		stroke_shape_n = stroke_shape_n + 1
		stroke_shape[stroke_shape_n] = string.format("%d %d", outer_outline[1][1], outer_outline[1][2])
	end
	return table.concat(stroke_shape, " ")
end

function shape.triangle(size)
	if type(size) ~= "number" then
		error("number expected", 2)
	end
	local h = math.sqrt(3) * size / 2
	local base = -h / 6
	return string.format("m %d %d l %d %d 0 %d %d %d", size/2, base, size, base+h, base+h, size/2, base)
end

-- STRING
--[[
UTF16 -> UTF8
--------------
U-00000000 ・U-0000007F: 	0xxxxxxx
U-00000080 ・U-000007FF: 	110xxxxx 10xxxxxx
U-00000800 ・U-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
U-00010000 ・U-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
U-00200000 ・U-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 ・U-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
]]--
function string.ucharrange(s, i)
	if type(s) ~= "string" or type(i) ~= "number" or i < 1 or i > s:len() then
		error("string and valid number expected", 2)
	end
	local byte = s:byte(i)
	if not byte then
		return 0
	elseif byte < 192 then
		return 1
	elseif byte < 224 then
		return 2
	elseif byte < 240 then
		return 3
	elseif byte < 248 then
		return 4
	elseif byte < 252 then
		return 5
	else
		return 6
	end
end

function string.uchars(s)
	if type(s) ~= "string" then
		error("string expected", 2)
	end
	local char_i, s_pos = 0, 1
	return function()
		if s_pos > s:len() then
			return nil
		end
		local cur_pos = s_pos
		s_pos = s_pos + s:ucharrange(s_pos)
		char_i = char_i + 1
		return char_i, s:sub(cur_pos, s_pos-1)
	end
end

function string.ulen(s)
	if type(s) ~= "string" then
		error("string expected", 2)
	end
	local n = 0
	for _ in s:uchars() do
		n = n + 1
	end
	return n
end

-- TABLE
function table.append(t1, t2)
	if type(t1) ~= "table" or type(t2) ~= "table" then
		error("table and table expected", 2)
	end
	local t1_n = #t1
	for i, v in pairs(t2) do
		if tonumber(i) then
			t1_n = t1_n + 1
			t1[t1_n] = v
		else
			t1[i] = v
		end
	end
end

function table.copy(old_t)
	if type(old_t) ~= "table" then
		error("table expected", 2)
	end
	local new_t = {}
	for index, value in pairs(old_t) do
		if type(value) == "table" then
		    new_t[index] = table.copy(value)
		else
            new_t[index] = value
		end
	end
	return new_t
end

function table.stack(...)
	-- Attribute
	local stack
	-- Member functions
	local ListFuncs = {}
	ListFuncs.__index = ListFuncs
	function ListFuncs:push(value)
		if value ~= nil then
			stack = {stack, value}
		end
	end
	function ListFuncs:pop()
		if stack then
			local value = stack[2]
			stack = stack[1]
			return value
		end
	end
	function ListFuncs:iter()
		local stack2 = stack
		return function()
			if stack2 then
				local value = stack2[2]
				stack2 = stack2[1]
				return value
			end
		end
	end
	function ListFuncs:size()
		local n = 0
		for _ in self:iter() do
			n = n + 1
		end
		return n
	end
	function ListFuncs:invert()
		local new_stack
		for value in self:iter() do
			new_stack = {new_stack, value}
		end
		stack = new_stack
	end
	function ListFuncs:totable()
		local t = table.create(self:size(), 0)
		local n = 0
		for value in self:iter() do
			n = n + 1
			t[n] = value
		end
		return t
	end
	-- Object creation
	local obj = setmetatable({}, ListFuncs)
	-- Initialization
	for _, value in ipairs(arg) do
		obj:push(value)
	end
	return obj
end

function table.max(t)
	if type(t) ~= "table" then
		error("table expected", 2)
	end
	local n = 0
	for _ in pairs(t) do
		n = n + 1
	end
	return n
end

function table.tostring(t)
	if type(t) ~= "table" then
		error("table expected", 2)
	end
	local result, result_n = {tostring(t)}, 1
	local function table_print(t, space)
		for i, v in pairs(t) do
			if type(i) == "string" then
				i = string.format("%q", i)
			end
			if type(v) == "string" then
				v = string.format("%q", v)
			end
			result_n = result_n + 1
			result[result_n] = string.format("%s[%s] = %s", space, tostring(i), tostring(v))
			if type(v) == "table" then
				table_print(v, space .. "\t")
			end
		end
	end
	table_print(t, "\t")
	return table.concat(result, "\n")
end

-- UTILS
utils = {}

function utils.distributor(t)
	if type(t) ~= "table" or #t < 1 then
		error("table expected (not empty)", 2)
	end
	-- Member functions
	local func_t = {}
	func_t.__index = func_t
	local index = 1
	function func_t:get()
		if index > #self then index = 1 end
		local val = self[index]
		index = index + 1
		return val
	end
	-- Return object
	return setmetatable(t, func_t)
end

function utils.frames(starts, ends, frame_time)
	if type(starts) ~= "number" or type(ends) ~= "number" or type(frame_time) ~= "number" or frame_time <= 0 then
		error("number, number and valid number expected", 2)
	end
	-- Intermediate data
	local cur_start_time, i, n = starts, 0, math.ceil((ends - starts) / frame_time)
	-- Iterator function
	return function()
		if cur_start_time >= ends then
			return nil
		end
		local return_start_time = cur_start_time
		local return_end_time = return_start_time + frame_time <= ends and return_start_time + frame_time or ends
		cur_start_time = return_end_time
		i = i + 1
		return return_start_time, return_end_time, i, n
	end
end

function utils.interpolate(pct, val1, val2, calc)
	if type(pct) ~= "number" or
	not (
		(type(val1) == "number" and type(val2) == "number") or
		(type(val1) == "string" and type(val2) == "string")
	) or
	(type(calc) ~= "string" and type(calc) ~= "number" and type(calc) ~= "nil") then
		error("number and 2 numbers or 2 strings and optional string expected", 2)
	end
	-- Calculate percent value depending on calculation mode
	if calc ~= nil then
		if type(calc) == "number" then
			pct = pct^calc
		elseif calc == "curve_up" then
			pct = (math.sin( -math.pi/2 + pct * math.pi*2) + 1) / 2
		elseif calc == "curve_down" then
			pct = 1 - (math.sin( -math.pi/2 + pct * math.pi*2) + 1) / 2
		else
			error("valid last value expected", 2)
		end
	end
	-- Interpolate numbers
	if type(val1) == "number" then
		return val1 + (val2 - val1) * pct
	-- Interpolate strings
	else
		-- RGB
		local r1, g1, b1 = convert.ass_to_rgb(val1)
		local r2, g2, b2 = convert.ass_to_rgb(val2)
		if b1 and b2 then
			return convert.rgb_to_ass((r2 - r1) * pct + r1, (g2 - g1) * pct + g1, (b2 - b1) * pct + b1)
		end
		-- Alpha
		local a1 = convert.ass_to_a(val1)
		local a2 = convert.ass_to_a(val2)
		if a1 and a2 then
			return convert.a_to_ass((a2 - a1) * pct + a1)
		end
		-- Invalid string
		error("invalid strings", 2)
	end
end

function utils.text_extents(text, style)
	-- Flat arguments check
	if type(text) ~= "string" or type(style) ~= "table" then
		error("string and style table expected", 2)
	end
	-- Result data
	local width, height, ascent, descent, internal_lead, external_lead
	-- Safe execution
	local success = pcall(function()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Extents with spacing
		if style.spacing > 0 then
			local spaced_width = 0
			for uchar_i, uchar in text:uchars() do
				width, height, ascent, descent, internal_lead, external_lead =
					ctx:text_extents(uchar, style.fontname, style.fontsize * 64, style.bold, style.italic, style.underline, style.strikeout, style.encoding)
				spaced_width = spaced_width + width + style.spacing * 64
			end
			width = spaced_width
		-- Extents without spacing
		else
			width, height, ascent, descent, internal_lead, external_lead =
				ctx:text_extents(text, style.fontname, style.fontsize * 64, style.bold, style.italic, style.underline, style.strikeout, style.encoding)
		end
		-- Scale correctly
		local scale_x, scale_y = style.scale_x / 100 / 64, style.scale_y / 100 / 64
		width, height, ascent, descent, internal_lead, external_lead =
			width * scale_x, height * scale_y, ascent * scale_y, descent * scale_y, internal_lead * scale_y, external_lead * scale_y
	end)
	if not success then
		error("invalid style table", 2)
	end
	return width, height, ascent, descent, internal_lead, external_lead
end
