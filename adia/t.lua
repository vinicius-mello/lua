package.cpath = "./?.so;" .. package.cpath
adia = require "adia"

local x = adia.var(1)
local y = adia.var(2)

local f = (x^2+3*x*y+y^2-1)^0.5
adia.print(f)
adia.__call = adia.eval_dual
v,grad = f(1, 2)
print("f(1,2)=",v)
print(grad[1],grad[2])
