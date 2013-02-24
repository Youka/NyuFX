-- Conversion: audio amplitudes to magnitudes
function amplitudes_to_magnitudes(samples)
	-- Complex numbers
	local complex_t
	do
		local complex = {}
		local function tocomplex(a, b)
			if type(b) == "number" then return a, {r = b, i = 0}
			elseif type(a) == "number" then return {r = a, i = 0}, b
			else return a, b end
		end
		complex.__add = function(a, b)
			local c1, c2 = tocomplex(a, b)
			return setmetatable({r = c1.r + c2.r, i = c1.i + c2.i}, complex)
		end
		complex.__sub = function(a, b)
			local c1, c2 = tocomplex(a, b)
			return setmetatable({r = c1.r - c2.r, i = c1.i - c2.i}, complex)
		end
		complex.__mul = function(a, b)
			local c1, c2 = tocomplex(a, b)
			return setmetatable({r = c1.r * c2.r - c1.i * c2.i, i = c1.r * c2.i + c1.i * c2.r}, complex)
		end
		complex.__index = complex
		complex_t = function(r, i)
			return setmetatable({r = r, i = i}, complex)
		end
	end
	local function polar(theta)
		return complex_t(math.cos(theta), math.sin(theta))
	end
	local function magnitude(c)
		return math.sqrt(c.r^2 + c.i^2)
	end
	-- Fast Fourier Transform
	local function fft(x)
		-- Check recursion break
		local N = #x
		if N > 1 then
			-- Divide
			local even, even_n, odd, odd_n = table.create(N/2, 0), 0, table.create(N/2, 0), 0
			for i=1, N, 2 do
				even_n = even_n + 1
				even[even_n] = x[i]
			end
			for i=2, N, 2 do
				odd_n = odd_n + 1
				odd[odd_n] = x[i]
			end
			-- Conquer
			fft(even)
			fft(odd)
			--Combine
			local t
			for k = 1, N/2 do
				t = polar(-2 * math.pi * (k-1) / N) * odd[k]
				x[k] = even[k] + t
				x[k+N/2] = even[k] - t
			end
		end
	end
	-- Check argument
	if type(samples) ~= "table" then
		error("table expected", 2)
	end
	local n = #samples
	while n > 1 do
		n = n / 2
		if n ~= math.floor(n) then
			error("table must have a number of elements based on exponent of 2", 2)
		end
	end
	for si, sample in ipairs(samples) do
		if type(sample) ~= "number" then
			error("table elements have to be numbers", 2)
		end
	end
	-- Amplitudes to complex numbers
	local data, data_n = table.create(#samples, 0), 0
	for _, sample in ipairs(samples) do
		data_n = data_n + 1
		data[data_n] = complex_t(sample, 0)
	end
	-- Process FFT
	fft(data)
	-- Complex numbers to magnitudes
	for i = 1, data_n do
		data[i] = magnitude(data[i])
	end
	return data
end

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

-- Magnitudes shape parameters
local shape_width, shape_weight, magnitude_multiplicator = 700, 2, -1
-- Samples per frame with exponent to 2
local frame_samples_max = math.floor(sample_rate / 25)
local exp = 1
while 2^exp <= frame_samples_max do
	exp = exp + 1
end
frame_samples_max = 2^(exp-1)
-- Frame-wise line creation (20s, 25 FPS)
local line = lines[1]
for s, e, i, n in utils.frames(0, 20000, 40) do
	line.start_time = s
	line.end_time = e
	-- Create magnitudes for every channel
	for channel = 0, channels-1 do
		-- Extract channel samples for frame and scale for following conversion
		local frame_samples, frame_samples_n = {}, 0
		local start_sample_i = 1 + channel + math.floor(line.start_time / 1000 * sample_rate) * channels
		for i = 0, frame_samples_max-1 do
			frame_samples_n = frame_samples_n + 1
			frame_samples[frame_samples_n] = samples_collection[start_sample_i + i * channels] / 32768
		end
		-- Convert samples/amplitudes to magnitudes
		frame_samples = amplitudes_to_magnitudes(frame_samples)
		-- Create magnitudes shape
		local magnitude_shape, magnitude_shape_n = {string.format("m 0 %d l", frame_samples[1] * magnitude_multiplicator)}, 1
		local magnitude
		for x = 1, shape_width do
			magnitude = frame_samples[1 + math.floor(x/shape_width * (frame_samples_n-1))]
			magnitude_shape_n = magnitude_shape_n + 1
			magnitude_shape[magnitude_shape_n] = string.format("%d %d", x, magnitude * magnitude_multiplicator)
		end
		for x = shape_width, 0, -1 do
			magnitude = frame_samples[1 + math.floor(x/shape_width * (frame_samples_n-1))]
			magnitude_shape_n = magnitude_shape_n + 1
			magnitude_shape[magnitude_shape_n] = string.format("%d %d", x, magnitude * magnitude_multiplicator + shape_weight)
		end
		magnitude_shape = table.concat(magnitude_shape, " ")
		-- Create magnitudes dialog line
		line.text = string.format("{\\an7\\pos(2,%d)\\bord0\\p1}%s", 100 + channel * 200, magnitude_shape)
		io.write_line(line)
	end
	-- Frame creation progress
	io.progressbar(i/n)
end