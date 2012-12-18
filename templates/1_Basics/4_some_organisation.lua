--[[
Time to manage the effect creation process.
'io.progressbar' will be called with the current line index divided by number of lines,
so the progress percent value.
Much later you'll create big effects which take some time to generate, so it's nice to know the status.
Additionally we differ the behaviour of line manipulation by line alignment.
Lines with alignment over 6, so at the top = roumaji, getting their original tagged text.
All other lines getting fade in and out tag with 400ms duration appended.
]]
for li, line in ipairs(lines) do
	local l = table.copy(line)
	if line.styleref.alignment > 6 then
		l.text = line.k_text
		io.write_line(l)
	else
		l.text = "{\\fad(400,400)}" .. line.text
		io.write_line(l)
	end
	io.progressbar(li / #lines)
end