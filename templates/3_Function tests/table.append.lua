local t1, t2 = {1, 2, a = "A"}, {3, 4, a = "B"}
table.append(t1, t2)
for i, v in pairs(t1) do
	print( i .. ": " .. v)
end