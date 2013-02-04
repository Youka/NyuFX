local demuxer = va.create_demuxer("templates\\Tests\\Functions\\colorbars.mkv")
local demuxer_info = demuxer:get_info()
print(table.tostring(demuxer_info))
for i=0, demuxer_info.streams-1 do
	local stream = demuxer:get_stream(i)
	local stream_info = stream:get_info()
	print(table.tostring(stream_info))
	if stream_info.type == "video" then
		stream:get_frames(function(frame)
			print(string.format("Frame %d - Type: %s - Size: %d", frame.i, frame.type, #frame))
		end)
	elseif stream_info.type == "audio" then
		stream:get_frames(function(samples)
			print(string.format("Start sample index: %d - Samples: %d", samples.i, #samples))
		end)
	elseif stream_info.type == "subtitle" then
		stream:get_frames(function(sub)
			print(sub)
		end)
	end
end