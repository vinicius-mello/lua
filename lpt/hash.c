#include <stdint.h>
#include <stddef.h>

uint64_t lpt_hash(uint64_t x) {
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ll;
  x = (x ^ (x >> 27)) * 0x94d049b13c66a8edll;
  x = x ^ (x >> 31);
  return x;
}

size_t next_tab_size(size_t n) {
  static const size_t meprimes[] = {
    17, 61, 127, 521, 
    1279, 3217, 9941,
     19937, 44497, 86243, 216091,
      756839, 1257787, 3021377, 6972593, 
      20996011, 
      42643801, 
      82589933, 136279841
  };
  for(size_t i=0;i<sizeof(meprimes)/sizeof(size_t);++i) {
    if(meprimes[i] > n) return meprimes[i];
  }
  return 0; // too big
};