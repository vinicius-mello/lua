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