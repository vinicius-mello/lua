#include <stdio.h>

typedef unsigned int uint;
typedef unsigned long ulong;

ulong splitBy3(unsigned int a){
  ulong x = a & 0x1fffff; // we only look at the first 21 bits
  x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
  x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
  x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
  x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
  x = (x | x << 2) & 0x1249249249249249;
  return x;
}

ulong morton3_21(unsigned int x, unsigned int y, unsigned int z){
  ulong answer = 0;
  answer |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
  return answer;
}

void unmorton3_21(ulong code, uint *x, uint *y, uint *z)
{
  *x = 0;
  *y = 0;
  *z = 0;
  for (uint i = 0; i < 21; ++i)
  {
    *x |= (code & 1) << i;
    code >>= 1;
    *y |= (code & 1) << i;
    code >>= 1;
    *z |= (code & 1) << i;
    code >>= 1;
  }
}

ulong morton_encode(int dim, double *p) {
  double x = p[0];
  double y = p[1];
  x=(1<<19)*(x+1.0);
  y=(1<<19)*(y+1.0);
  if (dim == 2)
    return morton3_21((uint)x, (uint)y, 0);
  double z = p[2];
  z=(1<<19)*(z+1.0);
  return morton3_21((uint)x, (uint)y, (uint)z);
}

void morton_decode(ulong code, int dim, double *p) {
  uint x, y, z;
  unmorton3_21(code, &x, &y, &z);
  p[0] = ((double)x)/(1<<19)-1.0;
  p[1] = ((double)y)/(1<<19)-1.0;
  if(dim == 2) 
    return;
  p[2] = ((double)z)/(1<<19)-1.0;
}