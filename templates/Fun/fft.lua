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
	local data = table.create(#samples, 0)
	for si, sample in ipairs(samples) do
		data[si] = complex_t(sample, 0)
	end
	-- Process FFT
	fft(data)
	-- Complex numbers to magnitudes
	for i = 1, #data do
		data[i] = magnitude(data[i])
	end
	return data
end

-- Test
print( table.tostring( amplitudes_to_magnitudes({1, 1, 1, 1, 0, 0, 0, 0}) ) )