/********************************
Author: Sravanthi Kota Venkata
********************************/

#ifndef _MSER_
#define _MSER_

#define sref(a,i) a->data[i]

//#include "sdvbs_common.h"

typedef struct
{
    int width;
    int height;
    int data[];
}I2D;

typedef struct
{
    int width;
    int height;
    unsigned int data[];
}UI2D;

typedef struct
{
    int width;
    int height;
    float data[];
}F2D;

#define subsref(a,i,j) a->data[(i) * a->width + (j)]
#define asubsref(a,i) a->data[i]
#define arrayref(a,i) a[i]

typedef int val_t;

typedef struct
{
    int width;
    int data[];
}iArray;

typedef struct
{
    int width;
    unsigned int data[];
}uiArray;

typedef struct
{
    int width;
    long long int unsigned data[];
}ulliArray;

//int script_mser();
void adv(iArray *dims, int ndims, iArray *subs_pt);
I2D* mser(I2D* I, int in_delta);

#endif


