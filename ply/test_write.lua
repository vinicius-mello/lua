ply = require("ply")

n = 6 
m = 6

function ind(i,j) 
	return j*(n+1)+i
end

vtx = {}
idx = {}

for j=0,m do
	for i=0,n do 
		vtx[#vtx+1] = {x=i,y=j,z=0}
	end
end

for j=0,m-1 do
	for i=0,n-1 do 
		local k = (-1)^(i+j)
		if k<0 then 
			idx[#idx+1] = {ind(i,j),ind(i+1,j),ind(i,j+1)}
			idx[#idx+1] = {ind(i+1,j),ind(i+1,j+1),ind(i,j+1)}
		else 
			idx[#idx+1] = {ind(i,j),ind(i+1,j),ind(i+1,j+1)}
			idx[#idx+1] = {ind(i,j),ind(i+1,j+1),ind(i,j+1)}
		end
	end
end

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

function ply_o.vertex_write_cb(i)
--	x = string.format("%.8f", x)
--	y = string.format("%.8f", y)
--	z = string.format("%.8f", z)
	return vtx[i+1]
end

function ply_o.face_write_cb(i)
	return {
		vertex_indices = idx[i+1]
	}
end

ply.create("teste.ply", ply_o)

ply_o:write_data()
