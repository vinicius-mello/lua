require("ply")
require("array")

print("Lendo header do arquivo ply")
filename=arg[1]
mesh=ply.load(filename)
mesh:print_header()

vtx=array.float(mesh.vertex.size,3)
idx=array.uint(mesh.face.size,3)

bb={
    xa=math.huge,
    xb=-math.huge,
    ya=math.huge,
    yb=-math.huge,
    za=math.huge,
    zb=-math.huge
}  --bounding box


function mesh.vertex_read_cb(i,reg)
    vtx:set(i,0,reg.x)
    vtx:set(i,1,reg.y)
    vtx:set(i,2,reg.z)
    bb.xa=math.min(bb.xa, reg.x)
    bb.xb=math.max(bb.xb, reg.x)
    bb.ya=math.min(bb.ya, reg.y)
    bb.yb=math.max(bb.yb, reg.y)
    bb.za=math.min(bb.za, reg.z)
    bb.zb=math.max(bb.zb, reg.z)
end

function mesh.face_read_cb(i,reg)
    for j=1,3 do 
        idx:set(i,j-1,reg.vertex_indices[j])
    end
end

print("Processando arquivo ply")
mesh:read_data()
print("Bounding Box: ["..bb.xa..","..bb.xb.."]x["..bb.ya..","..bb.yb.."]x["..bb.za..","..bb.zb.."]")

