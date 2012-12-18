print( shape.filter("m 0 0 l 20 0 20 10 0 10", function(x, y)
	return x+10, y+5
end))