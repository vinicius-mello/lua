package.path = "./?.lua;" .. package.path
package.cpath = "./?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath

local array = require("array")
local glm = require("glm")

a = glm.vec3(1,2,3)
b = glm.vec3(4,5,6)

print("a:")
print(a)
print("b:")
print(b)
print("a+b:")
print(a+b)
print("a-b:")
print(a-b)
print("a.b:")
print(glm.dot(a,b))
print("a x b:")
print(glm.cross(a,b))

q = glm.quat(1,2,3,4);
p = glm.quat(-5,6,-7,8);

print("q:")
print(q)
print("p:")
print(p)
print("q*p:")
print(glm.quat_mul(q,p))
print("q^-1:")
print(glm.quat_inv(q))
q = glm.normalize(q)
print("normalized q:")
print(q)
print("q axis:")
vq = glm.quat_axis(q)
print(vq)
print("q axis length^2:")
print(vq:dot(vq))
print("q angle:")
print(glm.quat_angle(q))

Pers = glm.perspective(math.rad(90), 1, 0.1, 10)
View = glm.lookAt(glm.vec3(0,0,3), glm.vec3(0,0,0), glm.vec3(0,1,0))
Triangle = array.float {rows=3, cols=4,
  -0.5,-0.5, 0, 1,
   0.5,-0.5, 0, 1,
   0  , 0.5, 0, 1
}

print(View)