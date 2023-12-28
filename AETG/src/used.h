#ifndef USED_H
#define USED_H

#include <stdlib.h>
#include <iostream>
#include "cover.h"

/* This class maintains an array that keeps track of how many times a symbol
   is used in an unused pair.  Each symbol has a maximum number of times that
   it can be used.  As a starting symbol for our permutation we choose each time  
   a symbol from the maximum row.
*/



class Used {
friend ostream & operator<<(ostream & os, const Used &uv);  
  /* this class will hold the least used symbols.
     It will use a list of size of total symbols.
     When a symbol is removed it will be swapped with the last
     symbol */

 public:
  Used(Cover &cv);
  Used(const Used & us); // copy constructor
  Used & operator=(const Used &us); // assignment operator
  void Clear(Cover & cv);
  void Clear2(Cover &cv);// for new clear function of LeastUsed in repeat loop
  ~Used();
  int pickLeastUsed()const;
  void updateUsed(int val);
  int getRow(int val)const {return symcount[val];}
  
 private:
  /* the symbols matrix holds symbols used a specific number of times */
  int **symbols;  
  /* count gives the last symbol in each row */
  int *count;
  int k;
  int *symcount;  // which row symbol is in
  int *symindex;  // which column it is in
  int v;  // number of symbols
  int m;  // max number of times a symbol is used
  int least; // row with least used -- maximum row 
};


#endif
















