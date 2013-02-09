-- Define source & destination path
local shape_path
do
	local ctx = tgdi.create_context()
	ctx:add_path("Stretching for cookies!", "Tahoma", 60 * 8)
	shape_path = ctx:get_path()
end
local warp_path = "m 0 0 b 1000 -500 2000 -800 3000 0"
-- Split paths to line points
shape_path = shape.split(shape_path, 3)
warp_path = shape.split(warp_path, 3)
-- Store points into tables
local shape_points, shape_points_n = {}, 0
shape_path:gsub("([ml]?%s*)(%-?%d+)%s+(%-?%d+)", function(prefix, x, y)
	shape_points_n = shape_points_n + 1
	shape_points[shape_points_n] = {tonumber(x), tonumber(y), nil, prefix}
end)
local warp_points, warp_points_n = {}, 0
shape.filter(warp_path, function(x, y)
	warp_points_n = warp_points_n + 1
	warp_points[warp_points_n] = {x, y, nil}
end)
-- Give points percentage values
local min_x, _, max_x = shape.bounding(shape_path)
local width = max_x - min_x
for pi, point in ipairs(shape_points) do
	point[3] = point[1] / width
end