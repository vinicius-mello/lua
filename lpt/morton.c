typedef unsigned int uint;
typedef unsigned long ulong;

ulong morton21(uint x, uint y, uint z)
{
  ulong r = 0;
  for (uint i = 0; i < 21; ++i)
  {
    r |= (ulong)(x & 1) << (3 * i);
    x >>= 1;
    r |= (ulong)(y & 1) << (3 * i + 1);
    y >>= 1;
    r |= (ulong)(z & 1) << (3 * i + 2);
    z >>= 1;
  }
  return r;
}

void unmorton21(ulong code, uint *x, uint *y, uint *z)
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
  double z = dim == 3 ? p[2] : 0.0;
  x=(1<<19)*(x+1.0);
  y=(1<<19)*(y+1.0);
  z=(1<<19)*(z+1.0);
  ulong code = morton21((uint)x, (uint)y, (uint)z);
  return code;
}

void morton_decode(ulong code, int dim, double *p) {
  uint x, y, z;
  unmorton21(code, &x, &y, &z);
  p[0] = ((double)x)/(1<<19)-1.0;
  p[1] = ((double)y)/(1<<19)-1.0;
  if(dim == 3)
    p[2] = ((double)z)/(1<<19)-1.0;
}