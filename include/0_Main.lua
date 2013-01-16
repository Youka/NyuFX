-- Output buffer & filename
local tmp_ofile, ofilename
-- Process information
local produced_lines, process_time

-- Execution initialization function
function Init(input_filename, output_filename)
	-- Evaluate input

	-- TODO: parse ASS

	-- Prepare output
	if output_filename:len() > 0 then
		tmp_ofile = io.tmpfile()
		ofilename = output_filename
		produced_lines = 0
		process_time = os.clock()
	end
end

-- Execution exit function
function Exit()
	-- Output buffer exists
	if tmp_ofile then
		-- Copy temporary buffer to output file
		local ofile = io.open(ofilename, "w")
		if ofile then
			for line in tmp_ofile:lines() do
				ofile:write(line .. "\n")
			end
			tmp_ofile:close()
			ofile:close()
			print( string.format("Produced lines: %d\nProcess duration (in seconds): %.3f", produced_lines, os.clock() - process_time) )
		else
			error("couldn't create output file", 1)
		end
	end
end

-- Output function
function io.write_line(line)
	-- Test line
	if type(line) ~= "table" then
		error("table expected", 2)
	elseif type(line.comment) ~= "boolean" or
		type(line.layer) ~= "number" or
		type(line.start_time) ~= "number" or
		type(line.end_time) ~= "number" or
		line.style == nil or tostring(line.style) == nil or
		line.actor == nil or tostring(line.actor) == nil or
		type(line.margin_l) ~= "number" or
		type(line.margin_r) ~= "number" or
		type(line.margin_v) ~= "number" or
		line.effect == nil or tostring(line.effect) == nil or
		line.text == nil or tostring(line.text) == nil then
		error("valid table expected", 2)
	end
	-- Convert line
	local function timestamp(t)
		local temp_time = t
		-- Hours
		local h = math.floor(temp_time / 3600000)
		temp_time = temp_time % 3600000
		-- Minutes
		local m = math.floor(temp_time / 60000)
		temp_time = temp_time % 60000
		-- Seconds
		local s = math.floor(temp_time / 1000)
		temp_time = temp_time % 1000
		-- Milliseconds
		local ms = math.floor(temp_time / 10)
		-- Timestamp
		return string.format("%d:%02d:%02d.%02d", h, m, s, ms)
	end
	local text = string.format("%s: %d,%s,%s,%s,%s,%04d,%04d,%04d,%s,%s\n",
								line.comment and "Comment" or "Dialogue",
								line.layer,
								timestamp(line.start_time),
								timestamp(line.end_time),
								tostring(line.style),
								tostring(line.actor),
								line.margin_l,
								line.margin_r,
								line.margin_v,
								tostring(line.effect),
								tostring(line.text)
							)
	--Write line
	if tmp_ofile then
		tmp_ofile:write(text)
		produced_lines = produced_lines + 1
	end
end
