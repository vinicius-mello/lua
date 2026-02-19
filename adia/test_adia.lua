package.cpath = "./?.so;" .. package.cpath
adia = require "adia"

interval = adia.interval
eps = 0.01
a = 3
b = -2
print("a=",a)
print("b=",b)

ai = interval.new(a-eps,a+eps)
bi = interval.new(b-eps,b+eps)

print("ai=",ai)
print("bi=",bi)

print("Testing interval arithmetic...")

print("ai+bi=",ai+bi)
print("ai-bi=",ai-bi)
print("ai*bi=",ai*bi)
print("ai/bi=",ai/bi)
print("ai^2=",ai^2)
print("ai^3=",ai^3)

print("Testing RPN...")
local x = adia.var(1)
local y = adia.var(2)

local f = x^2+3*x*y+y^2-1
adia.print(f)
print("Testing evaluation...")
print("f(a,b)=",f(a,b))
adia.__call = adia.eval_d
v,grad = f(a, b)
print("f(a,b)=",v)
print("grad f(a,b)=(",grad[1], grad[2], ")")
adia.__call = adia.eval_i
print("f(ai,bi)=",f(ai,bi))
adia.__call = adia.eval_di
v,grad = f(ai, bi)
print("f(ai,bi)=",v)
print("grad f(ai,bi)=(",grad[1], grad[2], ")")
