package.cpath = "./?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath
local array = require("array")
local gl = require("gl")
local glfw = require("glfw")
local glo = require("glo")
local glm = require("glm")

app = glo.app.init()

local window = app:create_window(512, 512, "Simple example")

function window:mouse_button(button, action, mods)
  if button == glfw.MOUSE_BUTTON_LEFT then
    if action == glfw.PRESS then
      self.pressed = true
      self.cursor_pos_x, self.cursor_pos_y = self:GetCursorPos()
      self.dragging = false
    elseif action == glfw.RELEASE then
      if not self.dragging then
        print("clicked")
      else
        self.pressed = false
        self.dragging = false
      end
    end
  end
end

function window:cursor_pos(x, y)
  if self.pressed then
    self.dragging = true
    self.cursor_pos_prev_x, self.cursor_pos_prev_y = self.cursor_pos_x, self.cursor_pos_y
    self.cursor_pos_x, self.cursor_pos_y = x, y
    self.arcball:drag(self.cursor_pos_prev_x, self.cursor_pos_prev_y,
                    self.cursor_pos_x, self.cursor_pos_y)
  end
end

function window:cursor_enter(v)
  if v == 1 then
    print("cursor entered")
    self.dragging = false
    self.pressed = false
  else
    print("cursor left")
    self.dragging = false
    self.pressed = false
  end
end

function window:size(width, height)
  print("size: ", width, height)
  self.width, self.height = width, height
  self.arcball:reset(width, height)
end

function window:char(char)
  print("char: ", char)
end

function window:setup()
  self.dragging = false
  self.pressed = false
  self.arcball = glo.arcball.new()
  self.arcball:reset(self.width, self.height)
  gl.Init()
  print(glfw.GetVersionString())
  print(gl.GetString(gl.VERSION))

  local fsh_src = [[
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor;
varying vec3 Normal;

void main()
{
    vec3 lightDir = normalize(vec3(0, 0, 3));
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    FragColor = vec4(diffuse * ourColor.rgb, ourColor.a);
}
]]
  local vsh_src = [[
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 Normal;

void main()
{
  gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  Normal = normalize((view * model * vec4(aNormal, 0)).xyz);
}
]]

  self.prog = glo.program.new(vsh_src, fsh_src)

  local fov = math.rad(90)

  --self.prog.projection = glm.perspective(fov, 1.0, 0.1, 100)
  self.prog.projection = glm.orthographic(-2,2,-2,2,0.1,100)
  gl.Enable(gl.CULL_FACE);
  --gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)

  local vtx = array.float {rows=8, cols=3,
    -1,-1,-1, -- 0
    -1,-1, 1, -- 1      2----6
    -1, 1,-1, -- 2     /|   /|
    -1, 1, 1, -- 3    3-0--7 4
     1,-1,-1, -- 4    |/   |/
     1,-1, 1, -- 5    1----5
     1, 1,-1, -- 6
     1, 1, 1  -- 7
  }
  local idx = array.uint {rows=12, cols=3,
    0,1,2, 1,3,2,
    4,6,5, 5,6,7,
    0,4,1, 1,4,5,
    2,3,6, 3,7,6,
    0,2,4, 2,6,4,
    1,5,3, 3,5,7
  }
  self.vao = glo.vertex_array.triangle_mesh {
    vertices = vtx, indices = idx, normals = 'flat' }
end

function window:key(key,scancode,action,mode)
  if key == glfw.KEY_ESCAPE and action == glfw.PRESS then
    self:SetWindowShouldClose(true)
  end
end

function window:render()
  local prog = self.prog
  local vao = self.vao
  local color = glm.vec4(1.0, 0.0, 0.0, 1.0)
  gl.ClearColor(0.2, 0.3, 0.3, 1.0)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  local time = glfw.GetTime()
  local greenValue = (math.sin(time) / 2.0) + 0.5
  local co = math.cos(time)
  local si = math.sin(time)
  --[2] = greenValue
  prog.ourColor = color
  prog.view = glm.lookAt(glm.vec3(0,0,3), glm.vec3(0,0,0), glm.vec3(0,1,0))
  prog.model = self.arcball:matrix()
  prog:use()
  vao:draw()
  self:SwapBuffers()
end

app:setup()
app:loop()