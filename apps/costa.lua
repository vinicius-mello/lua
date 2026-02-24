package.cpath = "../libs/?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path

complex = require("complex")
weierstrass = require("weierstrass")
lpt = require("lpt2")
array = require("array")
ply = require("ply")

local t = lpt.tree(1024)

local min_depth = 8
local max_depth = 12

t:subdivide_until(function(tree, code)
	return code:simplex_level() >=min_depth
end)

local poles = array.double { rows = 8, cols = 2,
  -1, -1,
	 0, -1,
	 1, -1,
	-1,  0,
	 1,  0,
	-1,  1,
	 0,  1,
	 1,  1
}

local pt = array.double {rows = 2}

local nb_infinity = {}
for i=1,poles:rows() do
	pt:set(1, poles:get(i,1))
	pt:set(2, poles:get(i,2))
	local v = t:subdivide_point(pt, max_depth)
	for j=1,#v do
		nb_infinity[v[j]:id()] = true
	end
end

function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

local vtx = t:vertex_emit_coords()
local idx = array.uint { rows = t:leaf_count()-tablelength(nb_infinity), cols = 3 }

local ii = 0
t:visit_leafs(function(tree, leaf)
	if nb_infinity[leaf:id()] then return end
	local v = tree:leaf_vertex_ids(leaf)
	idx:set(ii, 1, v[1])
	idx:set(ii, 2, v[2])
	idx:set(ii, 3, v[3])
	ii = ii + 1
end)

ply_o = {
	format = "ascii",
	"vertex",
	"face",
	vertex = {
		size = #vtx,
		"x", 
		"y",
		"z",
		x = "float",
		y = "float",
		z = "float",
	},
	face = { size = #idx,
		"vertex_indices",
		vertex_indices = "list uchar int"
	}
}

local P = weierstrass.P(1, complex.I)
local zeta = weierstrass.zeta(1, complex.I)
local wData = weierstrass.data(1, complex.I)
local e1 = wData.e1
local I = complex.I
local pi = math.pi

function costa(u, v) 
	local w = u+v*I
	local a = zeta(w-0.5)
  local b = zeta(w-0.5*I)
  local ab = a-b
  local c = zeta(w)
  local p = P(w)
  local x = complex.real(pi*(u+(pi/4)/e1)-c+(pi/2)/e1*ab)
  local y = complex.real(pi*(v+(pi/4)/e1)-I*c-I*(pi/2)/e1*ab)
  local z = math.sqrt(pi/2) * math.log(complex.abs((p-e1)/(p+e1)))
	return x, y, z
end

function ply_o.vertex_write_cb(i)
	local u = vtx[i+1][1]
	local v = vtx[i+1][2]
	local x,y,z = costa(u,v)
	--local x,y,z = u, v, 0
	x = string.format("%.8f", x)
	y = string.format("%.8f", y)
	z = string.format("%.8f", z)
	return {
		x = x,
		y = y,
		z = z
	}
end

function ply_o.face_write_cb(i)
	return {
		vertex_indices = idx[i+1]
	}
end

ply.create("teste.ply", ply_o)

ply_o:write_data()
