--[[
meta
	.comment
	.title
	.script_type
	.wrap_style
	.width
	.height
	.scaled_border_and_shadow
	.ratio
	.zoom
	.position
	.last_style
	.audio
	.video
styles
	[i]
		.name
		.fontname
		.fontsize
		.color1
		.alpha1
		.color2
		.alpha2
		.color3
		.alpha3
		.color4
		.alpha4
		.bold
		.italic
		.underline
		.strikeout
		.scale_x
		.scale_y
		.spacing
		.angle
		.borderstyle
		.outline
		.shadow
		.alignment
		.margin_l
		.margin_r
		.margin_v
		.encoding
lines
	[i]
		.comment
		.layer
		.start_time
		.mid_time
		.end_time
		.duration
		.infade
		.outfade
		.style
		.styleref
		.actor
		.margin_l
		.margin_r
		.margin_v
		.effect
		.k_text
		.text
		.i
		.width
		.height
		.ascent
		.descent
		.intlead
		.extlead
		.x
		.y
		.left
		.center
		.right
		.top
		.middle
		.bottom
		.syls
			[i]
				.i
				.word_i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.inline_fx
				.prespace
				.postspace
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
				.left
				.center
				.right
				.top
				.middle
				.bottom
		.chars
			[i]
				.i
				.syl_i
				.word_i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
				.left
				.center
				.right
				.top
				.middle
				.bottom
		.words
			[i]
				.i
				.start_time
				.mid_time
				.end_time
				.duration
				.text
				.width
				.height
				.ascent
				.descent
				.intlead
				.extlead
				.x
				.y
				.left
				.center
				.right
				.top
				.middle
				.bottom
]]--

-- Global ASS tables
meta = {}
styles = {}
lines = {}

-- ASS parser
function LoadASS(content, text_extents_func)
	-- Searching patterns
	local meta_comment = ";%s*(.*)"
	local meta_title = "Title:%s*(.*)"
	local meta_scripttype = "ScriptType:%s*(.*)"
	local meta_wrapstyle = "WrapStyle:%s*(%d)"
	local meta_width = "PlayResX:%s*(%d+)"
	local meta_height = "PlayResY:%s*(%d+)"
	local meta_scalebands = "ScaledBorderAndShadow:%s*(.*)"
	local meta_ratio = "Video Aspect Ratio:%s*(%d+%.?%d*)"
	local meta_zoom = "Video Zoom:%s*(%d+)"
	local meta_position = "Video Position:%s*(%d+)"
	local meta_laststyle = "Last Style Storage:%s*(.*)"
	local meta_audio = "Audio File:%s*(.*)"
	local meta_video = "Video File:%s*(.*)"
	local style_line = "Style:%s*(.-),(.-),(%d+%.?%d*),(.-),(.-),(.-),(.-),(%-?%d+),(%-?%d+),(%-?%d+),(%-?%d+),(%d+%.?%d*),(%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+),(%d+%.?%d*),(%-?%d+%.?%d*),(%d),(%d+),(%d+),(%d+),(%d+)"
	local dialog_line = "Dialogue:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)"
	local comment_line = "Comment:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)"
	-- Parse ASS line
	local function ParseLine(line)
		-- Meta
		if line:find(meta_comment) then
			if meta.comment then
				meta.comment = string.format("%s\n%s", meta.comment, line:match(meta_comment))
			else
				meta.comment = line:match(meta_comment)
			end
		end
		if line:find(meta_title) then
			meta.title = line:match(meta_title)
		end
		if line:find(meta_scripttype) then
			meta.script_type = line:match(meta_scripttype)
		end
		if line:find(meta_wrapstyle) then
			meta.wrap_style = tonumber(line:match(meta_wrapstyle))
		end
		if line:find(meta_width) then
			meta.width = tonumber(line:match(meta_width))
		end
		if line:find(meta_height) then
			meta.height = tonumber(line:match(meta_height))
		end
		if line:find(meta_scalebands) then
			meta.scaled_border_and_shadow = line:match(meta_scalebands) == "yes"
		end
		if line:find(meta_ratio) then
			meta.ratio = tonumber(line:match(meta_ratio))
		end
		if line:find(meta_zoom) then
			meta.zoom = tonumber(line:match(meta_zoom))
		end
		if line:find(meta_position) then
			meta.position = tonumber(line:match(meta_position))
		end
		if line:find(meta_laststyle) then
			meta.last_style = line:match(meta_laststyle)
		end
		if line:find(meta_audio) then
			meta.audio = line:match(meta_audio)
		end
		if line:find(meta_video) then
			meta.video = line:match(meta_video)
		end
		-- Styles
		if line:find(style_line) then
			-- Extract style
			local style = table.create(0,23)
			style.name, style.fontname, style.fontsize,
			style.color1, style.color2, style.color3, style.color4,
			style.bold, style.italic, style.underline, style.strikeout,
			style.scale_x, style.scale_y, style.spacing, style.angle,
			style.borderstyle, style.outline, style.shadow,
			style.alignment, style.margin_l, style.margin_r, style.margin_v,
			style.encoding =
			line:match(style_line)
			-- Reformat style
			style.fontsize = tonumber(style.fontsize)
			local a, b, g, r = style.color1:match("&H(%x%x)(%x%x)(%x%x)(%x%x)")
			if r then
				style.color1 = string.format("&H%s%s%s&", b, g, r)
				style.alpha1 = string.format("&H%s&", a)
			else
				style.color1 = nil
			end
			a, b, g, r = style.color2:match("&H(%x%x)(%x%x)(%x%x)(%x%x)")
			if r then
				style.color2 = string.format("&H%s%s%s&", b, g, r)
				style.alpha2 = string.format("&H%s&", a)
			else
				style.color2 = nil
			end
			a, b, g, r = style.color3:match("&H(%x%x)(%x%x)(%x%x)(%x%x)")
			if r then
				style.color3 = string.format("&H%s%s%s&", b, g, r)
				style.alpha3 = string.format("&H%s&", a)
			else
				style.color3 = nil
			end
			a, b, g, r = style.color4:match("&H(%x%x)(%x%x)(%x%x)(%x%x)")
			if r then
				style.color4 = string.format("&H%s%s%s&", b, g, r)
				style.alpha4 = string.format("&H%s&", a)
			else
				style.color4 = nil
			end
			style.bold = style.bold == "-1"
			style.italic = style.italic == "-1"
			style.underline = style.underline == "-1"
			style.strikeout = style.strikeout == "-1"
			style.scale_x = tonumber(style.scale_x)
			style.scale_y = tonumber(style.scale_y)
			style.spacing = tonumber(style.spacing)
			style.angle = tonumber(style.angle)
			style.borderstyle = style.borderstyle == "3"
			style.outline = tonumber(style.outline)
			style.shadow = tonumber(style.shadow)
			style.alignment = tonumber(style.alignment)
			style.margin_l = tonumber(style.margin_l)
			style.margin_r = tonumber(style.margin_r)
			style.margin_v = tonumber(style.margin_v)
			style.encoding = tonumber(style.encoding)
			styles[#styles+1] = style
		end
		-- Lines
		local c_line = line:find(comment_line)
		local d_line = line:find(dialog_line)
		if c_line or d_line then
			-- Extract dialog
			local dialog = table.create(0,11)
			if c_line then
				dialog.comment = true
				dialog.layer, dialog.start_time, dialog.end_time, dialog.style, dialog.actor, dialog.margin_r, dialog.margin_l, dialog.margin_v, dialog.effect, dialog.k_text =
				line:match(comment_line)

			else
				dialog.comment = false
				dialog.layer, dialog.start_time, dialog.end_time, dialog.style, dialog.actor, dialog.margin_r, dialog.margin_l, dialog.margin_v, dialog.effect, dialog.k_text =
				line:match(dialog_line)
			end
			-- Reformat dialog
			dialog.layer = tonumber(dialog.layer)
			local h, m, s, ms = dialog.start_time:match("(%d+):(%d+):(%d+).(%d+)")
			if ms then
				dialog.start_time = ms * 10 + s * 1000 + m * 60000 + h * 3600000
			else
				dialog.start_time = nil
			end
			h, m, s, ms = dialog.end_time:match("(%d+):(%d+):(%d+).(%d+)")
			if ms then
				dialog.end_time = ms * 10 + s * 1000 + m * 1000 * 60 + h * 3600000
			else
				dialog.end_time = nil
			end
			dialog.margin_r = tonumber(dialog.margin_r)
			dialog.margin_l = tonumber(dialog.margin_l)
			dialog.margin_v = tonumber(dialog.margin_v)
			lines[#lines+1] = dialog
		end
	end
	-- Add more informations to line
	local function UpgradeLine()
		-- Through lines
		for li, line in ipairs(lines) do
			-- Add times
			if line.start_time and line.end_time then
				line.duration = line.end_time - line.start_time
				line.mid_time = line.start_time + line.duration / 2
			end
			-- Add style reference
			for si, style in ipairs(styles) do
				if line.style == style.name then
					line.styleref = style
				end
			end
			-- Add tagless text
			line.text = line.k_text:gsub("{.-}", "")
			-- Add index
			line.i = li
		end
	end
	-- Add line subtexts
	local function AddSubTexts()

		-- TODO

	end
	-- Add sizes to all line texts
	local function AddSizes()

		-- TODO

	end
	-- Add positions to all line texts
	local function AddPositions()

		-- TODO

	end
	-- Add line in- & outfade time
	local function AddInOutTimes(first_last_dur)
		-- Collect style names and fitting lines
		local line_styles = {}
		for li, line in ipairs(lines) do
			if not line_styles[line.style] then
				line_styles[line.style] = {{start_time = line.start_time, end_time = line.end_time, i = line.i}}
			else
				line_styles[line.style][#line_styles[line.style]+1] = {start_time = line.start_time, end_time = line.end_time, i = line.i}
			end
		end
		-- Sort lines by time
		for si, style in pairs(line_styles) do
			-- Collect and order start times
			local start_times, start_times_n = table.create(#style,0), 0
			for li, line in ipairs(style) do
				start_times_n = start_times_n + 1
				start_times[start_times_n] = line.start_time
			end
			table.sort(start_times)
			-- Order lines by start times
			local old_style = table.copy(style)
			for ti, start_time in ipairs(start_times) do
				for li = 1, #old_style do
					local line = old_style[li]
					if line and start_time == line.start_time then
						style[ti] = table.copy(line)
						old_style[li] = nil
						break
					end
				end
			end
		end
		-- Insert calculated in- & outfade times
		for si, style in pairs(line_styles) do
			for li, line in ipairs(style) do
				lines[line.i].infade = (li == 1) and first_last_dur or line.start_time - style[li-1].end_time
				lines[line.i].outfade = (li == #style) and first_last_dur or style[li+1].start_time - line.end_time
			end
		end
	end
	-- Check function argument
	if type(content) ~= "string" then
		error("string expected", 2)
	end
	-- Parse lines
	for line in content:gmatch("[^\n]+") do
		ParseLine(line)
	end
	-- Add additional informations
	UpgradeLine()
	AddSubTexts()
	AddSizes()
	AddPositions()
	AddInOutTimes(1000.1)
end
