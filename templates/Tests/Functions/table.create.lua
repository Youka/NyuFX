local t = table.create(1000, 5)
for i=1, 1000 do
	t[i] = true
end
for i=97, 101 do
	t[string.char(i)] = true
end