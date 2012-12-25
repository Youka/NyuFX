require("lfs")

function gettext(path)
	local entries = {}
	for file in lfs.dir(path) do
		if file:find("%.cpp$") or file:find("%.hpp$") or file:find("%.c$") or file:find("%.h$") then
			for line in io.lines(path .. "\\" .. file) do
				for name in line:gmatch("_%(\"(.-)\"%)") do
					entries[name] = true
				end
			end
		end
	end
	local po_content = ""
	for entry, _ in pairs(entries) do
		po_content = string.format("%smsgid %q\nmsgstr \"\"\n\n", po_content, entry)
	end
	return po_content
end

print(gettext("..\\src"))
