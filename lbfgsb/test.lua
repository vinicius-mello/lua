package.cpath = "./?.so;" .. package.cpath
array = require "array"
lbfgsb = require "lbfgsb"

opt = lbfgsb.new {
    n_max = 25,
    m_max = 5
}

opt.iprint = 1
local n = opt.n 

for i=1,n,2 do
  opt.lower[i] = 1
  opt.upper[i] = 100
  opt.nbd[i] = 2
end

for i=2,n,2 do
    opt.lower[i] = -100
    opt.upper[i] = 100
    opt.nbd[i] = 2
end

for i=1,n do
    opt.x[i] = 3
end

repeat
  opt:call()
  if opt.task:sub(1,2) == "FG" then
    local x = opt.x
    local g = opt.g
    local f = .25*(x[1]-1)^2
    for i=2,n do
      f = f + (x[i]- x[i-1]^2)^2
    end
    opt.f = 4*f
    local t1=x[2]-x[1]^2
    g[1] = 2*(x[1]-1)-16*x[1]*t1
    for i=2,n-1 do
      local t2=t1
      t1=x[i+1]-x[i]^2
      g[i] = 8*t2-16*x[i]*t1   
    end
    g[n] = 8*t1
  end
until opt.task:sub(1,2) ~= "FG" and opt.task:sub(1,5)~="NEW_X"
