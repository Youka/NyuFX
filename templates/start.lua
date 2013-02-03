-- Roumaji
function roumaji(line, l)

	--TODO

	io.write_line(l)
end

-- Kanji
function kanji(line, l)

	--TODO

	io.write_line(l)
end

-- Subtitle
function sub(line, l)

	--TODO

	io.write_line(l)
end

-- Process
for li, line in ipairs(lines) do
	if not line.comment then
		if line.styleref.alignment > 6 then
			roumaji( line, table.copy(line) )
		elseif line.styleref.alignment > 3 then
			kanji( line, table.copy(line) )
		else
			sub( line, table.copy(line) )
		end
	end
	io.progressbar(li / #lines)
end
