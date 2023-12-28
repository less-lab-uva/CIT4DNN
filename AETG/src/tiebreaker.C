#include "tiebreaker.h"

tieBreaker::tieBreaker(){
  numb=0;
  size=10;
  index=new int[size];
 for(int i=0; i<size; i++)
   index[i]=0;

}
tieBreaker::tieBreaker(int v){

 numb=0;
 size=v;
 index = new int[size];

 for(int i=0; i<size; i++)
   index[i]=0;
 

}


tieBreaker::~tieBreaker(){

  delete [] index;


}

tieBreaker::tieBreaker(const tieBreaker & tb){

 numb=tb.numb;
 size=tb.size;
 index = new int[tb.size];

 for(int i=0; i<size; i++)
   index[i]=tb.index[i];
 

}


tieBreaker & tieBreaker::operator=(const tieBreaker & tb){
  

 if(this != & tb){
   delete [] index;
   size=tb.size;
   numb=tb.numb;
   index= new int[size];
   for(int i=0; i< size; i++)
     index[i]=tb.index[i];


 }
  return *this;
}


int tieBreaker::chooseRand(){
 
 
  // first decide how many elements and what modulus is

  int choice=numb-1;
  // now use this to choose the best
   choice=(rand()%numb);
   
  return index[choice];
 
}
