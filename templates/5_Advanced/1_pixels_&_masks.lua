--[[
Let's generate some megabytes.
Pixels and masks allow much more impressive effects, but increase output file size immensely.
For subtitles there're 2 layers: the text in black and the mask.
The mask is a moving, blurred, white point inside a clip region equal to text.
For roumajis and kanjis, pixels are in use.
Pixels are 1x1 rectangle shapes. For an effect of anti-aliasing (against pixellated edges), some pixels have transparency for a fake of flatness.

The "p" style stores attributes for pixels or simple shapes which have to be without border, without shadow, with alignment left-top, ...
If you want to use pixels, a special style like "p" avoids the need of many tags inside dialog lines and decreases output file size.
]]
function roumaji(line, l)
	l.style = "p"	--Pixels don't need a border and should have alignment 7 for right position in video raster
	for si, syl in ipairs(line.syls) do
		l.end_time = line.start_time + syl.end_time + 400
		for pi, pixel in ipairs( convert.text_to_pixels(syl.text, line.styleref) ) do
			local x, y = pixel.x + math.round(syl.left), pixel.y + math.round(syl.top)
			local alpha = (pixel.a == 255) and "" or ("\\1a" .. convert.a_to_ass(pixel.a))
			l.start_time = line.start_time - line.infade / 2 * math.random(100) / 100
			local start_offset = (line.start_time + syl.start_time) - l.start_time
			l.text = string.format("{\\move(%d,%d,%d,%d,%d,%d)\\fad(0,400)%s\\p1}m 0 0 l 1 0 1 1 0 1"
								, x, y, x + math.random(-10,10), y + math.random(-10,10), start_offset, start_offset + syl.duration + 400
								, alpha)
			io.write_line(l)
		end
	end
end

function kanji(line, l)
	l.style = "p"	--Pixels don't need a border and should have alignment 7 for right position in video raster
	for si, char in ipairs(line.chars) do
		l.end_time = line.start_time + char.end_time + 400
		for pi, pixel in ipairs( convert.text_to_pixels(char.text, line.styleref) ) do
			local x, y = pixel.x + math.round(char.left), pixel.y + math.round(char.top)
			local alpha = (pixel.a == 255) and "" or ("\\1a" .. convert.a_to_ass(pixel.a))
			l.start_time = line.start_time - line.infade / 2 * math.random(0,100) / 100
			local start_offset = (line.start_time + char.start_time) - l.start_time
			l.text = string.format("{\\move(%d,%d,%d,%d,%d,%d)\\fad(0,400)%s\\p1}m 0 0 l 1 0 1 1 0 1"
								, x, y, x + math.random(-10,10), y + math.random(-10,10), start_offset, start_offset + char.duration + 400
								, alpha)
			io.write_line(l)
		end
	end
end

function sub(line, l)
	--Text
	l.start_time = line.start_time - line.infade/2
	l.end_time = line.end_time + line.outfade/2
	l.text = string.format("{\\fad(%d,%d)\\bord0\\1c&H101010&}%s", line.infade/2, line.outfade/2, line.text)
	io.write_line(l)
	--Mask
	local mask = shape.move( convert.text_to_shape(line.text, line.styleref), line.left*8, line.top*8) --Text shape/mask has 8-fold size!
	local shine = shape.ellipse(20, 20)
	l.style = "p"	--No border, alignment left-top - all we need for shapes and no additional tags in every line
	l.start_time = line.start_time
	l.end_time = line.end_time
	l.text = string.format("{\\move(%.3f,%.3f,%.3f,%.3f)\\blur4\\clip(4,%s)\\p1}%s"
						, line.left - 20, line.top + (line.height - 20) / 2
						, line.right, line.top + (line.height - 20) / 2
						, mask, shine)
	io.write_line(l)
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
