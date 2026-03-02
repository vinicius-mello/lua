local glfw = require("glfw")
local gl = require("gl")
local glm = require("glm")
local array = require("array")

local program = {}

program.new = function(vsh_src, fsh_src)
  local vsh = gl.CreateShader(gl.VERTEX_SHADER)
  gl.ShaderSource(vsh, vsh_src)
  gl.CompileShader(vsh)
  local fsh = gl.CreateShader(gl.FRAGMENT_SHADER)
  gl.ShaderSource(fsh, fsh_src)
  gl.CompileShader(fsh)
  print("Vertex Shader Compile Status:", gl.GetShaderInfoLog(vsh))
  print("Fragment Shader Compile Status:", gl.GetShaderInfoLog(fsh))
  local prog = gl.CreateProgram()
  gl.AttachShader(prog, vsh)
  gl.AttachShader(prog, fsh)
  gl.LinkProgram(prog)
  gl.DeleteShader(vsh)
  local r = {}
  r.prog = prog
  r.uniforms = gl.ListUniforms(prog)
  setmetatable(r, program)
  return r
end

program.use = function(self)
  gl.UseProgram(self.prog)
end

program.__newindex = function(self, name, value)
  local location = self.uniforms[name]
  if location==nil then
    error("Uniform '"..name.."' not found in program")
  end
  gl.Uniform(location, value)
end

program.__index = program

program.__gc = function(self)
  gl.DeleteProgram(self.prog)
end

local vertex_array = {}

local function compute_normals(vertices, indices)
  local r = array.float {rows=vertices:rows(), cols=3}
  local degree = array.float {rows=vertices:rows(), cols=1}
  degree:zero()
  r:zero()
  local vi = glm.vec3()
  local vj = glm.vec3()
  local vk = glm.vec3()
  for l=1,#indices do
    local i,j,k = indices:get(l,1)+1, indices:get(l,2)+1, indices:get(l,3)+1
    vertices:get_row(i,vi)
    vertices:get_row(j,vj)
    vertices:get_row(k,vk)
    local v1 = vj - vi
    local v2 = vk - vi
    local n = glm.cross(v1,v2)
    r:add_to_row(i, n)
    r:add_to_row(j, n)
    r:add_to_row(k, n)
    degree:add_to_entry(i, 1)
    degree:add_to_entry(j, 1)
    degree:add_to_entry(k, 1)
  end
  local vn = glm.vec3()
  for i=1,vertices:rows() do
    r:get_row(i,vn)
    vn = glm.normalize(1/degree:get(i)*vn)
    r:set_row(i, vn)
  end
  return r
end

local function triangle_soup(vertices, indices)
  local r = array.float {rows=3*#indices, cols=3}
  local v = glm.vec3()
  for l=1,#indices do
    local i,j,k = indices:get(l,1)+1, indices:get(l,2)+1, indices:get(l,3)+1
    vertices:get_row(i, v)
    r:set_row(3*l-2, v)
    vertices:get_row(j, v)
    r:set_row(3*l-1, v)
    vertices:get_row(k, v)
    r:set_row(3*l, v)
  end
  return r
end

local function flat_normals(vertices)
  local r = array.float {rows=vertices:rows(), cols=3}
  local vi = glm.vec3()
  local vj = glm.vec3()
  local vk = glm.vec3()
  for i=1,vertices:rows()/3 do
    vertices:get_row(3*i-2, vi)
    vertices:get_row(3*i-1, vj)
    vertices:get_row(3*i, vk)
    local v1 = vj - vi
    local v2 = vk - vi
    local n = glm.normalize(glm.cross(v1,v2))
    r:set_row(3*i-2, n)
    r:set_row(3*i-1, n)
    r:set_row(3*i, n)
  end
  return r
end

vertex_array.triangle_mesh = function(desc)
  if desc.indices~=nil then
    error("Indexed meshes not supported yet")
  else -- triangles
    local vao = gl.GenVertexArray()
    local vbo = gl.GenBuffer()
    gl.BindVertexArray(vao)
    gl.BindBuffer(gl.ARRAY_BUFFER, vbo)
    gl.BufferData(gl.ARRAY_BUFFER, desc.vertices, desc.usage or gl.STATIC_DRAW)
    gl.VertexAttribArray(0, desc.vertices)
    gl.EnableVertexAttribArray(0)
    gl.BindVertexArray(0)
    local r = {}
    r.vao = vao
    r.indices = nil
    r.count = desc.vertices:rows()
    setmetatable(r, vertex_array)
    return r
  end
end

vertex_array.draw = function(self)
  gl.BindVertexArray(self.vao)
  if self.indices then
    gl.DrawElements(gl.TRIANGLES, self.count, gl.UNSIGNED_INT, self.indices)
  else
    gl.DrawArrays(gl.TRIANGLES, self.count)
  end
  gl.BindVertexArray(0)
end

vertex_array.__index = vertex_array

vertex_array.__gc = function(self)
  gl.DeleteVertexArray(self.vao)
end

local callbacks = {
  key = glfw.SetKeyCallback,
  position = glfw.SetWindowPosCallback,
  size = glfw.SetWindowSizeCallback,
  close = glfw.SetWindowCloseCallback,
  refresh = glfw.SetWindowRefreshCallback,
  focus = glfw.SetWindowFocusCallback,
  iconify = glfw.SetWindowIconifyCallback,
  char = glfw.SetCharCallback,
  mouse_button = glfw.SetMouseButtonCallback,
  cursor_pos = glfw.SetCursorPosCallback,
  cursor_enter = glfw.SetCursorEnterCallback,
  scroll = glfw.SetScrollCallback,
  framebuffer_size = glfw.SetFramebufferSizeCallback,
}

glfw.__newindex = function(self, name, value)
  if callbacks[name]==nil then
    rawset(self, name, value)
  else
    callbacks[name](self, value)
  end
end

local app = {}

app.init = function(self)
  glfw.Init()
  glfw.SetErrorCallback(function(error, description)
    print("GLFW Error: ", error, description)
  end)
  local r = {}
  r.windows = {}
  setmetatable(r, app)
  return r
end

app.create_window = function(self, width, height, title)
  local w = glfw.CreateWindow(width, height, title)
  table.insert(self.windows, w)
  return w
end

app.__index = app

app.__gc = function(self)
  glfw.Terminate()
end

app.setup = function(self)
  for i=1,#self.windows do
    local w = self.windows[i]
    if w.setup~=nil then
      glfw.MakeContextCurrent(w)
      w:setup()
    end
  end
end

app.loop = function(self)
  while #self.windows>0 do
    local removed = {}
    for i=1,#self.windows do
      local w = self.windows[i]
      if glfw.WindowShouldClose(w) then
        table.insert(removed, i)
      else  
        if w.render~=nil then
          glfw.MakeContextCurrent(w)
          w:render()
          glfw.PollEvents()
        end
      end
    end
    for i=#removed,1,-1 do
      glfw.DestroyWindow(self.windows[removed[i]])
      table.remove(self.windows, removed[i])
    end
  end
end

return { program = program, vertex_array = vertex_array, app = app }