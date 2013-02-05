print(table.tostring({
	10,
	20,
	test = "test",
	{
		a = true,
		b = false,
		{
			coroutine.create(function() print("Hi!") end)
		}
	},
}))