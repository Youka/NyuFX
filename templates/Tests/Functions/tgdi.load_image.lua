local frame = tgdi.load_image("templates\\Tests\\Functions\\noise.png")
print(string.format(
	"Has alpha: %s\nWidth: %d\nHeight: %d\nData size: %d"
	, frame.has_alpha and "true" or "false", frame.width, frame.height, #frame
))