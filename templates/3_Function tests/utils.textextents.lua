local text = "Test"
local style = {
	fontname = "Arial",
	fontsize = 60,
	bold = true,
	italic = true,
	underline = false,
	strikeout = false,
	encoding = 1,
	scale_x = 100,
	scale_y = 100,
	spacing = 1,
}
local w, h, a, d, i, e = utils.textextents(text, style)
print( "Width: " .. w .. "\nHeight: " .. h .. "\nAscent: " .. a .. "\nDescent: " .. d .. "\nInternal lead: " .. i .. "\nExternal lead: " .. e )