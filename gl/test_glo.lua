package.cpath = "./?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath
local array = require("array")
local gl = require("gl")
local glfw = require("glfw")
local glo = require("glo")
local glm = require("glm")

app = glo.app.init()

local window = app:create_window(640, 480, "Simple example")

local window2 = app:create_window(640, 480, "Simple example 2")

function window2:setup()
 -- gl.Init()
 -- gl.ClearColor(0.5, 0.3, 0.1, 1.0)
end

function window2:render()
 -- gl.Clear(gl.COLOR_BUFFER_BIT)
  self:SwapBuffers()
end

function window:mouse_button(button, action, mods)
  print("mouse button: ", button, action, mods)
end

function window:size(width, height)
  print("size: ", width, height)
end

function window:char(char)
  print("char: ", char)
end

function window:setup()
  gl.Init()
  print(glfw.GetVersionString())
  print(gl.GetString(gl.VERSION))

  local fsh_src = [[
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}
]]
  local vsh_src = [[
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
]]

  self.prog = glo.program.new(vsh_src, fsh_src)

  local vtx = array.float {rows=3, cols=3,
    -0.5,-0.5,0,
    0.5,-0.5,0,
    0,0.5,0
  }
  
  self.vao = glo.vertex_array.triangle_mesh { vertices = vtx }
end

function window:key(key,scancode,action,mode)
  if key == glfw.KEY_ESCAPE and action == glfw.PRESS then
    self:SetWindowShouldClose(true)
  end
end

function window:render()
  local prog = self.prog
  local vao = self.vao
  local color = glm.vec4(0.0, 0.0, 0.0, 1.0)
  gl.ClearColor(0.2, 0.3, 0.3, 1.0)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  local time = glfw.GetTime()
  local greenValue = (math.sin(time) / 2.0) + 0.5
  color[2] = greenValue
  prog.ourColor = color
  prog:use()
  vao:draw()
  self:SwapBuffers()
end

app:setup()
app:loop()