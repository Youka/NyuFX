--[[
We haven't kanjis in "in.ass", so the kanji function was removed.
For subtitles, some blur was added and time offsets setted to fit for the \fad tag.
For roumajis, there's finally a sylable effect - a simple grow/shrink effect.
Every sylable has to be one dialog line, so we loop through sylable entries of current line.
Blank sylables will be ignored.
For each sylable the line text gets information about current one and line will be outputted.
This is the way of creating big karaoke files with many dialog lines for lines, sylables, characters, words, pixels, ...
]]
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

function sub(line, l)
	l.start_time = line.start_time - 400
	l.end_time = line.end_time + 400
	l.text = "{\\blur2\\fad(400,400)}" .. line.text
	io.write_line(l)
end

for li, line in ipairs(lines) do
	if line.styleref.alignment >=7 then
		roumaji( line, table.copy(line) )
	elseif line.styleref.alignment <=3 then
		sub( line, table.copy(line) )
	end
	io.progressbar(li / #lines)
end
