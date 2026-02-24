package.cpath = "../libs/?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path

local ply = require("ply")
local array = require("array")

local max = math.max
local min = math.min
local abs = math.abs
local sqrt = math.sqrt


print("Lendo header do arquivo ply")
local filename=arg[1]
local mesh=ply.load(filename)
mesh:print_header()

local x = array.float { rows = mesh.vertex.size }
local y = array.float { rows = mesh.vertex.size }
local z = array.float { rows = mesh.vertex.size }
local t = array.float { rows = mesh.vertex.size }
local bdr = array.float { rows = mesh.vertex.size }
local gradF = array.float { rows = mesh.vertex.size }
local gradG = array.float { rows = mesh.vertex.size }
local idx = array.uint { rows = mesh.face.size, cols = 3 }
local edges = {}

function mesh.vertex_read_cb(i,reg)
    x:set(i+1, reg.x)
    y:set(i+1, reg.y)
    z:set(i+1, reg.z)
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

print("Processando arquivo ply")
bdr:set_all(1)
mesh:read_data()
for k,e in pairs(edges) do
	print(e.faces, e.va, e.vb)
	if e.faces == 1 then
		bdr:set(e.va+1, 0)
		bdr:set(e.vb+1, 0)
	end
end

function sArea(xi,yi,xj,yj,xk,yk)
	 -- 1 | 1  1  1  |
	 -- - | xi xj xk |
	 -- 2 | yi yj yk |
	 return 0.5*(xj*yk-xk*yj-xi*yk+xk*yi+xi*yj-xj*yi)
end

function coords(i) 
	return x:get(i+1), y:get(i+1), z:get(i+1)
end

function triData(i,j,k)
	local xi,yi,zi = coords(i)
	local xj,yj,zj = coords(j)
	local xk,yk,zk = coords(k)
	local xy = sArea(xi,yi,xj,yj,xk,yk)
	local yz = sArea(yi,zi,yj,zj,yk,zk)
	local zx = sArea(zi,xi,zj,xj,zk,xk)
	local A = sqrt(xy*xy+yz*yz+zx*zx)
	local dzi = 1/(2*A)*(yz*(yk-yj) + zx*(xj-xk)) 
	local dzj = 1/(2*A)*(yz*(yi-yk) + zx*(xk-xi)) 
	local dzk = 1/(2*A)*(yz*(yj-yi) + zx*(xi-xj)) 
	return A,zi,zj,zk,dzi,dzj,dzk
end

function compAll()
	local s = 0
	local a = 0

	gradF:set_all(0)
	gradG:set_all(0)
	for l=0, mesh.face.size-1 do
		local i,j,k = idx:get(l+1,1), 
			idx:get(l+1,2), idx:get(l+1,3)
		local A,zi,zj,zk,dzi,dzj,dzk = triData(i,j,k)
		s = s + (zi+zj+zk)*A
		a = a + A
		local bdi = bdr:get(i+1)
		local bdj = bdr:get(j+1)
		local bdk = bdr:get(k+1)
		if bdr:get(i+1)==1 then
		  gradF:add_to_entry(i+1, 1/3*(A+(zi+zj+zk)*dzi))
			gradG:add_to_entry(i+1, dzi)
		end
		if bdr:get(j+1)==1 then 
			gradF:add_to_entry(j+1, 1/3*(A+(zi+zj+zk)*dzj))
			gradG:add_to_entry(j+1, dzj)
		end
		if bdr:get(k+1)==1 then 
			gradF:add_to_entry(k+1, 1/3*(A+(zi+zj+zk)*dzk))
			gradG:add_to_entry(k+1, dzk)
		end
	end
	return 1/3*s,a
end

local f,A0 = compAll()
local A = A0
for i=1,50000 do
	z:add_to(0.0001, gradF)
	z:add_to(-0.0001*(A-A0), gradG)
  f,A = compAll()
	if i%1000 == 0 then print(f,A) end
end 


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

function ply_o.face_write_cb(i)
	local i,j,k = idx:get(i+1,1), idx:get(i+1,2), idx:get(i+1,3)
	return {
		vertex_indices = {i, j, k}
	}
end

ply.create("out.ply", ply_o)

ply_o:write_data()

