package.cpath = "./?.so;" .. package.cpath
array = require("array")
gl = require("gl")
glfw = require("glfw")

glfw.Init()
local major,minor,rev=glfw.GetVersion()
print(major,minor,rev)
print(glfw.GetVersionString())

function error_callback(error,description)
	print(error,description)
end

function  key_callback(window,key,scancode,action,mode)
	if key == glfw.KEY_ESCAPE and action == glfw.PRESS then
        glfw.SetWindowShouldClose(window, true)
    end
end

glfw.SetErrorCallback(error_callback)

window = glfw.CreateWindow(640, 480, "Simple example")
glfw.MakeContextCurrent(window)
gl.Init()

fsh_src = [[
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}
]]
fsh = gl.CreateShader(gl.FRAGMENT_SHADER)
gl.ShaderSource(fsh, fsh_src)
gl.CompileShader(fsh)

vsh_src = [[
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
]]
vsh = gl.CreateShader(gl.VERTEX_SHADER)
gl.ShaderSource(vsh, vsh_src)
gl.CompileShader(vsh)

print("Vertex Shader Compile Status:", gl.GetShaderInfoLog(vsh))
print("Fragment Shader Compile Status:", gl.GetShaderInfoLog(fsh))
prog = gl.CreateProgram()
gl.AttachShader(prog, vsh)
gl.AttachShader(prog, fsh)
gl.LinkProgram(prog)
gl.UseProgram(prog)

local vtx = array.float {rows=3, cols=3,
  -0.5,-0.5,0,
  0.5,-0.5,0,
  0,0.5,0
}

local vao = gl.GenVertexArray()
local vbo = gl.GenBuffer()
gl.BindVertexArray(vao)
gl.BindBuffer(gl.ARRAY_BUFFER, vbo)
gl.BufferData(gl.ARRAY_BUFFER, vtx, gl.STATIC_DRAW)
gl.VertexAttribArray(0, vtx)
gl.EnableVertexAttribArray(0)          	

glfw.SetKeyCallback(window,key_callback)

color = array.float {0.0, 0.0, 0.0, 1.0}

while not glfw.WindowShouldClose(window) do
  gl.ClearColor(0.2, 0.3, 0.3, 1.0)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  local loc = gl.GetUniformLocation(prog, "ourColor")
  time = glfw.GetTime()
  greenValue = (math.sin(time) / 2.0) + 0.5
  color:set(2, greenValue)
  gl.Uniform(loc, color)
  gl.DrawArrays(gl.TRIANGLES, 3)
  glfw.SwapBuffers(window)
  glfw.PollEvents()
end

glfw.DestroyWindow(window)
glfw.Terminate()
