/*****************************************
   Myra B. Cohen
   Dec 2001
   This is a ranking program to crbineate
   a ranking in lexocographic order for all t-subsets

******************************************/

#include "util.h"
#include "used.h"









void nurmela_binom(int & maxv,int & t,unsigned int **binomTable){

  int v,k;

  for(v = 0; v <= maxv; v++) {
    binomTable[v][0] = 1;
    if(v <t)
      binomTable[v][v+1] = 0;
    if(v<=t)
      binomTable[v][v]=1;
    for(k = 1; (k <= t && k <= v-1); k++) {
      binomTable[v][k] = binomTable[v - 1][k - 1] + binomTable[v - 1][k];
      if(binomTable[v][k] < binomTable[v - 1][k - 1] ||
	 binomTable[v][k] < binomTable[v - 1][k] ||
	 binomTable[v - 1][k - 1] == 0 ||
	 (binomTable[v - 1][k] == 0 && k < v))
	binomTable[v][k] = 0; /* there was an overflow */
    }
  }

  // debugging  
 /* for(int i=0; i<=maxv; i++) {
    for(int j=0; j<=t; j++)
      cout << "   " << binomTable[i][j];
    cout << endl;
  }
  
  //*/

}





int LexRank(int * const X, int & n, int & k,unsigned int **binomTable){

  /* Taken from Kreher and Stinson page 43 */
  
  /* first move these to a new array of size one larger then k and make
     them 1 based numbers instead of 0 based  by adding one
  */

  int *T=new int[k+1];
  T[0]=0;
  for(int i=0; i< k; i++){
    T[i+1]=X[i]+1;

  }




  /*  cout << "debugging for rank " << endl;
  for(int i=1; i<= k; i++)
    cout << " " << T[i] ;
  cout << endl;
  */

  int r=0;
  for(int i=1; i<=k; i++){
    if(T[i-1]+1 <= T[i] -1){
      for(int j=T[i-1]+1; j <= T[i]-1; j++)
         r=r+ binomTable[n-j][k-i];

    } // end if


  } 
   delete [] T;
    return r;


}



/* unranking algorithm */

 /* Taken from Kreher and Stinson page 43 
    Instead of returning the T-subset I am passing and changing it
    in the algorithm */

void LexUnrank(int & r,int & k,int & n, int *T,unsigned int ** binomTable){

  int x=1;
  for (int i=1; i<=k; i++){
    while(binomTable[n-x][k-i] <=r){
      r=r-binomTable[n-x][k-i];
      x=x+1;
    }
    /* subtract 1 from x in this line because we are using 0 based
       numbers and the algorithm uses 1 based numbers */
    T[i-1]=x-1;
    x=x+1;

  }

}


/* successor algorithm */
void LexSuccessor(int *T, int n, int & k){

 /* Taken from Kreher and Stinson page 43 
    Instead of returning the T-subset I am passing and changing it
    in the algorithm 

     T is the current vector and U is the succesor*/


  // first set U to be T 
  /*  note that our calculations assume we are using a zero based numbering system
      so we have subtracted a 1 in the while loop test */

 

  int i=k;
  int *U;
  U=new int[k+1];
  for(int i=0; i< k; i++)
    U[i+1]=T[i]+1;


  while( (i>=1) &&  ( U[i]== (n-k+i) ) ) {
    i--;
  }
  if(i!=0){
    for(int j=i; j<=k; j++){
      U[j]=T[i-1]+2+j-i;  // needed to add 2 instead of 1 due to zero based
    }
  }
    else { //undefined
      for(int i=1; i<=k; i++)
          U[i]=-1;

    }
  // now put back
  for(int i=0; i< k; i++)
    T[i]=U[i+1]-1;

  delete []U;
}













