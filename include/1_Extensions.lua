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
		error("valid number expected", 2)
	end
	return string.format("&H%02x&", 255-a)
end

function convert.ass_to_a(str)
	if type(str) ~= "string" then
		error("string expected", 2)
	end
	local a = str:match("&H(%x%x)&")
	if not a then
		error("invalid string", 2)
	else
		return 255-tonumber(a, 16)
	end
end

function convert.rgb_to_ass(r,g,b)
	if type(r) ~= "number" or type(g) ~= "number" or type(b) ~= "number" then
		error("number, number and number expected", 2)
	elseif r < 0 or r > 255 or g < 0 or g > 255 or b < 0 or b > 255 then
		error("valid numbers expected", 2)
	end
	return string.format("&H%02x%02x%02x&", b, g, r)
end

function convert.ass_to_rgb(str)
	if type(str) ~= "string" then
		error("string expected", 2)
	end
	local b, g, r = str:match("&H(%x%x)(%x%x)(%x%x)&")
	if not (b and g and r) then
		error("invalid string", 2)
	else
		return tonumber(r, 16), tonumber(g, 16), tonumber(b, 16)
	end
end

--STRING
--[[
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

--TABLE
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
	if type(t) ~= "table" or table.maxn(t) < 1 then
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
	local cur_start_time, i, n = starts, 0, math.ceil((ends - starts) / frame_time)
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
		if type(calc) == "number"  then
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
		return convert.rgb_to_ass((r2 - r1) * pct + r1, (g2 - g1) * pct + g1, (b2 - b1) * pct + b1)
		-- Alpha
		local a1 = convert.ass_to_a(val1)
		local a2 = convert.ass_to_a(val2)
		return convert.a_to_ass((a2 - a1) * pct + a1)
		-- Invalid string
		error("invalid arguments", 2)
	end
end

function utils.text_extents(text, styleref)
	-- Flat arguments check
	if type(text) ~= "string" or type(styleref) ~= "table" then
		error("string and table expected", 2)
	end
	-- Calculation data
	local ctx = tgdi.create_context()
	local status, width, height, ascent, descent, internal_lead, external_lead
	-- Extents with spacing
	if styleref.spacing > 0 then
		local spaced_width = 0
		for uchar in string.uchars(text) do
			status, width, height, ascent, descent, internal_lead, external_lead =
				pcall(ctx:text_extents, uchar, styleref.fontname, styleref.fontsize * 64, styleref.bold, styleref.italic, styleref.underline, styleref.strikeout, styleref.encoding)
			if not status then
				error("wrong arguments", 2)
			end
			spaced_width = spaced_width + width + styleref.spacing * 64
		end
		width = spaced_width
	-- Extents without spacing
	else
		status, width, height, ascent, descent, internal_lead, external_lead =
			pcall(ctx:text_extents, text, styleref.fontname, styleref.fontsize * 64, styleref.bold, styleref.italic, styleref.underline, styleref.strikeout, styleref.encoding)
		if not status then
			error("wrong arguments", 2)
		end
	end
	-- Scale correctly and return
	local scale_x, scale_y = styleref.scale_x / 100 / 64, styleref.scale_y / 100 / 64
	return width * scale_x, height * scale_y, ascent * scale_y, descent * scale_y, internal_lead * scale_y, external_lead * scale_y
end
