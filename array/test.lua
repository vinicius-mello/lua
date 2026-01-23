package.cpath = "./?.so;" .. package.cpath
array = require "array"

a = array.double {
	rows = 5, cols = 5
}:fill_random(2, array.int {1,7,3,5}) -- seed array

print(a)

x = array.double {
	1, 2, -1, 0, 1
}

b = a * x
print(b)
-- LU factorization

array.LU = function(A)
	local lu, p = A:LU_factor() 
	print(p)
	return
		function(B) return lu:LU_solve(p, B) end
	end
slv =  a:LU()
x = slv(b)
print("Solution x:")
print(x)

y = array.double {
	rows = 25, cols = 1
}

print("y before fill_random:")
print(y)
for i=1,10 do
	y:fill_random(3, array.int {2,4,6,8})
	print("y after fill_random:")
	print(y)
end

print(y[2])
y[2] = 5
print(y[2])
