/*****************************************
   Myra B. Cohen
   Dec 2001
   This is a ranking program to create
   a ranking in lexocographic order for all t-subsets

******************************************/

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include "cover.h"
#include "used.h"
#include "util.h"
#include "tiebreaker.h"
#include <sys/time.h>   // needed for timing
#include <sys/resource.h>   // needed for timing
#include <vector>
#include <set>
//test
long satNum=0;
long unsatNum=0;
long totalSatCallNum=0;
float totalSatCPUTime=0.0; //second
float totalSatCPUTime_usec=0.0; //millionsecond

long R_SAT_Call_Num;//total sat call number

/*all time are in millionSecond*/
float R_SAT_Time; //total sat time second part
float R_SAT_Time_usec; // million second part
float R_SAT_Cons_Time; // construction time
float R_SAT_Cons_Time_usec; // million second part
float R_SAT_Relv_Time;//resolve time 
float R_SAT_Relv_Time_usec; // million second part

float R_SAT_Cons1_Time; // construction time
float R_SAT_Cons1_Time_usec; // million second part
float R_SAT_Cons2_Time; // construction time
float R_SAT_Cons2_Time_usec; // million second part
float R_SAT_Cons3_Time; // construction time
float R_SAT_Cons3_Time_usec; // million second part

float R_SAT_Cons4_Time,R_SAT_Cons4_Time_usec;


float R_SAT_Relv1_Time,R_SAT_Relv1_Time_usec;
float R_SAT_Relv2_Time,R_SAT_Relv2_Time_usec;
float R_SAT_Relv3_Time,R_SAT_Relv3_Time_usec;


float Sum_Sat_Const=0.0;
float Sum_Sat_Resol=0.0;

float Sum_Sat_Const_1=0.0;
float Sum_Sat_Const_2=0.0;
float Sum_Sat_Const_3=0.0;
float Sum_Sat_Const_4=0.0;

float Sum_Sat_Resol_1=0.0;
float Sum_Sat_Resol_2=0.0;
float Sum_Sat_Resol_3=0.0;


vector<int> remaining;
int **main_kvcombo;

bitA *Covered;
unsigned int **bTable;
int k;
int t;
int v;
char density_range[50];
//char usets_filename[50];
int *tset;
int M=50; // default is M but can be changed by a program switch
int randSeed;
bool zeropairs=false;
void usage();
void setOptions(long margc, char **margv);
void printStats(std::ofstream & filename );

// flags
bool SEEDED = false ; // whether or not to use randoom seed

int REPEAT= 50; // times to test
int *can;

CA_SMT* smt=NULL;

int checkAllVCount=0;

//used to keep the updated Covered array and TCount

int  mainTCount=0; // cover.Tcount 
bitA *mainCovered; // cover.Covered;
int mainCoveredSize;// cover.CoveredSize;

/*****************************************************************************/
/* the following holds stats for this program */
void printStats(std::ofstream & filename ){

  // uses the can of size REPEAT 

  double max=0.0;
  double min=99999.0;
  double sum=0;
  double avg= 0.0;


  for(int i=0; i< REPEAT; i++){
    if(can[i]> max)
      max=can[i];
    if(can[i]< min)
      min=can[i];
      sum=sum+can[i];

       }

  avg = (sum / REPEAT);

 filename  << "Stats for this covering Array\n";
 filename << "Max: " <<max << " Min: " << min << " Avg: " << avg << std::endl;
}

/*************************************************************************************/




/************************************************************************/

void setRandSeed(int val){
  randSeed=val;
  SEEDED=true;
  cout << "Changed randSeed to " << randSeed << endl;
}


/************************************************************************/

void setM(int val){
  M=val;
  cout << "Changed M to " << M << endl;
}

/************************************************************************/


void setRepeat(int val){
  REPEAT=val;
  cout << "Changed REPEAT to " << REPEAT << endl;
}

/****************************************************************************/

void setDensityRange(char *suffix){
  strcpy(density_range,suffix);
  cout << "Changed density_range to " << density_range << endl;
}

/****************************************************************************/


void Add(int *TC, int** TS, int & count, const Cover & acover, int test, Used & least){

  // need to update covered/uncovered and tcount.
  int sym;
  int combo;
  int rankno;
  int *uset;
  uset = new int[t];

  for(int i=0; i< k; i++){
    sym=TC[i];
   

    if(i<t) { // want to put the first t elments in the temp array
      uset[i]=sym;
      tset[i]=i; // the column combination
    }
  }


  // update cover and uncovered
  combo=bTable[k][t];

 // cout<<"************************test case updated BEGIN***************************"<<endl;

  rankno=LexRank(uset,v,t,bTable);
  
  if(Covered[rankno].on==0){
    Covered[rankno].on=1;
    count--;
    // update least used if this is a new t-set
    for(int i=0; i < t; i++) {
     least.updateUsed(uset[i]);  // update used 
	 
	}
  }

  for(int i=1; i< combo; i++){
    LexSuccessor(tset,k,t);
    // now translate to symbols
    for(int j=0; j< t; j++){
      sym=TC[tset[j]];
      uset[j]=sym;
    }
    rankno=LexRank(uset,v,t,bTable);
    if(Covered[rankno].on==0){
       Covered[rankno].on=1;
      count--;
    for(int i=0; i< t; i++) {
     least.updateUsed(uset[i]);  // update used 
	 
	}
   }
  }

 // cout<<"-----------------------test case updated over-----------------------"<<endl;
  // now update TS
  for(int i=0; i< k; i++) {
	  TS[test][i]=TC[i];
  }

  delete [] uset;

}

/****************************************************************************/
void Add_vector(vector <int> &TC, int** TS, int & count, const Cover & acover, int test, Used & least){

  // need to update covered/uncovered and tcount.
  int sym;
  int combo;
  int rankno;
  int *uset;
  uset = new int[t];


  for(int i=0; i< k; i++){
    sym=TC.at(i);
   

    if(i<t) { // want to put the first t elments in the temp array
      uset[i]=sym;
      tset[i]=i;
    }

  }



  // update cover and uncovered
  combo=bTable[k][t];


  rankno=LexRank(uset,v,t,bTable);
  if(Covered[rankno].on==0){
    Covered[rankno].on=1;
    count--;
    // update least used if this is a new t-set
    for(int i=0; i< t; i++)
     least.updateUsed(uset[i]);  // update used 
  }



  for(int i=1; i< combo; i++){
    LexSuccessor(tset,k,t);
    // now translate to symbols
    for(int j=0; j< t; j++){
      sym=TC.at(tset[j]);
      uset[j]=sym;
    }
    rankno=LexRank(uset,v,t,bTable);
    if(Covered[rankno].on==0){
       Covered[rankno].on=1;
      count--;
    for(int i=0; i< t; i++)
     least.updateUsed(uset[i]);  // update used 
    }
  }

  // now update TS
  for(int i=0; i< k; i++)
    TS[test][i]=TC.at(i);

  delete [] uset;
}

void usage()
{
  cerr << "\tUsage:  maetg inputFile constFile [options]\n";
  cerr << "\t-s n     --set seed value for rand num generator to n \n";
  cerr << "\t-m n     --change the value of M that the program uses \n";
  cerr << "\t-r n     --change the number of repeats that this program uses \n";


}

/********************************************************************/
void setOptions(long margc, char **margv)

{
    margc-=3; 
    margv+=3;  
    while (margc && margv[0][0] == '-') {
	switch (margv[0][1]) {
		case 's':  // change the random seed to be this fixed value
			setRandSeed(atoi(margv[1]));
		    margc--; 
			margv++; 
			break;
		case 'r':  // change the repeat value 
			setRepeat(atoi(margv[1]));
			margc--; 
			margv++; 
			break;
		case 'm':  // change the repeat value 
			setM(atoi(margv[1]));
			margc--; 
			margv++; 
			break;
		case 'd':  // change density_range
			setDensityRange(margv[1]);
			margc--; 
			margv++; 
			break;
   	}
	margv++;
	margc--;     

    }

}

void fillRemain(const Cover &acover, int j, int * pi){

	// if there is no factor related to constraints, then fill all values to remain.
	remaining.clear();
	int v=acover.getVsize(pi[j]);
	for(int i=0;i<v;i++){
		remaining.push_back(i);
	}
}


int checkV(int *TC,int *pi,int j,Cover & acover,int i){
	
	
	/**
	*  Objective: check if the partial test case + value for column j is consistent or not 
	*  input: TC: test case
	          pi: column order
			  j: current computed column
			  i: current computed index
			  acover: Cover object including k,j,v, binomTable, kvcombo,varray,karray
	* 
	*
	**/

		int col=pi[j];

	
		vector<colVal> partialTC;
		vector<int> smt_tc;
    	for (int ii=0;ii<j;ii++){
      		colVal c_colval; //c_colval=corresponding col and val
     		c_colval.col=pi[ii];
			c_colval.val=TC[pi[ii]];
			c_colval.val=acover.getIndex(c_colval.col,c_colval.val); //get index
			partialTC.push_back(c_colval);
			smt_tc.push_back(c_colval.val);
		}

	// then add jth column and vlaue to partialTC
		colVal c_colval; //c_colval=corresponding col and val
     	c_colval.col=col;
		c_colval.val=acover.getSym(col,i);
		c_colval.val=i;
	   	
		partialTC.push_back(c_colval);
		smt_tc.push_back(c_colval.val);
		
		struct rusage before,after;
		
		getrusage(RUSAGE_SELF, &before); 
		
		smt->addScope();
		smt->add_constraints(smt_tc);
		int satOrnot = smt->checkSAT(false);
		smt->clearScope();
		getrusage(RUSAGE_SELF, &after); 

		totalSatCPUTime=totalSatCPUTime+((float)after.ru_utime.tv_sec - (float)before.ru_utime.tv_sec);
		totalSatCPUTime_usec=totalSatCPUTime_usec + ( (float)after.ru_utime.tv_usec - (float) before.ru_utime.tv_usec);

		R_SAT_Time=R_SAT_Time+((float)after.ru_utime.tv_sec - (float)before.ru_utime.tv_sec);
		R_SAT_Time_usec=R_SAT_Time_usec+ ( (float)after.ru_utime.tv_usec - (float) before.ru_utime.tv_usec);


		totalSatCallNum++;
		R_SAT_Call_Num++;


		if(satOrnot){ //sat
                 satNum++;
			return 1;
		}else{ //unsat
			 unsatNum++;
                return 0;
		}
}


/*************************************************************************/



/*************************************************************************/

int BestSymbol(int *TC,int * pi,int j, Cover & acover,const Used &use){

//
	
	
  // this needs to decide what the best symbol is based on where we are
  
  // now need to know what the size of this col symbol set is



  int col=pi[j];
  int pcol;
  int size= acover.getVsize(col);
  //int size= remaining.size();
  int tcount=bTable[j+1][t];
  int *jcount;
  int rank;
  int best;  
  int *pset;
  int *rset;
  int *permorder;
  int found;
  int scol;


  int pickBest=0;
  
  jcount = new int[size];
  pset=new int[j+1];  // use this for ordering symbols
  permorder=new int[k];  // use this to map the permutation in order
  rset=new int[t];

  if(j >= t-1){

	  //update first	
	  // limitation: remaining element is the sequence of values, for example [0,1]

	  for(int i=0; i< size; i++){
			jcount[i]=0;
	   }
	 //update again basing on remaining set 


  for(int i=0 ; i< k; i++){
    permorder[pi[i]]=i;
   }
    
         

 // need to think about how this is done!
  // we only care about which symbol causes most new pairs so keep track of when a new symbol 
  // is involved.. don't need the other counts

 // first tset set up
  
  int val=0;      
  for(int i=0; i< k; i++) {
    // figure out where pcol is
    if(permorder[i]==j)
      pcol=val;
    if(permorder[i]<=j){
      pset[val]=TC[i];
      val++;
    }
  }


    // now set up tset

 found=0; 
    for(int i=0; i< t; i++){
      tset[i]=pset[i]; // used for ranking
      rset[i]=i;  // used for combos
      if(pcol==i){
        found=1;
        scol=i;
      }
    }

    if(found){

      for(int i=0; i< size; i++){
          tset[scol]=acover.getSym(col,i);        
          rank=LexRank(tset,v,t,bTable);
         if(Covered[rank].on==0)  {
	   jcount[i]=jcount[i]+1;
         }
      }
    }


    // now do the rest

        for(int i=1; i< tcount; i++){
          found=0;
          LexSuccessor(rset,j+1,t);
          for(int p=0; p<t; p++){
	    tset[p]=pset[rset[p]];
            if(pcol==rset[p]){
              found=1;
              scol=p;
	    }
	  }
	    
	  if(found){
			
           for(int p=0; p< size; p++){
              tset[scol]=acover.getSym(col,p);   
               rank=LexRank(tset,v,t,bTable);
                if(Covered[rank].on==0)  {
			jcount[p]=jcount[p]+1;
		}
	    }
	  }
	}

	
      
	 // now figure out which is the best symbol
		
		while(true){
				
			
				tieBreaker tie(size);
				best=jcount[0];
				int bestcol=0;
				tie.addNum(0);
				for(int i=1; i< size; i++){
					if(jcount[i]==best ){ 
					// add one more heuristic here  ********* only add if it is ***least used
			        	
						tie.addNum(i);

					}else if(jcount[i]>best){
						tie.reWind();
						tie.addNum(i);
						bestcol=i;
						best=jcount[i];
					}
				}// now choose best
			
				best=tie.chooseRand();
				
				// Now check if such best is a good candidate according to Constraints using isSat()
				int sat=0;
				
				sat=checkV(TC,pi,j,acover,best);
				if(sat==0){
					jcount[best]=-1;
	
				}else{
					//cout<<pickBest<<endl;
					break; //return this best
				}

				if(pickBest>999){ //something wrong here 
				  std::cout<<"pick best is in trouble!"<<std::endl;
					exit(1);
				}
				pickBest++;
		}



        }  //end  j>= t

  else{  // just randomly choose symbol from least used

      tieBreaker tie(size);
    int counter;
    int bestcount=0;


    for(int i=0; i< size; i++){
              counter=use.getRow(acover.getSym(col,i));
                // want largest number here -- means used least
                       if (counter==bestcount && checkV(TC,pi,j,acover,i))
                               tie.addNum(i);
                                     else if(counter > bestcount && checkV(TC,pi,j,acover,i)){
                                             tie.reWind();
                                                     tie.addNum(i);
                                                             bestcount=counter;
                                                                  }
               
                                                                       }
                
                                                                           best=tie.chooseRand();
                                                                            }
                                                                                  

  delete [] pset;
  delete [] rset;
  delete [] permorder;
  delete [] jcount;


  return acover.getSym(col,best);           
}


/***************************************************************************************/


void PermuteAetg(int *pi, int &sym, const Cover & acover, const Used & LeastUsed){

	//objective: 
	// 1. find the first symbol of least used factor in first column
	// 2. then replace such factor with the value in its' orginal place
    // 3. random the remaining columns

  int choice;
  int count=k;
  int temp=0;
  int col=0;
 
  // first randomly pick first elements from leastused
  
  sym=LeastUsed.pickLeastUsed();

  col=acover.getCol(sym);

  if(pi[0]!=col){
    temp=pi[0];  // add to temp
    pi[0]=col;   // change pi[0] to column
   

  // now find that symbol

   int fi=1;



   while(pi[fi]!=pi[0]){ 
    assert(fi<k); // find where that symbol was
    fi++;
   }
  pi[fi]=temp;
  }
  // now fill the rest
 
  for(int i=k-1; i>=1; i--){
	
     choice=(rand()%k);
     if(choice !=0 && pi[choice]){
     temp=pi[i];
     pi[i]=pi[choice];
     pi[choice]=temp;
  }
  }
}

void AddBest(int **TCM, int **TS,int & count, Cover & acover, int tests,Used & least){

  int best=0;
  int bestcnt=0;
  tieBreaker tie(M);
  int rank;
  int tcount;
  int tsetcount;
  int *uset;


  uset = new int[t];

  // first choose the best test
   
  tcount=bTable[k][t];


  for(int i=0; i< M; i++){
  tsetcount=0;
    // count tests

    // begin with first one
    for(int j=0; j<t; j++){
      uset[j]=TCM[i][j];
      tset[j]=j;
    }

    rank=LexRank(uset,v,t,bTable);
    if(Covered[rank].on==0){
      tsetcount++;
    }
    // now do the rest
    for(int p=1;p<tcount; p++){
       LexSuccessor(tset,k,t);
       // now translate
       for(int j=0; j< t; j++)
         uset[j]=TCM[i][tset[j]]; 
       rank=LexRank(uset,v,t,bTable);
       if(Covered[rank].on==0){
          tsetcount++;
       }
    }

  
   if( tsetcount==bestcnt)
      tie.addNum(i);
    else if(tsetcount> bestcnt){
      tie.reWind();
      tie.addNum(i);
      best=i;
      bestcnt=tsetcount;
    }

  }
    
     
    
  // then add it
  if(bestcnt==0){
    zeropairs=true;
    std::cout << "no pairs " << std::endl;
  }
  else
     Add(TCM[best], TS,count,acover,tests, least);


     delete [] uset;
 

}
  

int main(int argc, char **argv) {


  /* input file has the following format:
t
k
v  number of occurences
v  number of occurences
..
0  - end of input

  ******************************/

  
 char fname[30];
 char cname[30]; // constraints file name
 density_range[0]='\0';
 std::ofstream ofile;
 std::ofstream logfile;


  //Make sure that we have correct number of args

  if(argc <3){
    usage();
    exit(1);
  } 
 
 std::cout << "setting options " << std::endl;
 setOptions(argc, argv); 
 std::cout << "ending setting of options " << std::endl;
 strcpy(fname,argv[1]);
 strcpy(cname,argv[2]);

/**********************************************************************************************
    algorithm starts below */


/* create a cover so we know what we need to do for the algorithm */
 can=new int[REPEAT];
 for(int i=0; i< REPEAT; i++)
   can[i]=0;

 if(!SEEDED){
    randSeed=(int (time(NULL))); // reseed random #   
 }
 srand(randSeed);

 struct rusage before,after;
 float CPU, CPUsum;
 CPUsum=0.0;
  
  CPU=0.0;
  getrusage(RUSAGE_SELF, &before); 

  Cover aetgcover(fname,cname, density_range); 

  std::cout << " aetg cover " << aetgcover;
  int TsetCount=aetgcover.getTcount();
  int TCount=TsetCount;   
  
  // inialize the values t,v,k
  t=aetgcover.getT();
  v=aetgcover.getV();
  k=aetgcover.getK();
  
  logfile.open("../../Results/CoveringArrays/aetglog_z"+to_string(k)+"_p"+to_string(v/k)+"_t"+to_string(t)+density_range+".out");
  logfile << aetgcover << std::endl;
  
   mainTCount=TsetCount; // cover.Tcount 

  int *TC;        
  int **TCM;  // holds M TC's
   
  
  
  int **TS;  // holds the final Test Suite
  int TSize=0;  // used to count how many tests in test suite

  //setup CA_SMT solver and add the distance constraints
  smt = new CA_SMT(k, v/k, density_range);
  smt->setup_distance_constraints();
  

  bTable=aetgcover.getTableRef();  // assign a reference pointer to the binomial table
  int size=bTable[v][t];  // find out v choose t

  /*********Covered*********/
  mainCoveredSize=aetgcover.CoveredSize;// cover.CoveredSize;
  bitA *mainCovered; // cover.Covered;
  
  //create
  mainCovered=new bitA[mainCoveredSize]; 
  Covered=new bitA[mainCoveredSize];

  for(int i=0;i<mainCoveredSize;i++){ //copy from aetgcover.Covered
	   mainCovered[i].on=aetgcover.Covered[i].on;	
  }
  /*********Covered*************/
  

  TC = new int[k];
  TCM= new int*[M];
  tset= new int[t];
  TS= new int*[v*v];
  TSize=v*v;

  for(int i=0; i< M; i++) {
    TCM[i]=new int[k];
    for(int j=0; j< k; j++)
      TCM[i][j]=0;

  }
 for(int i=0; i< TSize; i++){
    TS[i]=new int[k];
    for(int j=0; j<k;j++)
      TS[i][j]=0;
  }

   int *pi; // used to record the factors order in the permutation of remainging factors in PermuteAetg()
   pi=new int[k];

  
   int firstSym; // record the first symbol of best factor in PermuteAetg()

   /*   aetg algorithm proper  */
   int tests=0;  //contains final test set size
   int Gbest=999999;
   int zeroP=0;

   Used LeastUsed(aetgcover );   /*make an object for least used.
                                 It uses the current cover to 
                                 set up its parameters */
	

// So far above is the Overhead of considering constraints

   getrusage(RUSAGE_SELF, &after); 

   CPU=(after.ru_utime.tv_sec + (float)after.ru_utime.tv_usec/1000000.0) - (before.ru_utime.tv_sec + (float) before.ru_utime.tv_usec/1000000.0);
	CPUsum=CPUsum+CPU;
	logfile << "The overhead Time in Seconds " << CPU << std::endl;

	logfile << "*******************************" <<std::endl;

  

for(int R=0; R< REPEAT; R++){
	
  std::cout<<"Repeat "<<R<<std::endl;
    CPU=0.0; //Repeat total time


	
R_SAT_Time=0.0;
R_SAT_Time_usec=0.0;
R_SAT_Cons_Time=0.0;
R_SAT_Cons_Time_usec=0.0;
R_SAT_Relv_Time=0.0; 
R_SAT_Relv_Time_usec=0.0;

R_SAT_Cons1_Time=0.0; // construction time
R_SAT_Cons1_Time_usec=0.0; // million second part
R_SAT_Cons2_Time=0.0; // construction time
R_SAT_Cons2_Time_usec=0.0; // million second part
R_SAT_Cons3_Time=0.0; // construction time
R_SAT_Cons3_Time_usec=0.0; // million second part

R_SAT_Cons4_Time=0.0;
R_SAT_Cons4_Time_usec=0.0;
R_SAT_Relv1_Time=0.0;
R_SAT_Relv1_Time_usec=0.0;
R_SAT_Relv2_Time=0.0;
R_SAT_Relv2_Time_usec=0.0;
R_SAT_Relv3_Time=0.0;
R_SAT_Relv3_Time_usec=0.0;


    getrusage(RUSAGE_SELF, &before);  
    tests=0;//size of TS of this Repeat
    zeroP=0;
	checkAllVCount=0;
	TCount=mainTCount; 
	
	//Covered=aetgcover.getCoveredRef();
	for(int i=0;i<mainCoveredSize;i++){ //copy from aetgcover.Covered
	    Covered[i].on=mainCovered[i].on;
	}

	aetgcover.clear(); //refresh kvcombo and kcombo
	
	LeastUsed.Clear2(aetgcover); //use refreshed kvcombo to refresh the LeastUsed arrays

	// initialize those containers too

	for(int i=0; i< k; i++)
		pi[i]=i;

	for(int i=0; i< k; i++)
		TC[i]=0;

	 
	for(int i=0; i< M; i++) 
		for(int j=0; j< k; j++)
		TCM[i][j]=0;
	for(int i=0; i< TSize; i++){
		for(int j=0; j<k;j++)
		TS[i][j]=0;
	}


  while(TCount > 0){
	
     for(int i=0; i< M; i++){
		
		for(int pii=0; pii< k; pii++)
			pi[pii]=pii;


		PermuteAetg(pi,firstSym,aetgcover,LeastUsed);
		

		TCM[i][pi[0]]=firstSym;
		// fill in the remaining factors
		for (int j=1; j< k; j++){
			TCM[i][pi[j]]=BestSymbol(TCM[i],pi,j,aetgcover,LeastUsed);
		}
   }

	// cout<<"-------------------One Best Testcase out 50-----------------"<<endl;	

	//reallocate memory for TS when the number of tests exceeds the initially allocated memory size
	if (tests == TSize) {
		int** temp = new int*[TSize];
		for(int i=0; i< TSize; i++) {
			temp[i]=new int[k];
			for(int j=0; j<k;j++)
				temp[i][j]=TS[i][j];
		}
		for(int i=0; i< TSize; i++)
			delete [] TS[i];
		delete [] TS;
		
		int old_TSize = TSize;
		TSize += v*v;
		
		TS = new int*[TSize];
		for(int i=0; i< old_TSize; i++) {
			TS[i]=new int[k];
			for(int j=0; j<k;j++)
				TS[i][j]=temp[i][j];
			delete [] temp[i];
		}
		delete[] temp;
		
		for(int i=old_TSize; i< TSize; i++) {
			TS[i]=new int[k];
			for(int j=0; j<k;j++)
				TS[i][j]=0;
		}	
	}
    AddBest(TCM, TS,TCount,aetgcover,tests, LeastUsed);	//so far, TCM will always include good test cases 


     if(zeropairs)  // 0 new pairs,global variable 
	 {   
	     zeroP++;
	     zeropairs=false;
         tests--;
	 }

    tests++;
	if(zeroP==100){
		// print out the file so far:
		ofile.open("Failedaetg_z"+to_string(k)+"_p"+to_string(v/k)+"_t"+to_string(t)+".out");
		for(int i=0; i< tests ; i++){
			ofile << TS[i][0]%(v/k);
			for(int j=1; j< k ; j++){			        
				ofile << " " << TS[i][j]%(v/k);
			}
			ofile << std::endl;
		}
		ofile.close();
			
		int c;
		std::cout << "failed covering array " << std::endl;
		std::cout << "Tcount " <<  TCount << " Tests " << tests << std::endl;
		tests=Gbest+1;  // make sure we discount this one 
		TCount=0;
	}

	if((TCount%10)==0){
	  //  cout << "TCount Remaining " << TCount << " RowCount " << tests << endl;
	}
   }

   can[R]=tests;    // one Repeated TS is finished, and record it can[R]

   getrusage(RUSAGE_SELF, &after); 

   CPU=(after.ru_utime.tv_sec + (float)after.ru_utime.tv_usec/1000000.0) - (before.ru_utime.tv_sec + (float) before.ru_utime.tv_usec/1000000.0);
   //cout<<R<<" time "<<CPU<<endl;
   CPUsum=CPUsum+CPU; 
   //cout<<"total time " <<CPUsum<<endl;

    if(tests < Gbest){ //reord the best TS by comparing the tests number, CA size
			Gbest=tests;
			   
				// copy to best ts if best
			   

			ofile.open("../../Results/CoveringArrays/aetg_z"+to_string(k)+"_p"+to_string(v/k)+"_t"+to_string(t)+density_range+".out");


			std::ifstream oxfile("temp.out",ios::in);

			int x;

			
			for(int i=0; i< Gbest ; i++){
				ofile << TS[i][0]%(v/k);
				for(int j=1; j< k ; j++){			        
					ofile << " " << TS[i][j]%(v/k);	
				}
				ofile << std::endl;
			}
			ofile.close();
	}


   float R_sattime=0.0;

   R_sattime=R_SAT_Time+R_SAT_Time_usec/1000000.0;

   float R_constructTime=0.0;

   R_constructTime=R_SAT_Cons_Time+R_SAT_Cons_Time_usec/1000000.0;


/****************************************/
   float R_construct_1_Time=0.0; //initMng=newMng+addComputedConstraints

   R_construct_1_Time=R_SAT_Cons1_Time+R_SAT_Cons1_Time_usec/1000000.0;

/****************************************/
   float R_construct_2_Time=0.0;//AddTestCaseConstraints

   R_construct_2_Time=R_SAT_Cons2_Time+R_SAT_Cons2_Time_usec/1000000.0;

/****************************************/   
   float R_construct_3_Time=0.0;//0

   R_construct_3_Time=R_SAT_Cons3_Time+R_SAT_Cons3_Time_usec/1000000.0;
   
/****************************************/

   float R_construct_4_Time=0.0;//0

   R_construct_4_Time=R_SAT_Cons4_Time+R_SAT_Cons4_Time_usec/1000000.0;

  
   float R_sovlingTime=0.0; //Call SAT_Solver to solver above computed formula

   R_sovlingTime=R_SAT_Relv_Time+R_SAT_Relv_Time_usec/1000000.0; 

/****************************************/
	 float R_sovling_1_Time=0.0;//0
	 R_sovling_1_Time=R_SAT_Relv1_Time+R_SAT_Relv1_Time_usec/1000000.0;

/****************************************/
	float R_sovling_2_Time=0.0;//0

/****************************************/
	float R_sovling_3_Time=0.0;//0


   Sum_Sat_Const=Sum_Sat_Const+R_constructTime;//construct=initMng+AddTestCaseConstraints

   Sum_Sat_Resol=Sum_Sat_Resol+R_sovlingTime; //resolve=resolve time 

	/****************************************/
    Sum_Sat_Const_1=Sum_Sat_Const_1+R_construct_1_Time;//initMng=newMng+addComputedConstraints

	/****************************************/
    Sum_Sat_Const_2=Sum_Sat_Const_2+R_construct_2_Time;//AddTestCaseConstraints

	/****************************************/
    Sum_Sat_Const_3=Sum_Sat_Const_3+R_construct_3_Time;//0

	/****************************************/
    Sum_Sat_Const_4=Sum_Sat_Const_4+R_construct_4_Time;//0


	
/****************************************/
Sum_Sat_Resol_1=Sum_Sat_Resol_1+R_sovling_1_Time;//0

 logfile << "*************************** " << std::endl;

   //For each Repeat, Print the log info 

 logfile << "Repeat No. = " << R << std::endl; 
 logfile << "The best TS: " << tests << std::endl;
 logfile << "Total Time " << CPU << std::endl;
 logfile << "Total SAT Call # " << R_SAT_Call_Num << std::endl;
 logfile << "sat Call # " << satNum<<std::endl;
 logfile << "unsat Call #  " << unsatNum<<std::endl;
   
 logfile << "SAT Time in Seconds " << R_sattime << std::endl;  // SAT time= SAT construction time + SAT Resolving time

 logfile << "SAT Construction Time " << R_constructTime<< std::endl;
 logfile << "SAT Construction MngCommonCons  " << R_construct_1_Time<< std::endl;
 logfile << "SAT Construction MngTestCaseCons  " << R_construct_2_Time<< std::endl;
		
 logfile << "SAT Resolving Time " << R_sovlingTime << std::endl;


  }  // repeats

	//after repeats, compute the SAT call time
   float sattime=0.0;

   sattime=totalSatCPUTime+totalSatCPUTime_usec/1000000.0;
   
  
  //log for all Repeat
   logfile << "*************************** " << std::endl;

   //All repeated Number of TS has been produced, and best TS has been recorded
   //print out all associated information 

   logfile << "Best TS: " << Gbest << std::endl;
   logfile << "Size of M  =  " << M << std::endl;
   logfile << "Number of Runs = " << REPEAT << std::endl; 
   logfile << "Random Seed = " << randSeed << std::endl; 
   logfile << "Total Time in Seconds " << CPUsum << std::endl;

   logfile << "Total SAT Calls  " << totalSatCallNum<<std::endl;
   logfile << "sat Call # " << satNum<<std::endl;
   logfile << "unsat Call #  " << unsatNum<<std::endl;
         
   logfile << "Total SAT Time " << sattime <<std::endl;

   logfile << "SAT Constuct  " << Sum_Sat_Const <<std::endl;
	
   logfile << "SAT Construction MngCommonCons  " << Sum_Sat_Const_1<< std::endl;
   logfile << "SAT Construction MngTestCaseCons  " << Sum_Sat_Const_2<< std::endl;
	
   logfile << "SAT Resolving  " << Sum_Sat_Resol <<std::endl;


   printStats(logfile);
   logfile.close();
    
  
  // delete memory at end 
  delete [] Covered;
  for (int i=0; i< M; i++)
    delete [] TCM[i];
  for(int i=0; i< TSize; i++)
    delete [] TS[i];
  delete [] TS;
  delete [] TCM;
  delete [] TC;
  delete [] tset; 
  delete [] can;
  delete smt;
  return 0;

}

