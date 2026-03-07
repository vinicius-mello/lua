local glm = {}

local array = require("array")

glm.vec3 = function(x,y,z)
  return array.float({rows=3, cols=1, x, y, z})
end

glm.cross = function(a,b)
  return glm.vec3(
    a[2]*b[3] - a[3]*b[2], -- | i    j   k |
    a[3]*b[1] - a[1]*b[3], -- | a1  a2  a3 |
    a[1]*b[2] - a[2]*b[1]  -- | b1  b2  b3 |
  )
end

glm.vec4 = function(x,y,z,w)
  return array.float({rows=4, cols=1, x, y, z, w})
end
glm.quat = glm.vec4

glm.quat_mul = function(q1,q2)
  -- 8 multiplications - see "THE COMPLEXITY OF THE QUATERNION PRODUCT",
  -- T. D. Howell J-C. Lafon*/
  local q11, q12, q13, q14 = q1[1], q1[2], q1[3], q1[4]
  local q21, q22, q23, q24 = q2[1], q2[2], q2[3], q2[4]
  local t0 =q11*q21;
  local t1 =q14*q23;
  local t2 =q12*q24;
  local t3 =q13*q22;
  local t4 =(q11+q12+q13+q14)*(q21+q22+q23+q24);
  local t5 =(q11+q12-q13-q14)*(q21+q22-q23-q24);
  local t6 =(q11-q12+q13-q14)*(q21-q22+q23-q24);
  local t7 =(q11-q12-q13+q14)*(q21-q22-q23+q24);
  return glm.quat(
    2*t0-(t4+t5+t6+t7)/4,
    -2*t1+(t4+t5-t6-t7)/4,
    -2*t2+(t4-t5+t6-t7)/4,
    -2*t3+(t4-t5-t6+t7)/4
  )
end

glm.quat_conj = function(q)
  return glm.quat(q[1], -q[2], -q[3], -q[4])
end

glm.quat_inv = function(q)
  return (1/(q:dot(q)))*glm.quat_conj(q)
end

glm.quat_axis = function(q)
  local s = math.sin(math.acos(q[1]))
  if math.abs(s) < 0.00001 then
    return glm.vec3(0, 0, 0)
  else
    return (1/s) * glm.vec3(q[2], q[3], q[4])
  end
end

glm.quat_angle = function(q)
  return 2*math.acos(q[1])
end

glm.quat_action = function(q, v)
  local qv = glm.quat(0, v[1], v[2], v[3])
  local r = glm.quat_mul(glm.quat_mul(q, qv), glm.quat_inv(q))
  return glm.vec3(r[2], r[3], r[4])
end

glm.quat_rotation_around = function(th, ax)
  local thtwo = th/2
  local cothtwo = math.cos(thtwo)
  local sithtwo = math.sin(thtwo)
  return glm.quat(cothtwo, sithtwo*ax[1], sithtwo*ax[2], sithtwo*ax[3]) 
end

glm.quat_rotation_between = function(v0, v1)
  local d = glm.dot(v0, v1)
  if math.abs(d) > 0.999999 then
    return glm.quat(1, 0, 0, 0)
  end
  local th = math.acos(d)
  local n = glm.cross(v0, v1)
  n = glm.normalize(n)
  return glm.quat_rotation_around(th, n);
end

glm.quat_rotation_matrix = function(q)
  local qr, qi, qj, qk = q[1], q[2], q[3], q[4]
  return glm.mat4 {
    1-2*qj*qj-2*qk*qk,   2*qi*qj-2*qr*qk,   2*qi*qk+2*qr*qj,   0,
      2*qi*qj+2*qr*qk, 1-2*qi*qi-2*qk*qk,   2*qj*qk-2*qr*qi,   0,
      2*qi*qk-2*qr*qj,   2*qj*qk+2*qr*qi, 1-2*qi*qi-2*qj*qj,   0,
                    0,                 0,                 0,   1
  }
end

glm.dot = function(a,b)
  return a:dot(b)
end

glm.normalize = function(v)
  return 1/math.sqrt(v:dot(v))*v
end

glm.project = function(v,onto)
  return glm.dot(v,onto)/glm.dot(onto,onto)*onto
end

glm.mat3 = function(entries)
  if entries==nil then
    entries = {
      1,0,0,
      0,1,0,
      0,0,1
    }
  elseif type(entries)~="table" then
    entries = {
      entries,0,0,
      0,entries,0,
      0,0,entries
    } 
  end
  entries.cols = 3
  entries.rows = 3
  return array.float(entries)
end

glm.mat4 = function(entries)
  if entries==nil then
    entries = {
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      0,0,0,1
    }
  elseif type(entries)~="table" then
    entries = {
      entries,0,0,0,
      0,entries,0,0,
      0,0,entries,0,
      0,0,0,entries
    } 
  end
  entries.cols = 4
  entries.rows = 4
  return array.float(entries)
end

glm.perspective = function(fov,ar,n,f)
  local ff = 1/math.tan(fov/2)
  return glm.mat4{
    ff/ar,     0,            0,             0,
    0,        ff,            0,             0,
    0,         0, -(f+n)/(f-n),-(2*f*n)/(f-n),
    0,         0,           -1,             0
  }
end

glm.orthographic = function(l,r,b,t,n,f)
  return glm.mat4{
    2/(r-l),   0,           0, -(r+l)/(r-l),
    0,   2/(t-b),           0, -(t+b)/(t-b),
    0,         0,    -2/(f-n), -(f+n)/(f-n),
    0,         0,           0,            1 
  }
end

glm.lookAt = function(eye,center,up)
  local n = glm.normalize(center - eye)
  local u = glm.normalize(glm.cross(up,n))
  local v = glm.cross(n,u)
  return glm.mat4 {
    u[1], u[2], u[3], -glm.dot(u,eye),
    v[1], v[2], v[3], -glm.dot(v,eye),
    n[1], n[2], n[3], -glm.dot(n,eye),
       0,    0,    0,               1
  }
end

glm.translate = function(A,v)
  return glm.mat4{
    1,    0,    0,  v[1],
    0,    1,    0,  v[2],
    0,    0,    1,  v[3],
    0,    0,    0,    1 
  }*A
end

glm.rotate = function(A,angle,axis)
  local c = math.cos(angle)
  local s = math.sin(angle)
  local t = 1-c
  local x,y,z = axis[1], axis[2], axis[3]
  return glm.mat4{
    t*x*x+c,   t*x*y-s*z, t*x*z+s*y, 0,
    t*x*y+s*z, t*y*y+c,   t*y*z-s*x, 0,
    t*x*z-s*y, t*y*z+s*x, t*z*z+c,   0,
    0,         0,         0,         1
  }*A
end

glm.scale = function(A,v)
  return glm.mat4{
    v[1],  0,     0,    0,
    0,  v[2],     0,    0,
    0,     0,  v[3],    0,
    0,     0,     0,    1
  }*A
end

return glm