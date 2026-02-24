package.cpath = "../libs/?.so;" .. package.cpath
array = require "array"
cg = require "cg"
adia = require "adia"

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

local vars = {}
for i=1, n do
  vars[i] = adia.var(i)
end

function rosenbrock(x) 
  local f = 0
  for i=1,n-1 do
    f = f + 100*(x[i+1]-x[i]^2)^2+(x[i]-1)^2
  end
  return f
end

adia.__call = adia.eval_dual_array

local fg = rosenbrock(vars)

opt:minimize(fg)

print(x)