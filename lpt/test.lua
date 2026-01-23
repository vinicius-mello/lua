package.cpath = "./?.so;" .. package.cpath
array = require "array"
lpt = require "lpt2"

local root = {}

for i=1,2 do 
  root[i] = lpt.init(i-1)
end

s = array.double { rows = lpt.DIM+1, cols = lpt.DIM }

lpt.simplex(lpt.child(root[2], 0), s)

print(s)