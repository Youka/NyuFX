for starts, ends, index, max in utils.frames(1000, 2000, 50) do
	print(string.format("%d - %d / %d - %d", starts, ends, index, max))
end