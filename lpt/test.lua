package.cpath = "./?.so;" .. package.cpath
array = require "array"
lpt = require "lpt3"
queue = require "queue"
local t = lpt.tree(1024)
t:print_stats()
local r = t:search_all(array.double{1,1,1})

local q = queue.new()
q:pushright(r[1])

while not q:empty() do
  local node = q:popleft()
  if t:is_leaf(node) and lpt.simplex_level(node) < 3 then
    local r = t:compat_bisect(node)
    for i = 1,#r.subdivided do
      q:pushright(lpt.child(r.subdivided[i], 0))
      q:pushright(lpt.child(r.subdivided[i], 1))
    end
    for i=1,#r.vertex_id do
      print("New vertex "..r.vertex_id[i].." at ("..r.vertex_coord[i][1]..","..r.vertex_coord[i][2]..","..r.vertex_coord[i][3]..")")
    end
  end
end

local l = t:search_all(array.double{1,1,1})

--local l = t:leafs()
for s = 1,#l do
  print("Simplex "..s)
  lpt.print_simplex(l[s])
  print("")
end

t:print_stats()
