local frame = table.create(100*100*4, 3)
frame.width = 100
frame.height = 100
frame.has_alpha = true
for y = 0, 99 do
	for x = 0, 99 do
		local i = 1 + y * 100 * 4 + x * 4
		frame[i] = math.random(0,255)
		frame[i+1] = math.random(0,255)
		frame[i+2] = math.random(0,255)
		frame[i+3] = math.random(0,255)
	end
end
tgdi.save_png(frame, "templates\\Tests\\Functions\\noise.png")