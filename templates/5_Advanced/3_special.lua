--[[
"What's with border pixels or a border shape?"
This example goes farther with pixels and text shapes.
Beside borders there's a curve-moving roumaji text.
Analyse the code by yourself and learn something about professional techniques.
Hint: for generations with extreme long duration, just generate one line per effect to have a preview, that's enough.
]]
function roumaji(line, l)
	--Text shape
	local text_shape = shape.split( convert.text_to_shape(line.text, line.styleref), 16 )
	--Help function for vertical text bending
	local function bending(x)
		return math.sin(x / meta.width * math.pi) * 240
	end
	--Infade
	for s, e, i, n in utils.frames(line.start_time - line.infade / 2, line.start_time, 41.71) do
		l.start_time = s
		l.end_time = e
		local frame_pct = i / n
		local curved_text_shape = shape.filter( text_shape,
		function(x, y)
			local x_add = (meta.width - frame_pct * line.width) * 8
			local y_add = bending((x+x_add) / 8)
			return x + x_add, y + y_add
		end)
		l.text = "{\\an7\\pos(0,0)\\p4}" .. curved_text_shape
		io.write_line(l)
	end
	--Outfade
	for s, e, i, n in utils.frames(line.end_time, line.end_time + line.outfade / 2, 41.71) do
		l.start_time = s
		l.end_time = e
		local frame_pct = i / n
		local curved_text_shape = shape.filter( text_shape,
		function(x, y)
			local x_add = (frame_pct * -line.width) * 8
			local y_add = bending((x+x_add) / 8)
			return x + x_add, y + y_add
		end)
		l.text = "{\\bord0\\an7\\pos(0,0)\\p4}" .. curved_text_shape
		io.write_line(l)
	end
	--Basic
	local l2 = table.copy(line) --Extra line for pixels
	l2.style = "p"
	l2.layer = 0
	l.layer = 1
	for si, syl in ipairs(line.syls) do
		local syl_shape = shape.split( convert.text_to_shape(syl.text, line.styleref), 16 )
		for s, e, i, n in utils.frames(line.start_time, line.end_time, 41.71) do
			l.start_time = s
			l.end_time = e
			local frame_pct = i / n
			local curved_syl_shape = shape.filter( syl_shape,
			function(x, y)
				local x_add = (meta.width - line.width - frame_pct * (meta.width - line.width) + (syl.left - line.left)) * 8
				local y_add = bending((x+x_add) / 8)
				return x + x_add, y + y_add
			end)
			l.text = "{\\an7\\pos(0,0)\\p4}" .. curved_syl_shape
			--Effect
			if s >= line.start_time + syl.start_time then
				l.text = l.text:gsub("{", "{\\bord0")
				--Sylable glows
				if e <= line.start_time + syl.end_time then
					l.text = l.text:gsub( "{", "{\\1c" .. utils.interpolate((e - line.start_time - syl.start_time) / syl.duration, line.styleref.color1, "&H0000A0&", "curve_up") )
				end
				--Outline pixels spread
				if not syl.effect_over then
					--Extract border pixels
					local border_pixels = {}
					do
						local text_pixels = convert.shape_to_pixels(curved_syl_shape, true)
						local outline_pixels = convert.shape_to_pixels( shape.tooutline( curved_syl_shape, line.styleref.outline * 8), true)
						for _, o_pixel in ipairs(outline_pixels) do
							local inside = false
							for _, t_pixel in ipairs(text_pixels) do
								if o_pixel.x == t_pixel.x and o_pixel.y == t_pixel.y and t_pixel.a == 255 then
									inside = true
									break
								end
							end
							if not inside then
								table.insert(border_pixels, o_pixel)
							end
						end
					end
					--Draw border pixels
					l2.start_time = s
					l2.end_time = line.start_time + syl.end_time + 400
					for pi, pixel in ipairs(border_pixels) do
						local a = pixel.a == 255 and "" or "\\1a" .. convert.a_to_ass(pixel.a)
						local move_x, move_y = math.randomway() * math.random(5, 15), math.randomway() * math.random(5, 15)
						l2.text = string.format("{\\move(%d,%d,%d,%d)\\fad(0,400)%s\\1c%s\\p1}m 0 0 l 1 0 1 1 0 1", pixel.x, pixel.y, pixel.x + move_x, pixel.y + move_y, a, line.styleref.color3)
						io.write_line(l2)
					end
					--Limit pixel animation to beginning of sylable effect
					syl.effect_over = true
				end
			end
			--Draw normal text
			io.write_line(l)
		end
	end
end

function sub(line, l)
	--Preparations
	local text_mask = shape.move(convert.text_to_shape(line.text, line.styleref), line.left * 8, line.top * 8)
	local outline_mask = shape.tooutline(shape.split(text_mask), line.styleref.outline * 8)
	local fill_animated
	do
		local blurred_edge = 8
		local fill_range = math.distance(line.width / 2 + line.styleref.outline, line.height / 2 + line.styleref.outline) + blurred_edge
		local full_fill = string.format("{\\pos(%.3f,%.3f)\\an5\\bord0\\blur%d\\p1}%s", line.center, line.middle, blurred_edge, shape.ellipse(fill_range * 2, fill_range * 2))
		fill_animated = string.format("{\\fscx0\\fscy0\\t(0,%d,\\fscx100\\fscy100)\\t(%d,%d,\\fscx0\\fscy0)}", line.infade / 2, line.infade / 2 + line.duration, line.infade / 2 + line.duration + line.outfade / 2) .. full_fill
	end
	l.start_time = line.start_time - line.infade / 2
	l.end_time = line.end_time + line.outfade / 2
	--Outline fill
	l.text = string.format("{\\clip(4,%s)\\1c%s}", outline_mask, line.styleref.color3) .. fill_animated
	io.write_line(l)
	--Text fill
	l.text = string.format("{\\clip(4,%s)}", text_mask) .. fill_animated
	io.write_line(l)
end

for li, line in ipairs(lines) do
	if line.infade == 1000.1 then
		if line.styleref.alignment >=7 then
			roumaji( line, table.copy(line) )
		elseif line.styleref.alignment <=3 then
			sub( line, table.copy(line) )
		end
		io.progressbar(li / #lines)
	end
end
