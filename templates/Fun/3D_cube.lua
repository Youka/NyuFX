--Cube rectangles
local cube = {nil, nil, nil, nil, nil, nil}
-- Front rectangle (frontface has points clockwise; 1 pixel overhang to fill gaps)
cube[1] = {
	{-101, -101, 100},
	{101, -101, 100},
	{101, 101, 100},
	{-101, 101, 100}
}
-- Back rectangle
cube[2] = {
	math.rotate(cube[1][1], "y", 180),
	math.rotate(cube[1][2], "y", 180),
	math.rotate(cube[1][3], "y", 180),
	math.rotate(cube[1][4], "y", 180)
}
-- Left rectangle
cube[3] = {
	math.rotate(cube[1][1], "y", -90),
	math.rotate(cube[1][2], "y", -90),
	math.rotate(cube[1][3], "y", -90),
	math.rotate(cube[1][4], "y", -90)
}
-- Right rectangle
cube[4] = {
	math.rotate(cube[1][1], "y", 90),
	math.rotate(cube[1][2], "y", 90),
	math.rotate(cube[1][3], "y", 90),
	math.rotate(cube[1][4], "y", 90)
}
-- Top rectangle
cube[5] = {
	math.rotate(cube[1][1], "x", 90),
	math.rotate(cube[1][2], "x", 90),
	math.rotate(cube[1][3], "x", 90),
	math.rotate(cube[1][4], "x", 90)
}
-- Bottom rectangle
cube[6] = {
	math.rotate(cube[1][1], "x", -90),
	math.rotate(cube[1][2], "x", -90),
	math.rotate(cube[1][3], "x", -90),
	math.rotate(cube[1][4], "x", -90)
}
-- Frame-wise cube building (30s, 25 FPS)
local line = lines[1]
for s, e, i, n in utils.frames(0, 30000, 40) do
	line.start_time = s
	line.end_time = e
	-- Define rotations
	local x_rotation = i/n * 720
	local y_rotation = i/n * 540
	local z_rotation = i/n * 360
	-- Iterate through cube rectangles (copy)
	for rect_i, rect in ipairs(table.copy(cube)) do
		-- Rotate rectangle points
		for point_i = 1, #rect do
			rect[point_i] = math.rotate(math.rotate(math.rotate(rect[point_i], "x", x_rotation), "y", y_rotation), "z", z_rotation)
		end
		-- Calculate rectangle orthogonal vector
		local ortho = math.ortho({rect[2][1] - rect[1][1], rect[2][2] - rect[1][2], rect[2][3] - rect[1][3]}, {rect[4][1] - rect[1][1], rect[4][2] - rect[1][2], rect[4][3] - rect[1][3]})
		-- Further process only for visible frontface
		if ortho[3] > 0 then
			-- Calculate degree light direction <-> rectangle (fixed to 0-90 degree)
			local deg = math.trim(math.abs(math.degree(ortho, {0, -0.5, 1})), 0, 90)
			-- Create rectangle dialog line
			line.text = string.format("{\\an7\\pos(300,200)\\bord0\\1c%s\\p1}m %d %d l %d %d %d %d %d %d"
									, utils.interpolate(deg / 90, "&HFFFFFF&", "&H000000&")
									, rect[1][1], rect[1][2], rect[2][1], rect[2][2], rect[3][1], rect[3][2], rect[4][1], rect[4][2])
			io.write_line(line)
		end
	end
end