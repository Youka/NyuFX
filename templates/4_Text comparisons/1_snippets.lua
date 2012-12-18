local line = lines[1]
local l = table.copy(line)

--Original line
l.actor = "line"
l.text = string.format("{\\pos(%.3f,%.3f)}%s", line.x, line.y, line.text)
io.write_line(l)

--Line sylables
l.actor = "syl"
for si, syl in ipairs(line.syls) do
	l.text = string.format("{\\pos(%.3f,%.3f)}%s", syl.x, syl.y + line.height, syl.text)
	io.write_line(l)
end

--Line characters
l.actor = "char"
for ci, char in ipairs(line.chars) do
	l.text = string.format("{\\pos(%.3f,%.3f)}%s", char.x, char.y + 2 * line.height, char.text)
	io.write_line(l)
end

--Line words
l.actor = "word"
for ci, word in ipairs(line.words) do
	l.text = string.format("{\\pos(%.3f,%.3f)}%s", word.x, word.y + 3 * line.height, word.text)
	io.write_line(l)
end