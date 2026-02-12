#ifndef MORTON_H
#define MORTON_H

unsigned long morton_encode(int dim, double *p);
void morton_decode(unsigned long code, int dim, double *p);

#endif // MORTON_H