package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath
local pdf=require"pdfsg"
local array = require "array"
local lpt = require "lpt2"
local adia = require "adia"
local interval = adia.interval
local cg = require "cg"
local ply = require "ply"

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
  y=2*y+0.4
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
local f=circle(x,y)

adia.__call = adia.eval_dual_interval

local t = lpt.tree(1024)
t:print_stats()

local max_depth = 14

t:subdivide_until(function(tree, code)
  local Ix, Iy = bb(code)
  local I, grad = f(Ix,Iy)
  local dx = grad[1]
  local dy = grad[2]
  local dg = dx*dx+dy*dy
  --return (not I:contains(0)) or (not dg:contains(0) and code:simplex_level() >=max_depth/2) or code:simplex_level() >=max_depth
  --return (not I:contains(0)) or code:simplex_level() >=max_depth
  return code:simplex_level() >=max_depth
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

adia.__call = adia.eval_dual

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

function project_curve(x0,y0,x1,y1)
  local dx = (x1-x0)/2
  local dy = (y1-y0)/2
  local len = math.sqrt(dx*dx+dy*dy)
  local f0, grad0 = f(x0,y0)
  local n = math.sqrt(grad0[1]*grad0[1]+grad0[2]*grad0[2])
  grad0[1], grad0[2] = grad0[1]/n, grad0[2]/n
  dx, dy = grad0[1]*len, grad0[2]*len
  for i = 1,10 do
    x0,y0 = x0+dx, y0+dy
    local f = f(x0,y0)
    if f*f0 < 0 then
      dx = -dx/2
      dy = -dy/2
    end
    f0 = f
  end
  return x0, y0
end

function set_closest(i,xi,yi,xj,yj,fi,fj)
  --local x, y = edge_point(xi,yi,xj,yj,fi,fj)
  local x,y = project_curve(xi,yi,xj,yj)
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

local inside_triangle = {}

for r=1,idxs:rows() do
  local i = idxs:get(r,1)+1
  local j = idxs:get(r,2)+1
  local k = idxs:get(r,3)+1
  if (inside_vert[i] and inside_vert[j] and inside_vert[k])
   and not (boundary_vert[i] and boundary_vert[j] and boundary_vert[k]) then
    inside_triangle[#inside_triangle+1] = r

    local xi,yi =  coords:get(i,1), coords:get(i,2)
    local xj,yj =  coords:get(j,1), coords:get(j,2)
    local xk,yk =  coords:get(k,1), coords:get(k,2)
    for l=1,3 do
      local ux=xj-xi
      local uy=yj-yi
      local vx=xk-xi
      local vy=yk-yi
      if math.abs(ux*vx+uy*vy) < 1e-8 then
        break
      end
      xi,yi,xj,yj,xk,yk = xj,yj,xk,yk,xi,yi
      i,j,k = j,k,i
    end
    idxs:set(r,1,i-1)  
    idxs:set(r,2,j-1)
    idxs:set(r,3,k-1)
  end
end

for i, p in pairs(closest_vert) do
  coords:set(i,1,p.x)
  coords:set(i,2,p.y)
end

local k_angle = 8.0

function triangle_cost(x0,y0,x1,y1,x2,y2)
  local ux = x1-x0
  local uy = y1-y0
  local vx = x2-x0
  local vy = y2-y0
  local wx = x2-x1
  local wy = y2-y1
  local nu2 = ux*ux+uy*uy
  local nv2 = vx*vx+vy*vy
  local nw2 = wx*wx+wy*wy
  return (k_angle*(ux*vx+uy*vy)^2/(nu2*nv2)+nu2/nv2+nv2/nu2-2.0)
end

adia.__call = adia.eval_dual_array

local triangle_cost_ad = triangle_cost(
  adia.var(1), adia.var(2),
  adia.var(3), adia.var(4),
  adia.var(5), adia.var(6)
)

local xt = array.double { rows=6, cols=1 }
local gt = array.double { rows=6, cols=1 }

function fg(x,g)
  g:zero()
  local f_total = 0
  for l=1,#inside_triangle do
    local r = inside_triangle[l]
    local i = idxs:get(r,1)+1
    local j = idxs:get(r,2)+1 
    local k = idxs:get(r,3)+1
    xt:set(1,1,x:get(i,1))
    xt:set(2,1,x:get(i,2))
    xt:set(3,1,x:get(j,1))
    xt:set(4,1,x:get(j,2))
    xt:set(5,1,x:get(k,1))
    xt:set(6,1,x:get(k,2))
    local ft = triangle_cost_ad(xt,gt)
    f_total = f_total + ft
    g:add_to_entry(i,1,gt:get(1,1))
    g:add_to_entry(i,2,gt:get(2,1))
    g:add_to_entry(j,1,gt:get(3,1))
    g:add_to_entry(j,2,gt:get(4,1))
    g:add_to_entry(k,1,gt:get(5,1))
    g:add_to_entry(k,2,gt:get(6,1))
  end
  for i, p in pairs(closest_vert) do
--[[    local xi,yi = coords:get(i,1), coords:get(i,2)
    local fi, grad = f:eval_dual(xi,yi)
    local gx, gy = g:get(i,1), g:get(i,2)
    local proj_g = (gx*grad[1]+gy*grad[2])/(grad[1]*grad[1]+grad[2]*grad[2])
    g:set(i,1,gx-proj_g*grad[1])
    g:set(i,2,gy-proj_g*grad[2])]]
    g:set(i,1,0)
    g:set(i,2,0)
  end
  return f_total
end

local grad = array.double { rows= t:vertex_count(), cols = lpt.DIM }

local opt = cg.new {
  x = coords,
  g = grad,
  eps = 1e-6,
  method = 3,
  iprint1 = 0
}

opt:minimize(fg)


for l=1,#inside_triangle do
  local r = inside_triangle[l]
  local i = idxs:get(r,1)+1
  local j = idxs:get(r,2)+1 
  local k = idxs:get(r,3)+1
  draw_triangle(
    coords:get(i,1), coords:get(i,2),
    coords:get(j,1), coords:get(j,2),
    coords:get(k,1), coords:get(k,2)
  )
end

ply_o = {
	format = "ascii",
	"vertex",
	"face",
	vertex = {
		size = t:vertex_count(),
		"x", 
		"y",
		"z",
		x = "float",
		y = "float",
		z = "float",
	},
	face = { size = #inside_triangle,
		"vertex_indices",
		vertex_indices = "list uchar int"
	}
}

local inside_idx = {}
for l=1,#inside_triangle do
  local r = inside_triangle[l]
  local i = idxs:get(r,1)
  local j = idxs:get(r,2)
  local k = idxs:get(r,3)
  inside_idx[#inside_idx+1] = {i,j,k}
end

function ply_o.vertex_write_cb(i)
--	x = string.format("%.8f", x)
--	y = string.format("%.8f", y)
--	z = string.format("%.8f", z)
	return {x=coords:get(i+1,1), y=coords:get(i+1,2), z=0}
end

function ply_o.face_write_cb(i)
	return {
		vertex_indices = inside_idx[i+1]
	}
end

ply.create("mesh.ply", ply_o)

ply_o:write_data()


doc:enddoc()
