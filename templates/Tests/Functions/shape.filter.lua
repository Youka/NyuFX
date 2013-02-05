print(shape.filter("m 0 0 b 50 -50 50 50 0 0", function(x, y)
	x = x - 20
	print(string.format("%d|%d", x, y))
	return x, y
end))