-- Create demuxer
local demux = va.create_demuxer("templates\\Fun\\bad_apple.mp3")
-- Get audio stream
local stream = demux:get_stream(0)
-- Get audio information
local sample_rate, channels
do
	local stream_info = stream:get_info()
	sample_rate = stream_info.sample_rate
	channels = stream_info.channels
end
-- Collect samples
local samples_collection, samples_collection_n = {}, 0
stream:get_frames(function(samples)
	for _, sample in ipairs(samples) do
		samples_collection_n = samples_collection_n + 1
		samples_collection[samples_collection_n] = sample
	end
end)
-- Amplitude shape parameters
local shape_width, shape_height, amplitude_divisor = 700, 2, 250
-- Frame-wise line creation (20s, 25 FPS)
local frame_samples_max = math.floor(sample_rate / 25  * channels)
local line = lines[1]
for s, e, i, n in utils.frames(0, 20000, 40) do
	line.start_time = s
	line.end_time = e
	-- Extract samples for frame
	local frame_samples, frame_samples_n = {}, 0
	local start_sample_i = 1 + math.floor(line.start_time / 1000 * sample_rate * channels)
	for i = 0, frame_samples_max-1 do
		frame_samples_n = frame_samples_n + 1
		frame_samples[frame_samples_n] = samples_collection[start_sample_i + i]
	end
	-- Create amplitude shape
	local amplitude_shape, amplitude_shape_n = {string.format("m 0 %d l", frame_samples[1]/amplitude_divisor)}, 1
	for x = 1, shape_width do
		amplitude_shape_n = amplitude_shape_n + 1
		amplitude_shape[amplitude_shape_n] = string.format("%d %d", x, frame_samples[1 + math.floor(x/shape_width * (frame_samples_n-1))]/amplitude_divisor)
	end
	amplitude_shape_n = amplitude_shape_n + 1
	amplitude_shape[amplitude_shape_n] = string.format("%d %d", shape_width, frame_samples[frame_samples_n]/amplitude_divisor+shape_height)
	for x = shape_width, 0, -1 do
		amplitude_shape_n = amplitude_shape_n + 1
		amplitude_shape[amplitude_shape_n] = string.format("%d %d", x, frame_samples[1 + math.floor(x/shape_width * (frame_samples_n-1))]/amplitude_divisor+shape_height)
	end
	amplitude_shape = table.concat(amplitude_shape, " ")
	-- Output amplitude shape
	line.text = string.format("{\\an7\\pos(2,198)\\bord0\\p1}%s", amplitude_shape)
	io.write_line(line)
end