package.cpath = "../libs/?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path

local ply = require("ply")
local array = require("array")
local lbfgsb = require("lbfgsb")
local adia = require("adia")

local max = math.max
local min = math.min
local abs = math.abs
local sqrt = math.sqrt


print("Reading mesh")
local filename=arg[1]
local mesh=ply.load(filename)
mesh:print_header()

local x = array.double { rows = mesh.vertex.size }
local y = array.double { rows = mesh.vertex.size }
local z = array.double { rows = mesh.vertex.size }

local bdr = array.double { rows = mesh.vertex.size }
local gradF = array.double { rows = mesh.vertex.size }
local gradG = array.double { rows = mesh.vertex.size }
local idx = array.uint { rows = mesh.face.size, cols = 3 }
local edges = {}

x:zero()
y:zero()
z:zero()

function mesh.vertex_read_cb(i,reg)
    x:set(i+1, reg.x)
    y:set(i+1, reg.y)
    z:set(i+1, 0)
end


function mesh.face_read_cb(i,reg)
	local vert = reg.vertex_indices
  for j=1,3 do 
    idx:set(i+1,j,vert[j])
  end
	local function insert_edge(i,j)
		i,j = min(vert[i],vert[j]),max(vert[i],vert[j])
		local k = 
			tostring(i).." "..tostring(j)
		local t = edges[k]
		if t == nil then 
			edges[k] = {faces = 1, va = i , vb = j} 
		else 
		    t.faces = t.faces+1
		end
		return t
	end
	insert_edge(1,2)
	insert_edge(1,3)
	insert_edge(2,3)
end

print("Processing PLY file")
bdr:set_all(1)
mesh:read_data()

for k,e in pairs(edges) do
	if e.faces == 1 then
		bdr:set(e.va+1, 0)
		bdr:set(e.vb+1, 0)
	end
end

z:prod_to(bdr)


function sArea(xi,yi,xj,yj,xk,yk)
	 -- 1 | 1  1  1  |
	 -- - | xi xj xk |
	 -- 2 | yi yj yk |
	 return 0.5*(xj*yk-xk*yj-xi*yk+xk*yi+xi*yj-xj*yi)
end

function projected_area() 
	local s = 0
	for l=1, mesh.face.size do
		local i,j,k = idx:get(l,1)+1, idx:get(l,2)+1, idx:get(l,3)+1
		local xi,yi = x:get(i), y:get(i)
		local xj,yj = x:get(j), y:get(j)
		local xk,yk = x:get(k), y:get(k)
		s = s + sArea(xi,yi,xj,yj,xk,yk)
	end
	return s
end

print("Projected area=", projected_area())

local mu = 0.78

local lower = array.double { rows = mesh.vertex.size }
local upper = array.double { rows = mesh.vertex.size }
local nbd = array.int { rows=mesh.vertex.size }

for i=1, mesh.vertex.size do
	lower:set(i, 0)
	upper:set(i, mu)
	nbd:set(i, 3) -- only upper bound
end

local opt = lbfgsb.new {
  x = z,
  g = gradG,
  lower = lower,
  upper = upper,
  nbd = nbd,
	pgtol = 1e-6,
  m = 7,
  iprint = 0
}

local vxi, vyi, vzi = adia.var(1), adia.var(2), adia.var(3)
local vxj, vyj, vzj = adia.var(4), adia.var(5), adia.var(6)
local vxk, vyk, vzk = adia.var(7), adia.var(8), adia.var(9)

function triangle_area(xi,yi,zi,xj,yj,zj,xk,yk,zk)
	local xy = sArea(xi,yi,xj,yj,xk,yk)
	local yz = sArea(yi,zi,yj,zj,yk,zk)
	local zx = sArea(zi,xi,zj,xj,zk,xk)
	return (xy^2+yz^2+zx^2)^0.5
end

local triangle_area_ad = triangle_area(vxi,vyi,vzi,vxj,vyj,vzj,vxk,vyk,vzk)
local barycenter_ad = 1/3*(vzi+vzj+vzk)*triangle_area_ad
local objective_ad = (mu-1/3*(vzi+vzj+vzk))*triangle_area_ad

adia.__call = adia.eval_dual

--[[
function fg_area_sq(zz,g)
	local s = 0
	g:zero()
	for l=1, mesh.face.size do
		local i,j,k = idx:get(l,1)+1, idx:get(l,2)+1, idx:get(l,3)+1
		local xi,yi,zi = x:get(i), y:get(i), zz:get(i)
		local xj,yj,zj = x:get(j), y:get(j), zz:get(j)
		local xk,yk,zk = x:get(k), y:get(k), zz:get(k)
		local f,grad = triangle_area_ad(xi,yi,zi,xj,yj,zj,xk,yk,zk)
		s = s + f
		g:add_to_entry(i, grad[3])
		g:add_to_entry(j, grad[6])
		g:add_to_entry(k, grad[9])
	end
	g:prod_to(bdr)
	g:scale(2*(s-s0))
	return (s-s0)^2
end

function fg_area(zz,g)
	local s = 0
	g:zero()
	for l=1, mesh.face.size do
		local i,j,k = idx:get(l,1)+1, idx:get(l,2)+1, idx:get(l,3)+1
		local xi,yi,zi = x:get(i), y:get(i), zz:get(i)
		local xj,yj,zj = x:get(j), y:get(j), zz:get(j)
		local xk,yk,zk = x:get(k), y:get(k), zz:get(k)
		local f,grad = triangle_area_ad(xi,yi,zi,xj,yj,zj,xk,yk,zk)
		s = s + f
		g:add_to_entry(i, grad[3])
		g:add_to_entry(j, grad[6])
		g:add_to_entry(k, grad[9])
	end
	g:prod_to(bdr)
	return s
end

function fg_barycenter(zz,g)
	local s = 0
	g:zero()
	for l=1, mesh.face.size do
		local i,j,k = idx:get(l,1)+1, idx:get(l,2)+1, idx:get(l,3)+1
		local xi,yi,zi = x:get(i), y:get(i), zz:get(i)
		local xj,yj,zj = x:get(j), y:get(j), zz:get(j)
		local xk,yk,zk = x:get(k), y:get(k), zz:get(k)
		local f,grad = barycenter_ad(xi,yi,zi,xj,yj,zj,xk,yk,zk)
		s = s + f
		g:add_to_entry(i, grad[3])
		g:add_to_entry(j, grad[6])
		g:add_to_entry(k, grad[9])
	end
	g:prod_to(bdr)
	g:scale(1/s0)
	return s/s0
end
]]

function fg(zz,g)
	local s = 0
	g:zero()
	for l=1, mesh.face.size do
		local i,j,k = idx:get(l,1)+1, idx:get(l,2)+1, idx:get(l,3)+1
		local xi,yi,zi = x:get(i), y:get(i), zz:get(i)
		local xj,yj,zj = x:get(j), y:get(j), zz:get(j)
		local xk,yk,zk = x:get(k), y:get(k), zz:get(k)
		local f,grad = objective_ad(xi,yi,zi,xj,yj,zj,xk,yk,zk)
		s = s + f
		g:add_to_entry(i, grad[3])
		g:add_to_entry(j, grad[6])
		g:add_to_entry(k, grad[9])
	end
	g:prod_to(bdr)
	return s
end

opt:minimize(fg)

ply_o = {
	format = "ascii",
	"vertex",
	"face",
	vertex = {
		size = mesh.vertex.size,
		"x", 
		"y",
		"z",
		x = "float",
		y = "float",
		z = "float",
	},
	face = { size = mesh.face.size,
		"vertex_indices",
		vertex_indices = "list uchar int"
	}
}

function ply_o.vertex_write_cb(i)
	local xc = x:get(i+1)
	local yc = y:get(i+1)
	local zc = z:get(i+1)
	xc = string.format("%.8f", xc)
	yc = string.format("%.8f", yc)
	zc = string.format("%.8f", zc)
	return { x=xc, y=yc, z=zc}
end

function ply_o.face_write_cb(id)
	local i,j,k = idx:get(id+1,1), idx:get(id+1,2), idx:get(id+1,3)
	return {
		vertex_indices = {i, j, k}
	}
end

ply.create("catenary.ply", ply_o)

ply_o:write_data()

