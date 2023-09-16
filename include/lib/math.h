#ifndef _VESTIGE_LIB_MATH_H
#define _VESTIGE_LIB_MATH_H

#include <stdint.h>
#include <string.h>

// void setseed(long)  // set seed at prog start
// int irandrange(int start, int end)  // uniform dist
// float frandrange(float start, float end)  // uniform dist
// int randint()  // stateful pseudo random

int abs(int);

void setseed(uint32_t[4]);
int randint(void);
int irandrange(int start, int end);
float frandrange(float start, float end);

#endif
