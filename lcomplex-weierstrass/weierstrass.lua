local complex = require("complex")
local exp = complex.exp
local I = complex.I
local pi = math.pi
local theta1 = complex.theta1
local theta1d = complex.theta1d
local theta1ddd = complex.theta1ddd
local theta2 = complex.theta2
local theta3 = complex.theta3
local theta4 = complex.theta4

local weierstrass = {}
local function weierstrassData(w12, w32)
  local w1 = 0.5*w12
  local w3 = 0.5*w32
  local q = exp(I*pi*w3/w1)
	local f = ((pi * pi) / 12)/(w1*w1)
  local e1 = f*(theta2(0, q)^4+2*theta4(0, q)^4)
  local e2 = f*(theta2(0, q)^4-theta4(0, q)^4)
  local e3 = -f*(theta4(0, q)^4+2*theta2(0, q)^4)
  local eta1 = -pi*pi*theta1ddd(0, q)/(12*w1*theta1d(0, q))
	local data = {}
	data.w1 = w1
	data.w3 = w3
	data.q = q
	data.e1 = e1
	data.e2 = e2
	data.e3 = e3
	data.eta1 = eta1
  return data
end
weierstrass.data = weierstrassData

weierstrass.P = function(w12,w13) 
	local data = weierstrassData(w12, w13)
	local q, w1, e1 = data.q, data.w1, data.e1
	return function(z) 
		local u = (pi*z)/(2*w1)
		local t = pi*theta3(0, q)*theta4(0, q)*theta2(u, q)
		t = t/(2*w1*theta1(u,q))
		return e1+t*t
	end
end

weierstrass.zeta = function(w12,w13) 
	local data = weierstrassData(w12, w13)
	local q, w1, eta1 = data.q, data.w1, data.eta1
	return function(z) 
		local u = (pi*z)/(2*w1)
		return eta1*z/w1+(pi*theta1d(u,q))/(2*w1*theta1(u,q))
	end
end

return weierstrass
