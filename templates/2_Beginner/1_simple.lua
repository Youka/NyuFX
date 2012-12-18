--[[
Time to use more ASS values.
Subtitles are using precalculated infade and outfade times for fitting line-to-line changes.
Additional to the last basic example, kanjis get their sylable grow/shrink effect now.
Don't worry about their position:
Lines with vertical alignment in middle of screen have their sylables, characters and words positions in vertical order!
]]
function sub(line, l)
	l.start_time = line.start_time - line.infade/2
	l.end_time = line.end_time + line.outfade/2
	l.text = string.format("{\\fad(%d,%d)\\blur2}%s", line.infade/2, line.outfade/2, line.text)
	io.write_line(l)
end

function roumaji(line, l)
	for si, syl in ipairs(line.syls) do
		if syl.text ~= "" then
			syl.mid_time = syl.start_time + syl.duration / 2
			l.text = string.format("{\\an5\\pos(%.3f,%.3f)\\t(%d,%d,\\fscy150)\\t(%d,%d,\\fscy100)}%s"
									, syl.center, syl.middle, syl.start_time, syl.mid_time, syl.mid_time, syl.end_time, syl.text)
			io.write_line(l)
		end
	end
end

function kanji(line, l)
	for ci, char in ipairs(line.chars) do
		if char.text ~= " " then
			char.mid_time = char.start_time + char.duration / 2
			l.text = string.format("{\\an5\\pos(%.3f,%.3f)\\t(%d,%d,\\fscx150)\\t(%d,%d,\\fscx100)}%s"
									, char.center, char.middle, char.start_time, char.mid_time, char.mid_time, char.end_time, char.text)
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
