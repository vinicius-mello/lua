package.cpath = "./?.so;" .. package.cpath
package.path = "../libs/?.lua;" .. package.path
package.cpath = "../libs/?.so;" .. package.cpath
local array = require("array")
local gl = require("gl")
local glfw = require("glfw")

glfw.SetErrorCallback(function(error, description)
  print("GLFW Error: ", error, description)
end)

glfw.Init()

glfw.WindowHint(glfw.FOCUS_ON_SHOW, glfw.TRUE);
win1 = glfw.CreateWindow(640, 480, "Simple example")
glfw.MakeContextCurrent(win1)
gl.Init()
gl.ClearColor(0.2, 0.3, 0.3, 1.0)

glfw.WindowHint(glfw.FOCUS_ON_SHOW, glfw.FALSE);
win2 = glfw.CreateWindow(640, 480, "Simple example 2")
glfw.MakeContextCurrent(win2)
gl.Init()
gl.ClearColor(0.5, 0.3, 0.1, 1.0)

while not glfw.WindowShouldClose(win1) --[[and not glfw.WindowShouldClose(win2)]] do
  glfw.MakeContextCurrent(win1)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  glfw.SwapBuffers(win1)
  glfw.MakeContextCurrent(win2)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  glfw.SwapBuffers(win2)
  glfw.PollEvents()
end