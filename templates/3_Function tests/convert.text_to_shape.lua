local line = table.copy(lines[1])
line.text = string.format("{\\an7\\pos(%.3f,%.3f)\\p4}%s", line.left, line.top, convert.text_to_shape(line.text, line.styleref))
io.write_line(line)