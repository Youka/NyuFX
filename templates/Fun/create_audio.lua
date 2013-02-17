-- Number to bytes in little-endian
function ntob(n, bytes)
	local hex = string.format("%0" .. bytes*2 .. "x", n)
	local bin = {}
	for pos = #hex-1, #hex-bytes*2, -2 do
		bin[#bin+1] = tonumber(hex:sub(pos,pos+1), 16)
	end
	return string.char(unpack(bin))
end

-- Open file handle
local file = io.open("templates\\Fun\\peep.wav", "wb")

-- File chunk ID
file:write("RIFF")
-- File chunk size
file:write(ntob(882036,4))	-- File & format chunk size (= 36 bytes) + data chunk size (see below)
-- File WAVEID
file:write("WAVE")

-- Format chunk ID
file:write("fmt ")
-- Format chunk size
file:write(ntob(16,4))	-- Short format descriptor = 16 bytes
-- Format audio format
file:write(ntob(1,2))	-- PCM = 0x1
-- Format channels
file:write(ntob(1,2))	-- 1 channel
-- Format sample rate
file:write(ntob(44100,4))	-- 44100 Hz
-- Format data rate
file:write(ntob(88200,4))	-- 44100 Hz * 1 channel * 2 bytes per sample
-- Format block align
file:write(ntob(2,2))	-- 1 channel * 2 bytes per sample
-- Format sample bits
file:write(ntob(16,2))	-- 16 bits per sample

-- Data chunk ID
file:write("data")
-- Data chunk size
file:write(ntob(882000,4))	-- 441000 samples (10 seconds) * 1 channel * 2 bytes per sample
-- Data samples
for i = 1, 441000 do
	-- Calculate sample
	local wave_pct = (i-1) % 44100 / 44099
	local wave_amplitude = math.sin(wave_pct * math.pi * 200)	-- 200 waves per second
	local s16 = math.floor((wave_amplitude + 1) / 2 * 65535 - 32768)
	-- Write sample
	file:write(ntob(s16,2))
end

-- Close file handle
file:close()