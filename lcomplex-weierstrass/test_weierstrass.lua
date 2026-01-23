complex = require("complex")
weierstrass = require("weierstrass")

local I = complex.I

local P = weierstrass.P(1,I)

print(P(0.3+0.2*I))

