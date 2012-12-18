--[[
The transform ASS tag makes some nice animations possible, but that shouldn't be enough for us!
The alternative is to create one dialog line for every frame.
In roumaji and kanji, sylables/characters jitter by frame-per-frame reposition
and transparency is calculated by alpha interpolation inside the frames loop.
For subtitles, a distributor object with 2 entries is in use to get fill+border color.
Words gets another color than the one before.
]]
local dist = utils.distributor({"\\1cFFFFFF\\3c000000", "\\1c000000\\3cFFFFFF"})
function sub(line, l)
	l.start_time = line.start_time - line.infade/2
	l.end_time = line.end_time + line.outfade/2
	for wi, word in ipairs(line.words) do
		l.text = string.format("{\\fad(%d,%d)\\pos(%.3f,%.3f)\\blur2%s}%s"
								, line.infade/2, line.outfade/2, word.x, word.y, dist:get(), word.text)
		io.write_line(l)
	end
end

function roumaji(line, l)
	for si, syl in ipairs(line.syls) do
		if syl.text ~= "" then
			l.start_time = line.start_time
			l.end_time = line.start_time + syl.start_time
			l.text = string.format("{\\pos(%.3f,%.3f)}%s", syl.x, syl.y, syl.text)
			io.write_line(l)
			for s, e, i, n in utils.frames(line.start_time+syl.start_time, line.start_time+syl.end_time, 41.71) do
				l.start_time = s
				l.end_time = e
				local alpha = utils.interpolate(i/n, "&H00&", "&HFF&")
				l.text = string.format("{\\pos(%.3f,%.3f)\\alpha%s}%s", syl.x+math.random(-3,3), syl.y+math.random(-3,3), alpha, syl.text)
				io.write_line(l)
			end
		end
	end
end

function kanji(line, l)
	for ci, char in ipairs(line.chars) do
		if char.text ~= " " then
			l.start_time = line.start_time
			l.end_time = line.start_time + char.start_time
			l.text = string.format("{\\pos(%.3f,%.3f)}%s", char.x, char.y, char.text)
			io.write_line(l)
			for s, e, i, n in utils.frames(line.start_time+char.start_time, line.start_time+char.end_time, 41.71) do
				l.start_time = s
				l.end_time = e
				local alpha = utils.interpolate(i/n, "&H00&", "&HFF&")
				l.text = string.format("{\\pos(%.3f,%.3f)\\alpha%s}%s", char.x+math.random(-3,3), char.y+math.random(-3,3), alpha, char.text)
				io.write_line(l)
			end
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
