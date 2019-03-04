#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>

extern "C" void solve_system(int, int, float**, float*, float*); 

int main(int argc, char* argv[])
{
 int i;
 int m, n;
 float **a, *x, *b;

 a = new float* [m+1];
 for(i=0; i<m+1; i++)
   a[i] = new float [n+1];

 x = new float [n+1];

 b = new float [m+1];

 solve_system(m,n,a,x,b);

}

