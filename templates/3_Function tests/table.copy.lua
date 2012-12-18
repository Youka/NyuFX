local t1 = {"hi"}
t2 = t1
t2[1] = "hello"
print( t1[1] )
t3 = table.copy(t1)
t3[1] = "bye"
print( t1[1] )