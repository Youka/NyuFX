local stack = table.stack(5, 7, 2)
print("Pop: " .. stack:pop())
stack:push(1)
print("Size: " .. stack:size())
for value in stack:iter() do
	print(value)
end
stack:invert()
print(table.tostring(stack:totable()))