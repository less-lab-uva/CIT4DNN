#include <stdlib.h>
#include <iostream>
#ifndef TIE_H
#define TIE_H

using namespace std;

class tieBreaker{
 public:
  tieBreaker();
  tieBreaker(int v);
  tieBreaker(const tieBreaker & tb);
  tieBreaker & operator=(const tieBreaker & tb);  
  ~tieBreaker();
  void addNum(int n){index[numb]=n; numb++;}
  void changeNum(int &n) {index[numb-1]=n;}
  int getNum() const {return numb;}
  void reWind(){numb=0;}
  int chooseRand();
 private:
 
  int *index;
  int numb;
  int size;
 
};


#endif
