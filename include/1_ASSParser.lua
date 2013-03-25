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
]]--

-- ASS parser
function io.load_ass(content)
	-- String trimming
	local function trim(text)
		-- Get prespace, postspace and text between
		local prespace, subtext, postspace = text:match("([ \t]*)([^ \t]*)([ \t]*)")
		-- Return trimmed text and spaces
		return subtext, prespace:len(), postspace:len()
	end
	-- Searching patterns
	local meta_comment = "^;%s*(.*)$"
	local meta_title = "^Title:%s*(.*)$"
	local meta_scripttype = "^ScriptType:%s*(.*)$"
	local meta_wrapstyle = "^WrapStyle:%s*(%d)$"
	local meta_width = "^PlayResX:%s*(%d+)$"
	local meta_height = "^PlayResY:%s*(%d+)$"
	local meta_scalebands = "^ScaledBorderAndShadow:%s*(.*)$"
	local meta_ratio = "^Video Aspect Ratio:%s*(%d+%.?%d*)$"
	local meta_zoom = "^Video Zoom:%s*(%d+)$"
	local meta_position = "^Video Position:%s*(%d+)$"
	local meta_laststyle = "^Last Style Storage:%s*(.*)$"
	local meta_audio = "^Audio File:%s*(.*)$"
	local meta_video = "^Video File:%s*(.*)$"
	local style_line = "^Style:%s*(.-),(.-),(%d+%.?%d*),(.-),(.-),(.-),(.-),(%-?%d+),(%-?%d+),(%-?%d+),(%-?%d+),(%d+%.?%d*),(%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+%.?%d*),(%-?%d+),(%d+%.?%d*),(%-?%d+%.?%d*),(%d),(%d+),(%d+),(%d+),(%d+)$"
	local dialog_line = "^Dialogue:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)$"
	local comment_line = "^Comment:%s*(%d+),(.-),(.-),(.-),(.-),(%d+),(%d+),(%d+),(.-),(.*)$"
	-- Parse ASS line
	local function ParseLine(line)
		-- Meta
		if line:find(meta_comment) then
			if meta.comment then
				meta.comment = string.format("%s\n%s", meta.comment, line:match(meta_comment))
			else
				meta.comment = line:match(meta_comment)
			end
		elseif line:find(meta_title) then
			meta.title = line:match(meta_title)
		elseif line:find(meta_scripttype) then
			meta.script_type = line:match(meta_scripttype)
		elseif line:find(meta_wrapstyle) then
			meta.wrap_style = tonumber(line:match(meta_wrapstyle))
		elseif line:find(meta_width) then
			meta.width = tonumber(line:match(meta_width))
		elseif line:find(meta_height) then
			meta.height = tonumber(line:match(meta_height))
		elseif line:find(meta_scalebands) then
			meta.scaled_border_and_shadow = line:match(meta_scalebands) == "yes"
		elseif line:find(meta_ratio) then
			meta.ratio = tonumber(line:match(meta_ratio))
		elseif line:find(meta_zoom) then
			meta.zoom = tonumber(line:match(meta_zoom))
		elseif line:find(meta_position) then
			meta.position = tonumber(line:match(meta_position))
		elseif line:find(meta_laststyle) then
			meta.last_style = line:match(meta_laststyle)
		elseif line:find(meta_audio) then
			meta.audio = line:match(meta_audio)
		elseif line:find(meta_video) then
			meta.video = line:match(meta_video)
		-- Styles
		elseif line:find(style_line) then
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
		-- Lines
		elseif line:find(comment_line) or line:find(dialog_line) then
			-- Extract dialog
			local dialog = table.create(0,11)
			if line:find(comment_line) then
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
	local function UpgradeLines()
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
		-- Through lines
		for li, line in ipairs(lines) do
			-- Sylables
			line.syls = {}
			local syls_n, last_time, word_i = 0, 0, 1
			for pre_tag, tag_time, post_tag, text in line.k_text:gmatch("{(.-)\\[kK][of]?(%d+)(.-)}([^{]*)") do
				local syl = table.create(0,24)
				-- Times
				syl.duration = tag_time * 10
				syl.start_time = last_time
				syl.mid_time = syl.start_time + syl.duration / 2
				syl.end_time = syl.start_time + syl.duration
				last_time = syl.end_time
				-- Text
				syl.text, syl.prespace, syl.postspace = trim(text)
				-- Inline fx
				syl.inline_fx = string.match(pre_tag .. post_tag, "\\%-([^\\]*)") or ""
				-- Indices
				if syl.prespace > 0 then word_i = word_i + 1 end
				syl.word_i = word_i
				if syl.postspace > 0 then word_i = word_i + 1 end
				syls_n = syls_n + 1
				syl.i = syls_n
				line.syls[syls_n] = syl
			end
			-- Characters
			line.chars = {}
			local chars_n = 0
			word_i = 1
			for ci, uchar in line.text:uchars() do
				local char = table.create(0,22)
				-- Times
				char.duration = line.duration
				char.start_time = 0
				char.mid_time = char.duration / 2
				char.end_time = char.duration
				-- Text
				char.text = uchar
				-- Indices
				if uchar == " " or uchar == "\t" then
					char.word_i = 0
					if ci ~= 1 and line.chars[ci-1] ~= " " and line.chars[ci-1] ~= "\t" then
						word_i = word_i + 1
					end
				else
					char.word_i = word_i
				end
				char.syl_i = 1
				chars_n = chars_n + 1
				char.i = chars_n
				line.chars[chars_n] = char
			end
			-- Character-sylable relation
			local ci = 1
			for si, syl in ipairs(line.syls) do
				-- Prespace
				for i = 0, syl.prespace - 1 do
					line.chars[ci+i].duration = syl.duration
					line.chars[ci+i].start_time = syl.start_time
					line.chars[ci+i].mid_time = syl.mid_time
					line.chars[ci+i].end_time = syl.end_time
					line.chars[ci+i].syl_i = si
				end
				ci = ci + syl.prespace
				-- Characters
				for syl_ci, uchar in syl.text:uchars() do
					line.chars[ci+syl_ci-1].duration = syl.duration
					line.chars[ci+syl_ci-1].start_time = syl.start_time
					line.chars[ci+syl_ci-1].mid_time = syl.mid_time
					line.chars[ci+syl_ci-1].end_time = syl.end_time
					line.chars[ci+syl_ci-1].syl_i = si
				end
				ci = ci + syl.text:ulen()
				-- Postspace
				for i = 0, syl.postspace - 1 do
					line.chars[ci+i].duration = syl.duration
					line.chars[ci+i].start_time = syl.start_time
					line.chars[ci+i].mid_time = syl.mid_time
					line.chars[ci+i].end_time = syl.end_time
					line.chars[ci+i].syl_i = si
				end
				ci = ci + syl.postspace
			end
			-- Words
			line.words = {}
			local words_n = 0
			for text in line.text:gmatch("[ \t]*[^ \t]+[ \t]*") do
				local word = table.create(0,20)
				-- Times
				word.duration = line.duration
				word.start_time = 0
				word.mid_time = word.duration / 2
				word.end_time = word.duration
				-- Text
				word.text, word.prespace, word.postspace = trim(text)
				-- Indices
				words_n = words_n + 1
				word.i = words_n
				line.words[words_n] = word
			end
			-- Word-sylable relation
			for wi, word in ipairs(line.words) do
				local found_start = false
				for si, syl in ipairs(line.syls) do
					if syl.word_i == wi then
						if not found_start then
							word.start_time = syl.start_time
							found_start = true
						end
						if si == #line.syls or line.syls[si+1].word_i ~= wi then
							word.end_time = syl.end_time
							word.duration = word.end_time - word.start_time
							word.mid_time = word.start_time + word.duration / 2
							break
						end
					end
				end
			end
		end
	end
	-- Add sizes to all line texts
	local function AddSizes()
		-- Through lines
		for li, line in ipairs(lines) do
			-- Size calculation possible?
			if line.styleref then
				-- Line
				line.width, line.height, line.ascent, line.descent, line.intlead, line.extlead = utils.text_extents(line.text, line.styleref)
				-- Sylables
				for si, syl in ipairs(line.syls) do
					syl.width, syl.height, syl.ascent, syl.descent, syl.intlead, syl.extlead = utils.text_extents(syl.text, line.styleref)
				end
				-- Characters
				for ci, char in ipairs(line.chars) do
					char.width, char.height, char.ascent, char.descent, char.intlead, char.extlead = utils.text_extents(char.text, line.styleref)
				end
				-- Words
				for wi, word in ipairs(line.words) do
					word.width, word.height, word.ascent, word.descent, word.intlead, word.extlead = utils.text_extents(word.text, line.styleref)
				end
			end
		end
	end
	-- Add positions to all line texts
	local function AddPositions()
		-- Through lines
		for li, line in ipairs(lines) do
			-- Position calculation possible?
			if line.styleref and meta.width and meta.height then
				local space_width = utils.text_extents(" ", line.styleref)
				-- Line y
				if line.styleref.alignment > 6 then
					line.top = math.max(line.margin_v, line.styleref.margin_v)
					line.middle = line.top + line.height / 2
					line.bottom = line.top + line.height
					line.y = line.top
				elseif line.styleref.alignment > 3 then
					line.top = meta.height / 2 - line.height / 2
					line.middle = line.top + line.height / 2
					line.bottom = line.top + line.height
					line.y = line.middle
				else
					line.top = meta.height - math.max(line.margin_v, line.styleref.margin_v) - line.height
					line.middle = line.top + line.height / 2
					line.bottom = line.top + line.height
					line.y = line.bottom
				end
				-- Line x
				if (line.styleref.alignment-1) % 3 == 0 then
					line.left = math.max(line.margin_l, line.styleref.margin_l)
					line.center = line.left + line.width / 2
					line.right = line.left + line.width
					line.x = line.left
				elseif (line.styleref.alignment-2) % 3 == 0 then
					line.left = meta.width / 2 - line.width / 2
					line.center = line.left + line.width / 2
					line.right = line.left + line.width
					line.x = line.center
				else
					line.left = meta.width - math.max(line.margin_r, line.styleref.margin_r) - line.width
					line.center = line.left + line.width / 2
					line.right = line.left + line.width
					line.x = line.right
				end
				-- Sylables dimension
				local max_width, sum_height = 0, 0
				for si, syl in ipairs(line.syls) do
					max_width = math.max(max_width, syl.width)
					sum_height = sum_height + syl.height
				end
				-- Sylables y
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					for si, syl in ipairs(line.syls) do
						syl.top = line.top
						syl.middle = line.middle
						syl.bottom = line.bottom
						syl.y = line.y
					end
				else
					local cur_y = meta.height / 2 - sum_height / 2
					for si, syl in ipairs(line.syls) do
						syl.top = cur_y
						syl.middle = syl.top + syl.height / 2
						syl.bottom = syl.top + syl.height
						syl.y = syl.middle
						cur_y = cur_y + syl.height
					end
				end
				-- Sylables x
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					local cur_x = line.left
					for si, syl in ipairs(line.syls) do
						cur_x = cur_x + syl.prespace * space_width
						syl.left = cur_x
						syl.center = syl.left + syl.width / 2
						syl.right = syl.left + syl.width
						if (line.styleref.alignment-1) % 3 == 0 then
							syl.x = syl.left
						elseif (line.styleref.alignment-2) % 3 == 0 then
							syl.x = syl.center
						else
							syl.x = syl.right
						end
						cur_x = cur_x + syl.width + syl.postspace * space_width
					end
				else
					for si, syl in ipairs(line.syls) do
						local x_fix = (max_width - syl.width) / 2
						if line.styleref.alignment == 4 then
							syl.left = line.left + x_fix
							syl.center = syl.left + syl.width / 2
							syl.right = syl.left + syl.width
							syl.x = syl.left
						elseif line.styleref.alignment == 5 then
							syl.left = meta.width / 2 - syl.width / 2
							syl.center = syl.left + syl.width / 2
							syl.right = syl.left + syl.width
							syl.x = syl.center
						else
							syl.left = line.right - syl.width - x_fix
							syl.center = syl.left + syl.width / 2
							syl.right = syl.left + syl.width
							syl.x = syl.right
						end
					end
				end
				-- Characters dimension
				max_width, sum_height = 0, 0
				for ci, char in ipairs(line.chars) do
					max_width = math.max(max_width, char.width)
					sum_height = sum_height + char.height
				end
				-- Characters y
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					for ci, char in ipairs(line.chars) do
						char.top = line.top
						char.middle = line.middle
						char.bottom = line.bottom
						char.y = line.y
					end
				else
					local cur_y = meta.height / 2 - sum_height / 2
					for ci, char in ipairs(line.chars) do
						char.top = cur_y
						char.middle = char.top + char.height / 2
						char.bottom = char.top + char.height
						char.y = char.middle
						cur_y = cur_y + char.height
					end
				end
				-- Characters x
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					local cur_x = line.left
					for ci, char in ipairs(line.chars) do
						char.left = cur_x
						char.center = char.left + char.width / 2
						char.right = char.left + char.width
						if (line.styleref.alignment-1) % 3 == 0 then
							char.x = char.left
						elseif (line.styleref.alignment-2) % 3 == 0 then
							char.x = char.center
						else
							char.x = char.right
						end
						cur_x = cur_x + char.width
					end
				else
					for ci, char in ipairs(line.chars) do
						local x_fix = (max_width - char.width) / 2
						if line.styleref.alignment == 4 then
							char.left = line.left + x_fix
							char.center = char.left + char.width / 2
							char.right = char.left + char.width
							char.x = char.left
						elseif line.styleref.alignment == 5 then
							char.left = meta.width / 2 - char.width / 2
							char.center = char.left + char.width / 2
							char.right = char.left + char.width
							char.x = char.center
						else
							char.left = line.right - char.width - x_fix
							char.center = char.left + char.width / 2
							char.right = char.left + char.width
							char.x = char.right
						end
					end
				end
				-- Words dimension
				max_width, sum_height = 0, 0
				for wi, word in ipairs(line.words) do
					max_width = math.max(max_width, word.width)
					sum_height = sum_height + word.height
				end
				-- Words y
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					for wi, word in ipairs(line.words) do
						word.top = line.top
						word.middle = line.middle
						word.bottom = line.bottom
						word.y = line.y
					end
				else
					local cur_y = meta.height / 2 - sum_height / 2
					for wi, word in ipairs(line.words) do
						word.top = cur_y
						word.middle = word.top + word.height / 2
						word.bottom = word.top + word.height
						word.y = word.middle
						cur_y = cur_y + word.height
					end
				end
				-- Words x
				if line.styleref.alignment > 6 or line.styleref.alignment < 4 then
					local cur_x = line.left
					for wi, word in ipairs(line.words) do
						cur_x = cur_x + word.prespace * space_width
						word.left = cur_x
						word.center = word.left + word.width / 2
						word.right = word.left + word.width
						if (line.styleref.alignment-1) % 3 == 0 then
							word.x = word.left
						elseif (line.styleref.alignment-2) % 3 == 0 then
							word.x = word.center
						else
							word.x = word.right
						end
						cur_x = cur_x + word.width + word.postspace * space_width
					end
				else
					for wi, word in ipairs(line.words) do
						local x_fix = (max_width - word.width) / 2
						if line.styleref.alignment == 4 then
							word.left = line.left + x_fix
							word.center = word.left + word.width / 2
							word.right = word.left + word.width
							word.x = word.left
						elseif line.styleref.alignment == 5 then
							word.left = meta.width / 2 - word.width / 2
							word.center = word.left + word.width / 2
							word.right = word.left + word.width
							word.x = word.center
						else
							word.left = line.right - word.width - x_fix
							word.center = word.left + word.width / 2
							word.right = word.left + word.width
							word.x = word.right
						end
					end
				end
			end
		end
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
		local function sort_func(line1, line2)
			return line1.start_time < line2.start_time
		end
		for si, style in pairs(line_styles) do
			table.sort(style, sort_func)
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
	-- Try to open content as file
	if content:len() < 256 then
		local file = io.open(content, "r")
		if file then
			content = file:read("*a")
			file:close()
		end
	end
	-- Define global ASS tables
	meta, styles, lines = {}, {}, {}
	-- Parse lines
	for line in content:gmatch("[^\n]+") do
		ParseLine(line)
	end
	-- Add additional informations
	UpgradeLines()
	AddSubTexts()
	AddSizes()
	AddPositions()
	AddInOutTimes(1000.1)
end
