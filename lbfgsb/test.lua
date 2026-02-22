package.cpath = "../libs/?.so;" .. package.cpath
package.cpath = "./?.so;" .. package.cpath
array = require "array"
lbfgsb = require "lbfgsb"

n = 100
x = array.double { rows=n, cols=1 }
g = array.double { rows=n, cols=1 }
lower = array.double { rows=n, cols=1 }
upper = array.double { rows=n, cols=1 }
nbd = array.int { rows=n, cols=1 }

for i=1,n,2 do
  lower[i] = 1
  upper[i] = 100
  nbd[i] = 2
end

for i=2,n,2 do
  lower[i] = -100
  upper[i] = 100
  nbd[i] = 2
end

for i=1,n do
  x[i] = 3
end

opt = lbfgsb.new {
  x = x,
  g = g,
  lower = lower,
  upper = upper,
  nbd = nbd,
  m = 5,
  iprint = -1
}

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
