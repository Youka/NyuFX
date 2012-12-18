--[[
This time, text shapes and pixels are used for color and fade gradients, wobbling and the risk of huge output files.
For subtitles, text shapes are deformed for a wobbling effect during in- & outfade.
Text is converted to a shape, then splitted into shorter outline segments, so we can use the mass of outline points now for forming it like a modelling clay,
and finally a filter function changes outline points positions for wobbling.
For roumajis and kanjis, text pixels in different colors, with different times, fades and moves are giving a nice show.
The output file becomes huge and specially the combination of used ASS tags slows VSFilter rendering process a lot!
If it takes too much time to see the result, it's a good idea to separate effects by creating multiple output files
 - comment effect lines (code line 105, 107, 109) to disable effects for current pass.
]]
function roumaji(line, l)
	l.style = "p"
	for si, syl in ipairs(line.syls) do
		local syl_fx_dur = syl.duration + 200
		for pi, pixel in ipairs( convert.text_to_pixels(syl.text, line.styleref) ) do
			local x, y = math.round(syl.left) + pixel.x, math.round(syl.top) + pixel.y
			local a = pixel.a == 255 and "" or "\\1a" .. convert.a_to_ass(pixel.a)
			local pct = math.distance( pixel.x + (syl.left - line.left) - line.width / 2, pixel.x - line.height / 2 ) / math.distance( line.width / 2, line.height / 2 )
			local color = utils.interpolate(pct, "&HFFFFFF&", "&H000000&")
			l.start_time = line.start_time - line.infade / 2 + pct * line.infade / 4
			local fx_direction = pixel.y < syl.height / 2 and -1 or 1
			local x2 = x + math.random(-5, 5)
			local y2 = y + math.random(10,20) * fx_direction
			local split_start_time = (line.start_time + syl.start_time - l.start_time) + (pixel.x / syl.width) * syl_fx_dur / 2
			local split_end_time = split_start_time + syl_fx_dur / 2
			l.end_time = l.start_time + split_end_time
			l.text = string.format("{\\fad(%d,%d)%s\\1c%s\\move(%d,%d,%d,%d,%d,%d)\\p1}m 0 0 l 1 0 1 1 0 1"
			, line.infade / 4, syl_fx_dur / 2, a, color, x, y, x2, y2, split_start_time, split_end_time)
			io.write_line(l)
		end
	end
end

function kanji(line, l)
	l.style = "p"
	for ci, char in ipairs(line.chars) do
		local char_fx_dur = char.duration + 200
		for pi, pixel in ipairs( convert.text_to_pixels(char.text, line.styleref) ) do
			local x, y = math.round(char.left) + pixel.x, math.round(char.top) + pixel.y
			local a = pixel.a == 255 and "" or "\\1a" .. convert.a_to_ass(pixel.a)
			local pct = math.abs(pixel.y + (char.top - line.chars[1].top) - #line.chars * line.height / 2) / (#line.chars * line.height / 2)
			local color = utils.interpolate(pct, "&HFFFFFF&", "&H000000&")
			l.start_time = line.start_time - line.infade / 2 + pct * line.infade / 4
			local fx_direction = pixel.x < char.width / 2 and -1 or 1
			local x2 = x + math.random(10,20) * fx_direction
			local y2 = y + math.random(-5, 5)
			local split_start_time = (line.start_time + char.start_time - l.start_time) + (pixel.y / char.height) * char_fx_dur / 2
			local split_end_time = split_start_time + char_fx_dur / 2
			l.end_time = l.start_time + split_end_time
			l.text = string.format("{\\fad(%d,%d)%s\\1c%s\\move(%d,%d,%d,%d,%d,%d)\\p1}m 0 0 l 1 0 1 1 0 1"
			, line.infade / 4, char_fx_dur / 2, a, color, x, y, x2, y2, split_start_time, split_end_time)
			io.write_line(l)
		end
	end
end

function sub(line, l)
	--Getting text shape
	local text_shape = convert.text_to_shape(line.text, line.styleref)
	--Basic text
	l.text = string.format("{\\an7\\pos(%.3f,%.3f)\\p4}%s", line.left, line.top, text_shape)
	io.write_line(l)
	--Flatten text shape
	text_shape = shape.split( text_shape, 16 )
	--Infade
	for s, e, i, n in utils.frames(line.start_time - line.infade / 2, line.start_time, 41.71) do
		l.start_time = s
		l.end_time = e
		local frame_pct = i / n
		local alpha = utils.interpolate(frame_pct, "&HFF&", "&H00&")
		local deform_animator = frame_pct * math.pi * 2
		local deform_range = 24 * (1 - frame_pct)
		local deformed_text_shape = shape.filter(text_shape,
		function(x,y)
			local x_add = math.sin(y / 24 + deform_animator) * deform_range
			local y_add = math.sin(x / 24 + deform_animator) * deform_range
			return x + x_add, y + y_add
		end)
		l.text = string.format("{\\an7\\pos(%.3f,%.3f)\\alpha%s\\p4}%s"
		, line.left, line.top, alpha, deformed_text_shape)
		io.write_line(l)
	end
	--Outfade
	for s, e, i, n in utils.frames(line.end_time, line.end_time + line.outfade / 2, 41.71) do
		l.start_time = s
		l.end_time = e
		local frame_pct = i / n
		local alpha = utils.interpolate(frame_pct, "&H00&", "&HFF&")
		local deform_animator = frame_pct * math.pi * 2
		local deform_range = 24 * frame_pct
		local deformed_text_shape = shape.filter(text_shape,
		function(x,y)
			local x_add = math.sin(y / 24 + deform_animator) * deform_range
			local y_add = math.sin(x / 24 + deform_animator) * deform_range
			return x + x_add, y + y_add
		end)
		l.text = string.format("{\\an7\\pos(%.3f,%.3f)\\alpha%s\\p4}%s"
		, line.left, line.top, alpha, deformed_text_shape)
		io.write_line(l)
	end
end

for li, line in ipairs(lines) do
	if line.styleref.alignment >=7 then
		roumaji( line, table.copy(line) )
	elseif line.styleref.alignment <=3 then
		sub( line, table.copy(line) )
	else
		kanji( line, table.copy(line) )
	end
	io.progressbar(li / #lines)
end
