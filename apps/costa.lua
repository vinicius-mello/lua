complex = require("complex")
weierstrass = require("weierstrass")
lpt = require("lpt")
array = require("array")
ply = require("ply")


-- tetrahedra tree
tree=lpt.lpt2d_tree()

-- subdivide tetrahedra three until point p be inside of a
-- tetrahedron of level level
function subdivide_to(pnt, level)
    tree:search_all(pnt:data())
		local r = {}
		local rec = {}
		repeat
			local leaf = tree:recent_code()
			rec[#rec+1] = leaf
			if leaf:simplex_level()<level then
				r[#r+1] = leaf
			end
		until not tree:recent_next()
		if #r==0 then return rec end
		for i=1,#r do
			local leaf = r[i] 
			if tree:is_leaf(leaf) then 
				tree:compat_bisect(leaf)
			end
		end
		return subdivide_to(pnt, level)
end

function print_array(a) 
	for i=1, a:height() do
		local line = ""
		for j=1, a:width() do
			line = line ..a:get(i-1, j-1).." "
		end
		print(line)
	end
end

function set_array(a, t) 
	for i=1, a:height() do
		for j=1, a:width() do
			a:set(i-1, j-1, t[i][j])
		end
	end
end

poles = array.double(8,2)
set_array(poles, {
	{-1,-1}, {0,-1},{1,-1},
	{-1,0}, {1,0},
	{-1,1}, {0,1},{1,1}
})

max_level = 15
pole_face = {}
for i=1,poles:height() do
  local r = subdivide_to(poles:row(i-1), max_level)
	for j=1,#r do
		pole_face[lpt.hash(r[j])] = true 
	end
end

fts = {}
tree:node_reset()
repeat
  if tree:node_is_leaf() then
    local cur=tree:node_code()
		if not pole_face[lpt.hash(cur)] then 
			fts[#fts+1] = cur
		end
	end
until not tree:node_next()

function test_face(f)
	return f:simplex_level()<14
end

local cf = 1
repeat
	local cur = fts[cf]
	if tree:is_leaf(cur) and test_face(cur) then 
		tree:compat_bisect(cur)
		repeat
			local leaf = tree:recent_code()
			fts[#fts+1] = leaf
		until not tree:recent_next()
	end
	cf = cf+1
until cf>#fts

vert = array.double(6)
vert_hash = {}
vtx = {}
idx = {}

tree:node_reset()
repeat
  if tree:node_is_leaf() then
    local cur=tree:node_code()
		if not pole_face[lpt.hash(cur)] then 
    	local id=tree:node_id()
    	cur:simplex(vert:data())
			local vs = {}
			vs[1] = { vert:get(0), vert:get(1) }
			vs[2] = { vert:get(2), vert:get(3) }
			vs[3] = { vert:get(4), vert:get(5) }
			if cur:orientation()<0 then
				vs[2], vs[3] = vs[3], vs[2]
			end
			local vi = {}
			for i=1,3 do
				local v = vs[i]
				local x = (0.5*v[1]+0.5) % 1
				local y = (0.5*v[2]+0.5) % 1
				local code = lpt.morton2_16(2*x-1, 2*y-1)
				local vh = vert_hash[code]
				if vh == nil then
					vtx[#vtx+1] = { x, y}
					vert_hash[code] = #vtx
					vi[i] = #vtx-1
				else
					vi[i] = vh-1
				end
			end
			idx[#idx+1] = vi
  	end
	end
until not tree:node_next()

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
