local lpt_tree = {}

local factorial = function(n)
  local f = 1
  for i=2,n do
    f = f * i
  end
  return f
end

lpt_tree.new = function(lpt)
  local lt = {}
  setmetatable(lt, lpt_tree)
  lt.lpt = lpt
  lt.tree = {}
  for i=1,factorial(lpt.DIM) do
    lt.tree[lpt.init(i-1)] = i
  end
  return lt
end

lpt_tree.__index = lpt_tree

return lpt_tree