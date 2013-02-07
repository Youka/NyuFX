local demuxer = va.create_demuxer("templates\\Fun\\[mikeinel]draw_with_me.avi")
local stream = demuxer:get_stream(0)
local stream_info = stream:get_info()
stream:get_frames(function(frame)
	if frame.i % 20 == 0 then
		frame.width = stream_info.width
		frame.height = stream_info.height
		frame.has_alpha = false
		tgdi.save_png(frame, string.format("templates\\Fun\\F%03d.png", frame.i))
	end
	io.progressbar(frame.i / (stream_info.frames-1))
end)