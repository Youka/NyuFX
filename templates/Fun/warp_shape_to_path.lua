-- Define source & destination path
local shape_path
do
	local ctx = tgdi.create_context()
	shape_path = ctx:add_path("Stretching for cookies!", "Tahoma", 60 * 8):get_path()
end
local warp_path = "m 0 0 b 1000 -500 2000 -800 3000 0"
-- Store line points into tables
local shape_points, shape_points_n = {}, 0
shape.split(shape_path, 3):gsub("([ml]?%s*)(%-?%d+)%s+(%-?%d+)", function(prefix, x, y)
	shape_points_n = shape_points_n + 1
	shape_points[shape_points_n] = {x = tonumber(x), y = tonumber(y), type = prefix, pct = nil}
end)
local warp_points, warp_points_n = {}, 0
shape.filter(shape.split(warp_path, 3), function(x, y)
	warp_points_n = warp_points_n + 1
	warp_points[warp_points_n] = {x = x, y = y, pct = nil, shape_points = {}}
end)
-- Give points percentage values
local _, _, width = shape.bounding(shape_path)
for pi, point in ipairs(shape_points) do
	point.pct = point.x / width
end
local len, last_point = 0
for pi, point in ipairs(warp_points) do
	if pi > 1 then
		len = len + math.distance(point.x - last_point.x, point.y - last_point.y)
	end
	point.pct = len
	last_point = point
end
for pi, point in ipairs(warp_points) do
	point.pct = point.pct / len
end
-- Order shape points to warp points
local shape_point_i, warp_point_i = 1, 1
while shape_point_i <= shape_points_n and warp_point_i < warp_points_n do
	local shape_point = shape_points[shape_point_i]
	local warp_point = warp_points[warp_point_i]
	if shape_point.pct <= warp_point.pct then
		warp_point.shape_points[#warp_point.shape_points+1] = shape_point
		shape_point_i = shape_point_i + 1
	else
		warp_point_i = warp_point_i + 1
	end
end
-- Calculate new shape points with referenced warp points