
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

