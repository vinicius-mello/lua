package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath
local pdf=require"pdfsg"
local array = require "array"
local lpt = require "lpt2"
local adia = require "adia"
local interval = adia.interval

local c = array.double { rows=3, cols =2 }

function bb(code)
	code:coords(c)
	local Ix = interval.new(
		math.min(c:get(1,1),c:get(2,1),c:get(3,1)),
		math.max(c:get(1,1),c:get(2,1),c:get(3,1))
	)
	local Iy = interval.new(
		math.min(c:get(1,2),c:get(2,2),c:get(3,2)),
		math.max(c:get(1,2),c:get(2,2),c:get(3,2))
	)
	return Ix, Iy
end

function clown(x,y)
  x=1.5*x
  y=1.5*y-0.5
  return (y-x^2+1)^4+(x^2+y^2)^4-1
end

function taubin(x,y)
  x=2*x
  y=2*y
  local xx=x*x
  local yy=y*y
  local xy=x*y
  return 0.004+0.110*x-0.177*y-0.174*xx+0.224*xy-0.303*yy-0.168*xx*x
    +0.327*xx*y-0.087*x*yy-0.013*y*yy+0.235*xx*xx-0.667*xx*xy
    +0.745*xx*yy-0.029*xy*yy+0.072*yy*yy
end

function circle(x,y)
  return x*x+y*y-0.7
end

local x=adia.var(1)
local y=adia.var(2)
local f=clown(x,y)

adia.__call = adia.eval_di

local t = lpt.tree(1024)
t:print_stats()

t:subdivide_until(function(tree, code)
  local Ix, Iy = bb(code)
  local I, grad = f(Ix,Iy)
  local dx = grad[1]
  local dy = grad[2]
  local dg = dx*dx+dy*dy
  --return (not I:contains(0)) or (not dg:contains(0) and code:simplex_level() >=8) or code:simplex_level() >=16
  return (not I:contains(0)) or code:simplex_level() >=18
end)

t:print_stats()

coords = array.double { rows= t:vertex_count(), cols = lpt.DIM }
idxs = array.int { rows = t:leaf_count(), cols = (lpt.DIM + 1) }
t:vertex_emit_coords(coords)
t:emit_idxs(idxs)

local L=400
local output="test.pdf"
local doc=pdf.newdoc(output):setinfo("/Producer ("..pdf.version..")")
local ft=doc:setfont("Times-Roman")
local fh=doc:setfont("Helvetica")
local p,g

p=doc:newpage(L,L,"test",0)
doc:concat(195,0,0,195,200,200)
 :setlinewidth(0.0025)
 :setstroke(0.0,0.0,1.0)

function math.sign(x)
  if x < 0 then return -1
  elseif x > 0 then return 1
  else return 0
  end
end

adia.__call = adia.eval_d

newton = true

function edge_point(x0,y0,x1,y1, f0, f1)
  -- g(t) = f((1-t)*x0+t*x1, (1-t)*y0+t*y1)
  -- g'(t) = grad f . (x1-x0, y1-y0)
  if f0>0 then
    x0,y0,x1,y1 = x1,y1,x0,y0
    f0,f1 = f1,f0
  end
  local t = (-f0)/(f1-f0)
  local ix, iy
  ix, iy = (1-t)*x0+t*x1, (1-t)*y0+t*y1
  if not newton then return ix, iy end
  for i=1,3 do 
    local g, grad = f(ix,iy)
    local dg = grad[1]*(x1-x0)+grad[2]*(y1-y0)
    if math.abs(g) < 1e-6 or math.abs(dg) < 1e-6 then break end
    t = t - g/dg -- Newton step
    ix, iy = (1-t)*x0+t*x1, (1-t)*y0+t*y1
  end
  return ix, iy
end

local boundary_vert = {}
local inside_vert = {}
local closest_vert = {}

function set_closest(i,xi,yi,xj,yj,fi,fj)
  local x, y = edge_point(xi,yi,xj,yj,fi,fj)
  if not closest_vert[i] then
    closest_vert[i] = {x=x,y=y,dist=(x-xi)^2+(y-yi)^2}
  else
    local dist = (x-xi)^2+(y-yi)^2
    if dist < closest_vert[i].dist then
      closest_vert[i] = {x=x,y=y,dist=dist}
    end
  end
end

function process_triangle(i,j,k)
  local xi,yi = coords:get(i,1), coords:get(i,2)
  local xj,yj = coords:get(j,1), coords:get(j,2)
  local xk,yk = coords:get(k,1), coords:get(k,2)
  local fi = f(xi,yi)
  local fj = f(xj,yj)
  local fk = f(xk,yk)
  local si = math.sign(fi)
  local sj = math.sign(fj)
  local sk = math.sign(fk)
  for l=1,3 do
    if si<0 then
      inside_vert[i] = true
      if sj>0 then
        boundary_vert[i] = true
        set_closest(i,xi,yi,xj,yj,fi,fj)
      end
    end
    if sj<0 then
      inside_vert[j] = true
      if si>0 then
        boundary_vert[j] = true
        set_closest(j,xj,yj,xi,yi,fj,fi)
      end
    end
    i,j,k = j,k,i
    fi,fj,fk = fj,fk,fi
    xi,xj,xk = xj,xk,xi
    yi,yj,yk = yj,yk,yi
    si,sj,sk = sj,sk,si
  end
end 

function draw_triangle(x0,y0,x1,y1,x2,y2) 
--[[  local f0 = f(x0,y0)
  local f1 = f(x1,y1)
  local f2 = f(x2,y2)
  local e12 = math.sign(f1)*math.sign(f2)
  local e20 = math.sign(f2)*math.sign(f0)
  local e01 = math.sign(f0)*math.sign(f1)
  local ix, iy, jx, jy

  if f0<0 and f1<0 and f2<0 then
    doc:triangle(x0,y0,x1,y1,x2,y2):setfill(0.8,0.8,0.8):fill()
  end
]]
  doc:triangle(x0,y0,x1,y1,x2,y2)
    :setlinewidth(0.0005)
    :setstroke(0.0,0.0,1.0)
    :stroke()
--[[
  if e12<0 and e20<0 then
    ix, iy = edge_point(x1,y1,x2,y2,f1,f2)
    jx, jy = edge_point(x2,y2,x0,y0,f2,f0)
  elseif e12<0 and e01<0 then
    ix, iy = edge_point(x1,y1,x2,y2,f1,f2)
    jx, jy = edge_point(x0,y0,x1,y1,f0,f1)  
  elseif e20<0 and e01<0 then
    ix, iy = edge_point(x2,y2,x0,y0,f2,f0)
    jx, jy = edge_point(x0,y0,x1,y1,f0,f1)
  else 
    return
  end
  doc:setlinewidth(0.001)
    :setstroke(1.0,0.0,0.0)
    :moveto(ix,iy)
    :lineto(jx,jy)
    :stroke()]]
end


for r=1,idxs:rows() do
  local i = idxs:get(r,1)+1
  local j = idxs:get(r,2)+1
  local k = idxs:get(r,3)+1
  process_triangle(i,j,k)
end

for i, p in pairs(closest_vert) do
  coords:set(i,1,p.x)
  coords:set(i,2,p.y)
end

for r=1,idxs:rows() do
  local i = idxs:get(r,1)+1
  local j = idxs:get(r,2)+1
  local k = idxs:get(r,3)+1
  if (inside_vert[i] and inside_vert[j] and inside_vert[k])
   and not (boundary_vert[i] and boundary_vert[j] and boundary_vert[k]) then
    draw_triangle(
      coords:get(i,1), coords:get(i,2),
      coords:get(j,1), coords:get(j,2),
      coords:get(k,1), coords:get(k,2)
    )
  end
end

doc:enddoc()
