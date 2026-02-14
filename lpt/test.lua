package.cpath = "./?.so;" .. package.cpath
array = require "array"
lpt = require "lpt3"

local t = lpt.tree(1024)
t:print_stats()

t:subdivide_while(function(tree, code)
  return lpt.simplex_level(code) < 5
end)

t:print_stats()

coords = array.double { rows= t:vertex_count(), cols = lpt.DIM }
idxs = array.int { rows = t:leaf_count(), cols = (lpt.DIM + 1) }
t:vertex_emit_coords(coords)
t:emit_idxs(idxs)
print(coords)
print(idxs)
