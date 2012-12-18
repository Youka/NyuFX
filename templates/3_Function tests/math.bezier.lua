local point = math.bezier(0.3, {
	{0, 0, 0},
	{10, 20, -5},
	{20, -20, 5},
	{30, 0, 0}
})
print( point[1] .. " / " .. point[2] .. " / " .. point[3] )