-- Number to bytes in little-endian
function ntob(n, bytes)
	if n < 0 then
		n = n + 2^(bytes * 8 - 1)
	end
	local bin = {}
	for i = 1, bytes do
		bin[#bin+1] = n % 256
		n = math.floor(n / 256)
	end
	return string.char(unpack(bin))
end

-- Open file handle
local file = io.open("templates\\Fun\\peep.wav", "wb")

-- File chunk ID
file:write("RIFF")
-- File chunk size
file:write(ntob(1764036,4))	-- File & format chunk size (= 36 bytes) + data chunk size (see below)
-- File WAVEID
file:write("WAVE")

-- Format chunk ID
file:write("fmt ")
-- Format chunk size
file:write(ntob(16,4))	-- Short format descriptor = 16 bytes
-- Format audio format
file:write(ntob(1,2))	-- PCM = 0x1
-- Format channels
file:write(ntob(2,2))	-- 2 channels
-- Format sample rate
file:write(ntob(44100,4))	-- 44100 Hz
-- Format data rate
file:write(ntob(176400,4))	-- 44100 Hz * 2 channels * 2 bytes per sample
-- Format block align
file:write(ntob(4,2))	-- 2 channels * 2 bytes per sample
-- Format sample bits
file:write(ntob(16,2))	-- 16 bits per sample

-- Data chunk ID
file:write("data")
-- Data chunk size
file:write(ntob(1764000,4))	-- 441000 samples (10 seconds) * 2 channels * 2 bytes per sample
-- Data samples
for i = 1, 441000 do
	local wave_pct = (i-1) % 44100 / 44099
	-- Channel 1
	local wave_amplitude = math.sin(wave_pct * math.pi * 100)	-- 100 waves per second
	local s16 = math.floor((wave_amplitude + 1) / 2 * 65535 - 32768)
	file:write(ntob(s16,2))
	-- Channel 2
	wave_amplitude = math.sin(wave_pct * math.pi * 1000)	-- 1000 waves per second
	s16 = math.floor((wave_amplitude + 1) / 2 * 65535 - 32768)
	file:write(ntob(s16,2))
end

-- Close file handle
file:close()