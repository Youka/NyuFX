--[[
This is the recommended start template for all effect creation works.
Line manipulation and output was outsourced to functions.
You can order every effect to a function.
If your code becomes too big, use editors folding property to fold all functions
except this one you're working on for a better overview.
]]
function roumaji(line, l)
	
	--TODO
	
	io.write_line(l)
end

function kanji(line, l)
	
	--TODO
	
	io.write_line(l)
end

function sub(line, l)
	
	--TODO
	
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