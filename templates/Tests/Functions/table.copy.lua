local t = {{2}}
local t2 = table.copy(t)
t[1][1] = nil
print(t2[1][1])