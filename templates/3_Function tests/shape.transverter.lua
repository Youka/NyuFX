local transv = shape.transverter()
transv:add("{\\1c&H0000FF&}m 10 10 l 40 10 40 40 10 40")
transv:add("{\\1c&H00FF00&}m 20 0 l 40 0 40 50 20 50")
print( transv:get() )