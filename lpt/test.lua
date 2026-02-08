package.cpath = "./?.so;" .. package.cpath
array = require "array"
lpt = require "lpt3"
queue = require "queue"

local t = lpt.tree(1024)
t:print_stats()
local r = t:search_all(array.double{1,1,1})
t:print_stats()

local q = queue.new()
q:pushright(r[1])

while not q:empty() do
  local node = q:popleft()
  lpt.print_simplex(node)
  print("")
  if t:is_leaf(node) and lpt.simplex_level(node) < 2 then
    print("Bisecting simplex...")
    local subdivided = t:compat_bisect(node)
    print("Subdivided into "..#subdivided.." simplex(s)")
    for i = 1,#subdivided do
      q:pushright(lpt.child(subdivided[i], 0))
      q:pushright(lpt.child(subdivided[i], 1))
    end
  end
end
t:print_stats()

local l = t:search_all(array.double{1,1,1})

--local l = t:leafs()
for s = 1,#l do
  print("Simplex "..s)
  lpt.print_simplex(l[s])
  print("")
end

t:print_stats()
