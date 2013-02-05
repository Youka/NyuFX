print(table.tostring(
	math.bezier(0.5, {
		{0, 0},
		{0, 200},
		{300, 200},
		{300, 0}
	})
))