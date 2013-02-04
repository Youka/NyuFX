local frame = tgdi.load_image("templates\\Tests\\Functions\\noise.png")
print(string.format(
	"Has alpha: %s\nWidth: %d\nHeight: %d\nData size: %d"
	, tostring(frame.has_alpha), frame.width, frame.height, #frame
))