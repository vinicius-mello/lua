package.cpath = "../libs/?.so;" .. package.cpath
package.cpath = "./?.so;" .. package.cpath
array = require "array"
cg = require "cg"

local n = 100
local x = array.double { rows=n, cols=1 } 
local g = array.double { rows=n, cols=1 } 

opt = cg.new {
  x = x,
  g = g,
  eps = 0.00001,
  method = 1,
  iprint1 = 0
}

for i=1,n do
  x[i] = 3
end

function fg(x,g)
  local f = 0
  for i=1,n-1 do
    f = f + 100*(x[i+1]-x[i]^2)^2+(x[i]-1)^2
  end
  for i=2,n-1 do
    g[i] = 200*(x[i]-x[i-1]^2)-400*(x[i+1]-x[i]^2)*x[i]+2*(x[i]-1)
  end
  g[1] = 2*(x[1]-1)-400*(x[2]-x[1]^2)*x[1]
  g[n] = 200*(x[n]-x[n-1]^2)
  return f
end

opt:minimize(fg)

print(x)
