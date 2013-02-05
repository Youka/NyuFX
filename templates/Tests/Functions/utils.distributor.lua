local distro = utils.distributor({"Hello", "world", "!"})
for i=1, 7 do
	print(distro:get())
end