#ifndef COVER_H
#define COVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <set>
#include "util.h"
#include "solve_smt.h"

using namespace std;

class Cover{
  // make a way to print out this covering array
  friend std::ostream & operator<<(std::ostream & os, const Cover &cv);  
 public:
  Cover();
  Cover(char *name,char *cname, char *density_range);
  Cover(const Cover & cv); // copy constructor
  Cover & operator=(const Cover &cv);
  ~Cover();
   int getTcount()const {return Tcount;}
   int getT()const{return t;}
   int getV()const{return v;}
   int getK()const{return k;}
   int getCol(int sym)const{assert(sym<v);return cols[sym];}
   int getVsize(int sym)const{return karray[sym];}
   int getSym(int field, int index)const{return varray[field][index];}
   int getIndex(int, int) ;

   int getkCombo(int col) const {return kcombo[col];}
   unsigned int ** getTableRef(){return binomTable;}
   bitA * getCoveredRef(){return Covered;}
  
   //all v choose t pairs' covered or not array Covered, precomputed n choose m Table binomialTable, 
   //all pairs count TsetCount=k choose t * v^t,Covered Size size (k*v) choose t 

   void CoveredUpdate(unsigned int ** ,int&,int);
   void KVCombo(int **,unsigned int **,int);
   int getkvCombo(int col,int v) const {return kvcombo[col][v];}
   void printUncovered(bitA *,unsigned int **,int,int,int);
   void updateKcombo(int, int, int **);
   int ** getkvcomboRef(){return kvcombo;}
	void CoverForRepeat(char *);
	int getTcountBeforeUpdated()const {return Tcount_BeforeCoveredUpdating;}
	int * getKarray(){return karray;}

	void columnCombComputing(int , int,vector<int> );
	void createConcreteValueCombSet_CoveredUpdate();
	void CoveredUpdateInLoop(vector<int>);
	void createConcreteValueCombo(int , vector <int> , vector<int> );
	void printColumnCombvalueSet();
	void printConcreteValueCombSet();
	void clear();

 private:

  int k;
  int v;
  int t;
  int Tcount;
  int Tcount_BeforeCoveredUpdating;

  
  int *kcombo;  // holds how many times each symbol from a column is used
  int *cols;     // holds the column for each symbol
  unsigned int **binomTable;  // holds the binomial table
 

public:
  
  int **varray;  // holds the values of each field 
  int *karray;   // holds the sizes of each field
  int **kvcombo;// holds how many times each symbol from a column is used by considering constraint
  int ** kvcombo_ForUsedClear; //cover.kvcombo; holding for kvcombo [][] for kvcombo updating
  int *Kcombo_ForUsedClear; //cover.kcombo; holding for kcombo[] for kcombo updating
  
  CA_SMT* cover_smt;

  set<vector <int> > columnCombSet;
  vector<vector <int> > concreteValueCombSet;
  vector<int> oneColumCombo;
  int CoveredSize;
   bitA *Covered;	
};


#endif

