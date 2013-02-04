local frame = table.create(100*100*4, 3)
frame.width = 100
frame.height = 100
frame.has_alpha = true
for y = 0, 99 do
	for x = 0, 99 do
		for channel = 0, 3 do
			frame[1 + y * 100 * 4 + x * 4 + channel] = math.random(0,255)
		end
	end
end
tgdi.save_png(frame, )