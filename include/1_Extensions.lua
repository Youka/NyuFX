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

function convert.text_to_pixels(text, style)
	-- Flat arguments check
	if type(text) ~= "string" or type(style) ~= "table" then
		error("string and style table expected", 2)
	end
	-- Calculation data
	local pixel_palette
	-- Safe execution
	local function text_to_pixels()
		-- Graphic context
		local ctx = tgdi.create_context()

		-- TODO: use paths to support spacing & scaling

		--[[-- Text with spacing
		if style.spacing > 0 then

		-- Text without spacing
		else

		end]]
	end
	local success = pcall(text_to_pixels)
	if not success then
		error("invalid style table", 2)
	end
	-- Convert pixel palette to comfortable pixel table and return
	local pixels, pixels_n = {}, 0
	for pi, alpha in ipairs(pixel_palette) do
		if alpha > 0 then
			pixels_n = pixels_n + 1
			pixels[pixels_n] = {a = alpha, x = (pi-1) % pixel_palette.width, y = math.floor((pi-1) / pixel_palette.width)}
		end
	end
	return pixels
end

-- TODO: convert.text_to_shape

-- TODO: convert.shape_to_pixels

-- TODO: convert.image_to_pixels

-- IO
function io.load_ass(filename)
	if type(filename) ~= "string" then
		error("string expected", 2)
	end
	local file, err = io.open(filename, "r")
	if file then
		LoadASS(file:read("*a"), utils.text_extents)
		file:close()
	else
		error(err, 2)
	end
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

function shape.bounding(shape, as_region)
	if type(shape) ~= "string" or (type(as_region) ~= "boolean" and as_region ~= nil) then
		error("string and optional boolean expected", 2)
	end
	-- Calculation data
	local min_x, min_y, max_x, max_y
	-- Look for minimal and maximal shape point
	if not as_region then
		-- Iterate through points
		local function point_finder(x, y)
			min_x = min_x and math.min(min_x, x) or x
			min_y = min_y and math.min(min_y, y) or y
			max_x = max_x and math.max(max_x, x) or x
			max_y = max_y and math.max(max_y, y) or y
		end
		shape:gsub("(%-?%d+)%s+(%-?%d+)", point_finder)
		if not min_x then
			error("invalid shape", 2)
		end
	-- Convert to region and calculate bounding rectangle
	else
		-- Safe execution
		local function bounding()
			-- Graphic context
			local ctx = tgdi.create_context()
			-- Get bounding
			ctx:add_path(shape)
			min_x, min_y, max_x, max_y = ctx:path_box()
		end
		local success = pcall(bounding)
		if not success then
			error("invalid shape", 2)
		end
	end
	-- Return resulting data
	return min_x, min_y, max_x, max_y
end

function shape.ellipse(w, h)
	if type(w) ~= "number" or type(h) ~= "number" then
		error("number and number expected", 2)
	end
	return string.format("m 0 %d b 0 %d 0 0 %d 0 %d 0 %d 0 %d %d %d %d %d %d %d %d %d %d 0 %d 0 %d", h/2, h/2, w/2, w/2, w, w, h/2, w, h/2, w, h, w/2, h, w/2, h, h, h/2)
end

function shape.filter(shape, filter)
	if type(shape) ~= "string" or type(filter) ~= "function" then
		error("string and function expected", 2)
	end
	local function parser(x, y)
		local new_x, new_y = filter(tonumber(x), tonumber(y))
		if type(new_x) == "number" and type(new_y) == "number" then
			return string.format("%d %d", new_x, new_y)
		end
	end
	local new_shape = shape:gsub("(%-?%d+)%s+(%-?%d+)", parser)
	return new_shape
end

-- TODO: shape.glance

-- TODO: shape.heart

-- TODO: shape.move

-- TODO: shape.rectangle

-- TODO: shape.ring

-- TODO: shape.split

-- TODO: shape.star

-- TODO: shape.tooutline

-- TODO: shape.triangle

-- TODO: shape.transverter

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
	local function itor()
		if s_pos > s:len() then
			return nil
		end
		local cur_pos = s_pos
		s_pos = s_pos + string.ucharrange(s, s_pos)
		char_i = char_i + 1
		return char_i, s:sub(cur_pos, s_pos-1)
	end
	return itor
end

function string.ulen(s)
	if type(s) ~= "string" then
		error("string expected", 2)
	end
	local n = 0
	for _ in string.uchars(s) do
		n = n + 1
	end
	return n
end

-- TABLE
function table.append(t1, t2)
	if type(t1) ~= "table" or type(t2) ~= "table" then
		error("table and table expected", 2)
	end
	for i, v in pairs(t2) do
		if tonumber(i) then
			table.insert(t1, v)
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
	local temp_text = ""
	local function table_print(t, space)
		for i, v in pairs(t) do
			if type(i) == "string" then
				i = string.format("%q", i)
			end
			if type(v) == "string" then
				v = string.format("%q", v)
			end
			temp_text = string.format("%s%s[%s] = %s\n", temp_text, space, tostring(i), tostring(v))
			if type(v) == "table" then
				table_print(v, space .. "\t")
			end
		end
	end
	table_print(t, "\t")
	return temp_text
end

-- UTILS
utils = {}

function utils.distributor(t)
	if type(t) ~= "table" or #t < 1 then
		error("table expected (not empty)", 2)
	end
	--Member functions
	local func_t = {}
	func_t.__index = func_t
	local index = 1
	function func_t:get()
		if index > #self then index = 1 end
		local val = self[index]
		index = index + 1
		return val
	end
	--Return object
	return setmetatable(t, func_t)
end

function utils.frames(starts, ends, frame_time)
	if type(starts) ~= "number" or type(ends) ~= "number" or type(frame_time) ~= "number" or frame_time <= 0 then
		error("number, number and valid number expected", 2)
	end
	-- Intermediate data
	local cur_start_time, i, n = starts, 0, math.ceil((ends - starts) / frame_time)
	-- Iterator function
	local function next_frame()
		if cur_start_time >= ends then
			return nil
		end
		local return_start_time = cur_start_time
		local return_end_time = return_start_time + frame_time <= ends and return_start_time + frame_time or ends
		cur_start_time = return_end_time
		i = i + 1
		return return_start_time, return_end_time, i, n
	end
	return next_frame
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
	-- Calculation data
	local width, height, ascent, descent, internal_lead, external_lead
	-- Safe execution
	local function text_extents()
		-- Graphic context
		local ctx = tgdi.create_context()
		-- Extents with spacing
		if style.spacing > 0 then
			local spaced_width = 0
			for uchar in string.uchars(text) do
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
	end
	local success = pcall(text_extents)
	if not success then
		error("invalid style table", 2)
	end
	return width, height, ascent, descent, internal_lead, external_lead
end
