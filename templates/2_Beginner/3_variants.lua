--[[
Inline effects is a method to define exclusive effects for sylables.
Roumajis and kanjis are looking for inline effects "hl1" and "hl2" to choose an effect for prepending to basic text.
For subtitles, there's a star jumping over words by frame-per-frame positioning.
Looks like much code for such a simple effect, but it's needed and an easy method with much potential for extensions.
]]
function sub(line, l)
	l.start_time = line.start_time - line.infade/2
	l.end_time = line.end_time + line.outfade/2
	l.text = string.format("{\\fad(%d,%d)\\pos(%.3f,%.3f)}%s"
						, line.infade/2, line.outfade/2, line.x, line.y, line.text)
	io.write_line(l)
	local my_star = shape.star(5, 4, 10)
	for wi, word in ipairs(line.words) do
		--Jump-in to first word
		if wi == 1 then
			for s, e, i, n in utils.frames(line.start_time - line.infade / 2, line.start_time, 41.71) do
				l.start_time = s
				l.end_time = e
				local frame_pct = i / n
				local x = word.center - word.width / 2 * (1-frame_pct)
				local y = word.top - math.sin(frame_pct * math.pi) * (word.width / 16)
				local alpha = utils.interpolate(frame_pct, "&HFF&", "&H00&")
				l.text = string.format("{\\alpha%s\\pos(%.3f,%.3f)\\bord0\\blur1\\p1}%s.", alpha, x, y, my_star)
				io.write_line(l)
			end
		end
		--Jump to next word/end
		local jump_width = (wi ~= #line.words) and (line.words[wi+1].center - word.center) or (word.width / 2)
		local start_pct = (wi - 1) / #line.words
		local end_pct = wi / #line.words
		for s, e, i, n in utils.frames( line.start_time + start_pct * line.duration, line.start_time + end_pct * line.duration, 41.71 ) do
			l.start_time = s
			l.end_time = e
			local frame_pct = i / n
			local x = word.center + frame_pct * jump_width
			local y = word.top - math.sin(frame_pct * math.pi) * (jump_width / 8)
			l.text = string.format("{\\pos(%.3f,%.3f)\\bord0\\blur1\\p1}%s.", x, y, my_star)
			--Jump to end should fading out
			if wi == #line.words then
				l.text = "{\\alpha" .. utils.interpolate(frame_pct, "&H00&", "&HFF&") .. "}" .. l.text
			end
			io.write_line(l)
		end
	end
end

function roumaji(line, l)
	for si, syl in ipairs(line.syls) do
		if syl.text ~= "" then
			l.start_time = line.start_time - line.infade / 2
			l.text = string.format("{\\an5\\pos(%.3f,%.3f)}%s", syl.center, syl.middle, syl.text)
			local start_offset = line.infade / 2 + syl.start_time
			if syl.inline_fx == "hl1" then
				l.end_time = line.start_time + syl.end_time
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,0.9,\\frz720\\fscx0\\fscy0)}"
									, line.infade / 2, syl.duration
									, start_offset, start_offset + syl.duration)
									.. l.text
			elseif syl.inline_fx == "hl2" then
				l.end_time = line.start_time + syl.end_time
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,\\frx%d\\fry%d\\frz%d)}"
									, line.infade / 2, syl.duration
									, start_offset, start_offset + syl.duration
									, math.randomway() * 90, math.randomway() * 90, math.randomway() * 90)
									.. l.text
			else
				l.end_time = line.end_time + line.outfade / 2
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,\\1c%s\\3c%s)\\t(%d,%d,\\1c%s\\3c%s)}"
									, line.infade / 2, line.outfade / 2
									, start_offset, start_offset + syl.duration / 2, line.styleref.color3, line.styleref.color1
									, start_offset + syl.duration / 2, start_offset + syl.duration, line.styleref.color1, line.styleref.color3)
									.. l.text
			end
			io.write_line(l)
		end
	end
end

function kanji(line, l)
	for si, char in ipairs(line.chars) do
		if char.text ~= " " then
			l.start_time = line.start_time - line.infade / 2
			l.text = string.format("{\\an5\\pos(%.3f,%.3f)}%s", char.center, char.middle, char.text)
			local start_offset = line.infade / 2 + char.start_time
			if line.syls[char.syl_i].inline_fx == "hl1" then
				l.end_time = line.start_time + char.end_time
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,0.9,\\frz720\\fscx0\\fscy0)}"
									, line.infade / 2, char.duration
									, start_offset, start_offset + char.duration)
									.. l.text
			elseif line.syls[char.syl_i].inline_fx == "hl2" then
				l.end_time = line.start_time + char.end_time
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,\\frx%d\\fry%d\\frz%d)}"
									, line.infade / 2, char.duration
									, start_offset, start_offset + char.duration
									, math.randomway() * 90, math.randomway() * 90, math.randomway() * 90)
									.. l.text
			else
				l.end_time = line.end_time + line.outfade / 2
				l.text = string.format("{\\fad(%d,%d)\\t(%d,%d,\\1c%s\\3c%s)\\t(%d,%d,\\1c%s\\3c%s)}"
									, line.infade / 2, line.outfade / 2
									, start_offset, start_offset + char.duration / 2, line.styleref.color3, line.styleref.color1
									, start_offset + char.duration / 2, start_offset + char.duration, line.styleref.color1, line.styleref.color3)
									.. l.text
			end
			io.write_line(l)
		end
	end
end

for li, line in ipairs(lines) do
	if line.styleref.alignment >=7 then
		roumaji(line, table.copy(line))
	elseif line.styleref.alignment <=3 then
		sub(line, table.copy(line))
	else
		kanji(line, table.copy(line))
	end
	io.progressbar(li / #lines)
end
