#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
struct colVal{
  int col;
  int val;
};

long factorial(int n);
//int  binom(int n, int k);
void nurmela_binom(int &vv, int & t,unsigned int **binomTable);
int LexRank(int * const X, int &n, int& k, unsigned int **binomTable);
void LexUnrank(int& r,int &n,int& k,int *T, unsigned int **binomTable);
void LexSuccessor(int *T, int n, int &k);
struct bitA{
  bool on:1;
};

#endif



