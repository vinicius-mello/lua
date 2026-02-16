package.cpath = "../libs/?.so;" .. package.cpath
local pdf=require"pdfsg"
local array = require "array"
local lpt = require "lpt2"

local t = lpt.tree(1024)
t:print_stats()

t:subdivide_while(function(tree, code)
  return lpt.simplex_level(code) < 8
end)

t:print_stats()

coords = array.double { rows= t:vertex_count(), cols = lpt.DIM }
idxs = array.int { rows = t:leaf_count(), cols = (lpt.DIM + 1) }
t:vertex_emit_coords(coords)
t:emit_idxs(idxs)

local L=400
local output="test.pdf"
local doc=pdf.newdoc(output):setinfo("/Producer ("..pdf.version..")")
local ft=doc:setfont("Times-Roman")
local fh=doc:setfont("Helvetica")
local p,g

p=doc:newpage(L,L,"test",0)
doc:concat(195,0,0,195,200,200)
 :setlinewidth(0.0025)
 :setstroke(0.0,0.0,1.0)

for r=1,idxs:rows() do
  local i = idxs:get(r,1)+1
  local j = idxs:get(r,2)+1
  local k = idxs:get(r,3)+1
  print(i,j,k)
  doc:triangle(
    coords:get(i,1), coords:get(i,2),
    coords:get(j,1), coords:get(j,2),
    coords:get(k,1), coords:get(k,2)
  ):stroke()
end
doc:enddoc()
