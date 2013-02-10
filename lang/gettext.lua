require("lfs")

function gettext(path)
	local entries = {}
	for file in lfs.dir(path) do
		if file:find("%.cpp$") or file:find("%.hpp$") or file:find("%.c$") or file:find("%.h$") then
			for line in io.lines(path .. "\\" .. file) do
				for text in line:gmatch('_%("(.-)"%)') do
					entries[text] = true
				end
			end
		end
	end
	local po_content, po_content_n = {}, 0
	for entry in pairs(entries) do
		po_content_n = po_content_n + 1
		po_content[po_content_n] = string.format('msgid %q\nmsgstr ""', entry)
	end
	return table.concat(po_content, "\n\n")
end

local f = io.open("nyufx.po", "w")
f:write(gettext("..\\src"))
f:close()
