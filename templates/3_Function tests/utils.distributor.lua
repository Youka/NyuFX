local dist = utils.distributor({"&H0000FF&", "&H00FF00&", "&HFF0000&"})
for i=1, 4 do
	print( dist:get() )
end