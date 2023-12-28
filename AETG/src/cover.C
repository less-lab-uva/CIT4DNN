/*****************************************
   This is a class to hold a covering array
   It holds k,t,v parameters.
   It has a karray which tells you what v1,v2.. etc are
   It has a varray to map the symbols to values
  
   It maintains a count of the total combinations of t-sets

******************************************/
#include<cstring>
#include "cover.h"


/*****************************************************************/

/* main constructor to create from a file 

  input file has the following format:


t
k
v  number of occurences
v  number of occurences
..
0  - end of input

 *************************************************************/

void Cover::columnCombComputing(int kk, int tt,vector<int> vec)
{  
	vector <int > localvec;
	for(int i=kk;i>=tt;i--)
	{
		localvec=vec;
		localvec.insert(localvec.begin(),i-1);
		
		if(tt>1) 
		{ 
			columnCombComputing(i-1,tt-1,localvec);}//There are other combinations, iterative
		else
		{   //there is one combination
			
			columnCombSet.insert(localvec);
		}
	}

} 

void Cover::createConcreteValueCombSet_CoveredUpdate()
{
	for(set<vector <int> >::iterator p = columnCombSet.begin(); p!= columnCombSet.end(); p++){

		vector<int> oneColumCombo=*p; // for each columnCombo

		vector<int> emptyValueCombo;
		int startFactorIndex=0;
    // remove factor checking because some constrains which exclude all possible combinations
     //	if(cover_satForOthers->isInvolvedFactor(oneColumCombo)){//factor invovled 
			createConcreteValueCombo(startFactorIndex,oneColumCombo,emptyValueCombo);
	//	}
	
	}

}

void Cover::createConcreteValueCombo(int factorIndex, vector<int> currCombo, vector<int> valueComboVec)
{
	
	vector<int> localvec;

	//get the current factor
	int curFactor=currCombo.at(factorIndex);
	//get the current value size of this factor
	int curFactorSize=getVsize(curFactor);
	
	//get the currCombo size 
	int comboSize=currCombo.size();

	for(int j=0;j<curFactorSize;j++) 
	{
		localvec=valueComboVec;
		//get the current concrete value of this factor curFactor with this index j
		int curSym=getSym(curFactor,j);

		localvec.insert(localvec.end(),curSym);

		if(factorIndex<(comboSize-1)) 
		{ 
			createConcreteValueCombo(factorIndex+1,currCombo,localvec);//There are other combinations, iterative
		}else { //there is one general value combination value
			//concreteValueCombSet.push_back(localvec);
			//updated covered array 
			
				CoveredUpdateInLoop(localvec);
			
			

		}
	}
	
}


void Cover::printColumnCombvalueSet(){
	
	cout<<"-------------------------------Column Combinations Set--------------------------------"<<endl;

     		cout<<columnCombSet.size()<<endl;

			for(set<vector <int> >::iterator p = columnCombSet.begin(); p!= columnCombSet.end(); p++){
				vector<int> tmp=*p;
				for(vector<int >::iterator p2 = tmp.begin(); p2!= tmp.end(); p2++){
				cout<<"--------------------------------------------"<<*p2<<":"<<endl;
				}
				cout<<"--------------"<<endl;
			}
}

void Cover::printConcreteValueCombSet(){

	cout<<concreteValueCombSet.size()<<endl;
	cout<<"-------------------------------All Concrete Value Combinations Set--------------------------------"<<endl;

	for(vector <vector <int> >::iterator p = concreteValueCombSet.begin(); p!= concreteValueCombSet.end(); p++)
	{
		vector<int> valueComb=*p; // (0,1,2)

		for(vector <int> ::iterator p2 = valueComb.begin(); p2!= valueComb.end(); p2++){
			int tmp;
			tmp=*p2;
			cout<<"========================================================"<<tmp<<endl;
		}
		cout<<"====="<<endl;
	}
}


Cover::Cover(){
  /* default constructor that doesn't really do anything other than initialize everything to 0 */
  k=1;  // need for destructor
  v=0;
  t=0;
  Tcount=0;
  karray=new int[1];
  kcombo=new int[1];
  varray=new int*[1];
  varray[0]=new int[1];
  kvcombo=new int*[1];
  kvcombo[0]=new int[1];
  cols=new int[1];
}


/* copy constructor */

Cover::Cover(const Cover & cv){

    k=cv.k;
    t=cv.t;
    v=cv.v;
    Tcount=cv.Tcount;

    karray=new int[k];
    for (int i=0; i< k; i++)
      karray[i]=cv.karray[i];
    kcombo=new int[k];
    for (int i=0; i<k; i++)
      kcombo[i]=cv.kcombo[i];
    
    varray=new int*[k];
    cols=new int[v];
   
    for(int i=0; i< k; i++){
      varray[i]= new int[karray[i]];
       for(int j=0; j< karray[i]; j ++){
          varray[i][j]=cv.varray[i][j];
        }
    }

	kvcombo = new int*[k];
	for(int i=0; i< k; i++){
      kvcombo[i]= new int[v];
       for(int j=0; j< v; j ++){
          kvcombo[i][j]=cv.kvcombo[i][j];
        }
    }

    for(int i=0; i< v; i++)
      cols[i]=cv.cols[i];


  binomTable = new unsigned int*[v+1];

  for(int i=0; i<=v; i++){
     binomTable[i]=new unsigned int[t+1];
   }

  for(int i=0; i<=v; i++)
    for(int j=0; j<=t; j++)
      binomTable[i][j]=cv.binomTable[i][j];

  }




 int Cover::getIndex(int field, int value) {
	   int index;
	   for(index=0;index<karray[field];index++)     {
		   if(varray[field][index]==value){
			   return index;
		   }
	   }
	   cout<<"no value:"<<value<<"in factor:"<<field<<endl;
	   exit(0);
   }	  



/* assignment operator */

Cover & Cover::operator=(const Cover & cv){

  // first check to see if you are assinging to oneself
  // then delete all memory
 if(this != & cv){
    delete [] karray;
    delete [] kcombo;

 for(int i=0; i< k; i++)
   delete [] varray[i];
 delete [] varray;

 for(int i=0; i< k; i++)
   delete [] kvcombo[i];
 delete [] kvcombo;

 for(int i=0; i<=v; i++)
   delete [] binomTable[i];
    
 delete [] cols;    
 delete [] binomTable;

    // now create new memory and copy

    k=cv.k;
    t=cv.t;
    v=cv.v;
    Tcount=cv.Tcount;

    karray=new int[k];
    kcombo=new int[k];
    for (int i=0; i< k; i++)
      karray[i]=cv.karray[i];
    
    for(int i=0; i< k; i++)
      kcombo[i]=cv.kcombo[i];
    varray=new int*[k];
    cols= new int[v];
    for(int i=0; i< k; i++){
      varray[i]= new int[karray[i]];
       for(int j=0; j< karray[i]; j ++){
          varray[i][j]=cv.varray[i][j];
       }
    }

	kvcombo = new int*[k];
	for(int i=0; i< k; i++){
      kvcombo[i]= new int[v];
       for(int j=0; j< v; j ++){
          kvcombo[i][j]=cv.kvcombo[i][j];
        }
    }


    for(int i=0; i< v; i++)
      cols[i]=cv.cols[i];
 }


  binomTable = new unsigned int*[v+1];

  for(int i=0; i<=v; i++){
     binomTable[i]=new unsigned int[t+1];
   }

  for(int i=0; i<=v; i++)
    for(int j=0; j<=t; j++)
      binomTable[i][j]=cv.binomTable[i][j];


  return * this;      


}



Cover::Cover(char *name,char *cname, char *d_range){


  char fname[50];
  char conname[50];
  char density_range[50];
  v=0;
  k=0;
  t=0;
  Tcount=0;
  int kV=0;
  int x,y;
  strcpy(fname, name);    
strcpy(conname, cname);  
strcpy(density_range, d_range);
  

  ifstream ifile(fname, ios::in);
  while(!ifile){
     cerr<< "No infile " ;
     cout<< "Enter a valid file name or type 'quit' to quit: " ;
     cin >> fname ;
     if (strcmp(fname,"quit")==0)
		exit(1);
  ifile.open(fname, ios::in);    
  } 
  
  // now start reading params
 
  ifile >> t;
  ifile >> k;
  ifile >> x;
  
  karray=new int[k];  // create memory
  kcombo=new int[k]; // create memory


  while (x!=0 && !ifile.eof() ){
    ifile >> y;
    for(int i=0; i< y; i++){
      karray[kV]=x;
      kV++; 
    }

    // calculate v
    v=v+ (x*y);
    ifile >> x;
  }

    // reality check    
    assert(kV==k);

  // set the size of n

  /* now create the varray */
 cols=new int[v];

  varray=new int*[k];
  int s=0;   
  for(int i=0; i< k; i++){
    varray[i]= new int[karray[i]];
    for(int j=0; j< karray[i]; j ++){
      varray[i][j]=s;
      cols[s]=i;  // give it a column
      s++;
    }
  }

  /* now calculate how many t-sets there will be. (this is based on first finding how many
     combinations of rows there are and then multiplying by using the succesors */

   
  int *uset;
  uset = new int[t]; 
  int prod=1;
  int count=0;

  /*create memory for the binomial table. */
  binomTable = new unsigned int*[v+1];

  for(int i=0; i<=v; i++){
     binomTable[i]=new unsigned int[t+1];
   }
     nurmela_binom(v,t,binomTable);

	


  count = binomTable[k][t]; //how many t-sets/pairs
 
  // we want to make sure that the kcombo array is initialized to 0
   for(int i=0; i< k; i++)
    kcombo[i]=0;



 
  // now multiply and add each one 

  // we know the first combination 
 // unrank the t-tuple and multiply each one together
   
  // get first lexocograhic element
 
  for(int i=0; i< t; i++)
     uset[i]=i;
  
  // now get its prodcut

    for(int a=0; a<t; a++){
      prod=prod*(karray[uset[a]]);
     }
  
    Tcount=Tcount+prod;
    // now count how many t-sets each karray is involved with
    for(int a=0; a<t; a++)
      kcombo[uset[a]]=kcombo[uset[a]]+prod;

  for(int x=1; x< count; x++){
 
	LexSuccessor(uset,k,t);
	prod=1;
	for(int a=0; a<t; a++){
		prod=prod*(karray[uset[a]]);
	}

    Tcount=Tcount+prod;
    for(int a=0; a< t; a++)
      kcombo[uset[a]]=kcombo[uset[a]]+prod;   
  }

    // now divide out by number of symbols

  for(int a=0; a<k; a++)
    kcombo[a]=kcombo[a]/karray[a];
 
    	//added by jiangfan shi
 
	int coveredarraysize=binomTable[v][t]; //Covered array size
	
	CoveredSize=coveredarraysize;

	/*
		all class fields/members must be created outside of initilization functions
		so that, those fields can be used in the whole class scope without Segmentation fault(core dumped).
	*/
	//create
	Covered=new bitA[coveredarraysize]; 
	//create
    kvcombo=new int*[k];
	for(int i=0; i< k; i++)
		kvcombo[i]= new int[v];

	
	//create
    kvcombo_ForUsedClear=new int*[k];
	for(int i=0; i< k; i++)
		kvcombo_ForUsedClear[i]= new int[v];

	Kcombo_ForUsedClear=new int[k]; // create memory
	for (int i=0;i<k;i++)
			Kcombo_ForUsedClear[i]=0;


    //Tcount = (k choose t)* v'^t, and it is real number of t-sets
    //coveredarraysize = v choose t and it is covered array size

	Tcount_BeforeCoveredUpdating=Tcount;
	cover_smt = new CA_SMT(k, v/k, density_range);
	cover_smt->setup_distance_constraints();
	
	//initialize it as 0

	for(int i=0; i<coveredarraysize ; i++){
		Covered[i].on=0;
	}

  
	//Compute the valid concrete value combination set

	vector<int> emptyForColumnComboVec;
	columnCombComputing(k,t,emptyForColumnComboVec);
	createConcreteValueCombSet_CoveredUpdate();  

	KVCombo(kvcombo,binomTable,t);
	
	//copy kvcombo to kvcombo_ForUsedClear[][] 
	for(int i=0; i< k; i++)
		for(int j=0;j<v;j++)
			kvcombo_ForUsedClear[i][j]=kvcombo[i][j];
	  
	// we want to make sure that the kcombo array is initialized to 0
	for (int i=0;i<k;i++)
		kcombo[i]=0;

	updateKcombo(k,v,kvcombo);

	//copy Kcombo to kvcombo_ForUsedClear[][] 
	for(int i=0; i< k; i++)
		Kcombo_ForUsedClear[i]=kcombo[i];


	delete []uset;
  
}


void Cover::clear(){
	
	//copy Kcombo_ForUsedClear[][] to Kcombo
	for(int i=0; i< k; i++)
		kcombo[i]=Kcombo_ForUsedClear[i];

	//copy kvcombo_ForUsedClear[][] to kvcombo 
	for(int i=0; i< k; i++)
		for(int j=0;j<v;j++)
			kvcombo[i][j]=kvcombo_ForUsedClear[i][j];

}

void Cover::updateKcombo(int k, int v, int ** kvcombo){
/*

inputs: 
		k: factor number
		v: all values number
		kvcombo: two dimension array recording value level's t-sets occurences  

outputs:

	created, initialized, updated kcombo[] which is factor levels's t-sets orrences, and used in choosing first column in Aetg permAetg()
	
*/
 // initialized
   for(int i=0; i< k; i++)
		kcombo[i]=0;

   //updated according to kvcombo[][]
   int tmpsum=0;

	for(int a=0; a<k; a++){
		tmpsum=0;
		for(int b=0;b<v;b++){
			tmpsum=tmpsum+kvcombo[a][b];
		}
		kcombo[a]=tmpsum;
	}

}
/**************************************************************************/

/* need to destroy memory */
Cover::~Cover(){ 

// delete the dynamic memory
// delete the karray  


delete [] karray;

for(int i=0; i< k; i++)
   delete [] varray[i];
 delete [] varray;

 delete [] cols;
 delete [] kcombo;
 for(int i=0; i<=v; i++)
   delete [] binomTable[i];
 delete []binomTable;
 
	for(int i=0; i< k; i++)
		 delete [] kvcombo[i];
	delete [] kvcombo;

	if(cover_smt != NULL) {
		delete cover_smt;
	}
}



/*************************************************************************/



ostream & operator<<(ostream & os, const Cover &cv){
 
  os << "t   k   v  TCount  " << endl;
  os << cv.t << " " << cv.k << " " << cv.v << " " <<cv.Tcount << endl;
  os << endl;
  for (int i=0; i< cv.k; i++)
    os << " " << cv.karray[i];
  os << endl;
  os << endl;
  for(int i=0; i< cv.k; i++){
    for(int j=0; j<cv.karray[i]; j++)
      os << "  " << cv.varray[i][j];
    os << endl;
  }

  return os;

}

void Cover::CoveredUpdateInLoop( vector<int> cur_tset){
										

// merge the covered updated function in recursive call

  int *usymbol=new int[t];
  vector <colVal> trans_usymbol; //transform usymbol/t-set/(3,6) to (0,1) mode 
  vector<int> smt_usymbol;
  int oneFactorInvolved; //boolean value
  
  
  vector<int> oneValComb;

	  oneFactorInvolved=0;

	  oneValComb=cur_tset;
	 oneFactorInvolved=1; 
	  if(oneFactorInvolved){// at least there is one factor which appears in the factors of constraints
				trans_usymbol.clear();
				smt_usymbol.clear();
				for(int xy=0;xy<oneValComb.size();xy++)
				{
					usymbol[xy]=oneValComb.at(xy);
				}
			     

				//transfer (0,3) to (0_0,1_0) mode 
				for (int ii=0;ii<t;ii++){
      				colVal c_colval; //c_colval=corresponding col and val
      				c_colval.val=usymbol[ii];
      				c_colval.col=getCol(c_colval.val);
					c_colval.val=getIndex(c_colval.col,c_colval.val); //get index

      				trans_usymbol.push_back(c_colval);
					smt_usymbol.push_back(c_colval.val);
				}
    

						cover_smt->addScope();
						cover_smt->add_constraints(smt_usymbol);
						int satOrnot = cover_smt->checkSAT(false);
						cover_smt->clearScope();

						if(satOrnot==0){ // it violates implicit or explicit constraints

							assert(trans_usymbol.size()==t);
							
							int * tmp_uset=new int[t];
							for(int i=0;i < trans_usymbol.size(); i++)
							{
								colVal tmpStru = trans_usymbol.at(i);
								//construct a uset
								tmp_uset[i]=getSym(tmpStru.col,tmpStru.val); //field,index

							}
							//get index of position of such pair (0,3)
							int rankno= LexRank(tmp_uset,v,t,binomTable);
							delete [] tmp_uset;

							//update the corresponding pairs Covered information

							if(Covered[rankno].on==0){
								Covered[rankno].on=1;
								Tcount--; 
							}
						}

						
				}
					
}

void Cover::CoveredUpdate( unsigned int ** bTable,
										int& TsetCount,int size){
	
/*
inputs:
		bTable:binomTable
		TsetCount: real t-set number, it is used to terminate the Aetg construction
		size: all t-set number, produced by v choose t and including some illegal t-sets from the same columns
output: 
	   initilized and updated Covered bitA array basing on the contraints

*/

//considering constraints to update Covered and produce new data structure KVCombo

// one functions
// 1. update Covered 

/*   // create the pi array
   int *p all pairs one by one and ask if such pair is satisfiable or not?
 1. produce the first uset
 2. ask if it is satisfialbe or not
 3. if not, then lexRank() to get the rank, and Cover[rank].on=1
 4. while(not finished tcount>1) /we have alreaday computed the first one.
 5  lexsuccessor() to get next uset
 6. ask if it is satisfiale or not
 7. if not then lexrank() to get the rank, and Cover[rank].on=1
 8. tcount--
 9. end while
*/

  int *usymbol=new int[t];
  vector <colVal> trans_usymbol; //transform usymbol/t-set/(3,6) to (0,1) mode 
  vector <int> smt_usymbol;
  int tmp_rankno;
  int tmp_count;
  int oneFactorInvolved;
  
  tmp_count=bTable[v][t];
  

  //initialize it as 0

  for(int i=0; i< size; i++){
    Covered[i].on=0;
  }

  
//then check the remaining t-set and update Covered
//cout<<"the total number of valid pairs without considering avoids:" <<TsetCount<<endl;
//for(int x=1; x< tmp_count; x++){
  vector<int> oneValComb;
  for(int x=0;x<concreteValueCombSet.size();x++){
	  oneFactorInvolved=0;
	  oneValComb=concreteValueCombSet.at(x);
		
	  oneFactorInvolved = 1;
	  if(oneFactorInvolved){// at least there is one factor which appears in the factors of constraints
				trans_usymbol.clear();
				smt_usymbol.clear();
				for(int xy=0;xy<oneValComb.size();xy++)
				{
					usymbol[xy]=oneValComb.at(xy);
				}
			     

				//transfer (0,3) to (0_0,1_0) mode 
				for (int ii=0;ii<t;ii++){
      				colVal c_colval; //c_colval=corresponding col and val
      				c_colval.val=usymbol[ii];
      				c_colval.col=getCol(c_colval.val);
					c_colval.val=getIndex(c_colval.col,c_colval.val); //get index

      				trans_usymbol.push_back(c_colval);
					smt_usymbol.push_back(c_colval.val);
				}
			      
						cover_smt->addScope();
						cover_smt->add_constraints(smt_usymbol);
						int satOrnot = cover_smt->checkSAT(false);
						cover_smt->clearScope();

						if(satOrnot==0){ // it violates implicit or explicit constraints

							assert(trans_usymbol.size()==t);
							
							int * tmp_uset=new int[t];
							for(int i=0;i < trans_usymbol.size(); i++)
							{
								colVal tmpStru = trans_usymbol.at(i);
								//construct a uset
								tmp_uset[i]=getSym(tmpStru.col,tmpStru.val); //field,index

							}
							//get index of position of such pair (0,3)
							int rankno= LexRank(tmp_uset,v,t,bTable);
							delete [] tmp_uset;

							//update the corresponding pairs Covered information

							if(Covered[rankno].on==0){
								Covered[rankno].on=1;
								TsetCount--; 
							}
						}
				}
	  
	  }

	  
 
delete []usymbol;
}

void Cover::printUncovered(bitA * Covered, unsigned int **bTable,int t, int k, int v){

 int *usymbol=new int[t];
  vector <colVal> trans_usymbol;
  int tmp_rankno;
  int tmp_count;
  tmp_count=bTable[v][t];
  int numofremaining=0;


  //produce the first t-set
  for(int i=0;i<t;i++)
	  usymbol[i]=i;

  //check if such first t-set is covered or not
 //transfer (0,3) to (0_0,1_0) mode 
      for (int ii=0;ii<t;ii++){
      	colVal c_colval; //c_colval=corresponding col and val
      	c_colval.val=usymbol[ii];
      	c_colval.col=getCol(c_colval.val);
    	c_colval.val=getIndex(c_colval.col,c_colval.val); //get index

      	trans_usymbol.push_back(c_colval);
      }
      
	  //check if it follows legal contrainst

			int * tmp_uset=new int[t];
			for(int i=0;i < trans_usymbol.size(); i++)
			{
				colVal tmpStru = trans_usymbol.at(i);
				//construct a uset
				tmp_uset[i]=getSym(tmpStru.col,tmpStru.val); //field,index

			}
			
			//get index of position of such pair (0,3)

			int rankno= LexRank(tmp_uset,v,t,bTable);
			
			//update the corresponding pairs Covered information

			if(Covered[rankno].on==0){ // remaining 
				cout<<numofremaining<<"-->";
				for(int i=0;i < trans_usymbol.size(); i++)
				{
					cout<<tmp_uset[i] << "   ";	
				}
				cout<<endl;
				numofremaining++;
			}


//then check the remaining t-sets and print it out 

for(int x=1; x< tmp_count; x++){
	  trans_usymbol.clear();
      LexSuccessor(usymbol,v,t);
	 
      //transfer (0,3) to (0_0,1_0) mode 
      for (int ii=0;ii<t;ii++){
      	colVal c_colval; //c_colval=corresponding col and val
      	c_colval.val=usymbol[ii];
      	c_colval.col=getCol(c_colval.val);
		c_colval.val=getIndex(c_colval.col,c_colval.val); //get index
      	trans_usymbol.push_back(c_colval);
      }
      

			int * tmp_uset=new int[t];
			for(int i=0;i < trans_usymbol.size(); i++)
			{
				colVal tmpStru = trans_usymbol.at(i);
				//construct a uset
				tmp_uset[i]=getSym(tmpStru.col,tmpStru.val); //field,index
			}
		
			//get index of position of such pair (0,3)

			int rankno= LexRank(tmp_uset,v,t,bTable);
			
			//update the corresponding pairs Covered information

			if(Covered[rankno].on==0){ // remaining 
				cout<<numofremaining<<"-->";
				for(int i=0;i < trans_usymbol.size(); i++)
				{
					cout<<tmp_uset[i] << "   ";	
				}
				cout<<endl;
				numofremaining++;
			}


}

delete []usymbol;
}


void Cover::KVCombo(int **kvcombo,unsigned int ** bTable,int t){

/*

inputs:
		kvcombo: for v value of k factor, kvcombo(k,v) gives the # of t-sets occurences which have such v
		bTable: binomTable
		t: strength 

output: 
	   created, initilized and updated kvcombo[][] basing on the contraints

*/
	// New Method to compute the kvcombo[][] by considering constraints

	/*
	0:  create and initialize kvcombo
	1. loop all real legal value pairs (not column pair) by using
		both isIllegal() and the lexsuccessor()
	2. search if such pair is covered in Cover or not
	3. if not then, update the kvcombo[][] 
		3.1 transform the the value into column and index
		3.2 update kvcombo[column][index]++
	*/

  int *usymbol=new int[t];

  vector <colVal> trans_usymbol;
  vector <int> smt_usymbol;
  
  int tmp_rankno;
  int tmp_count;
  tmp_count=bTable[v][t]; 

  //initialize 
	for(int i=0; i< k; i++)
		 for(int j=0; j< v; j++)
			kvcombo[i][j]=0;

 //produce the first t-set
  for(int i=0;i<t;i++)
	  usymbol[i]=i;
  
  //transfer (0,3) to (0_0,1_0) mode 
      for (int ii=0;ii<t;ii++){
      	colVal c_colval; //c_colval=corresponding col and val
      	c_colval.val=usymbol[ii];
      	c_colval.col=getCol(c_colval.val);
     	
		//c_colval.val=c_colval.val-v_col*c_colval.col;
	c_colval.val=getIndex(c_colval.col,c_colval.val); //get index
	//cout<<"getindex()5"<<endl;
      	trans_usymbol.push_back(c_colval);
		smt_usymbol.push_back(c_colval.val);
      }
      
	cover_smt->addScope();
	cover_smt->add_constraints(smt_usymbol);
	int satOrnot = cover_smt->checkSAT(false);
	cover_smt->clearScope();
	 if(satOrnot==1){ //sat
		
		 //then update kvcombo[column][index]++
		 for (int ii=0;ii<t;ii++){
			int column=(trans_usymbol.at(ii)).col;
			int sequence=(trans_usymbol.at(ii)).val;

			//update kvcombo[column][index]++
			kvcombo[column][sequence]++;
		}
	 }

// then produce remaining t-sets and updates the KVcombo data structures

	for(int x=1; x< tmp_count; x++){
	  trans_usymbol.clear();
	  smt_usymbol.clear();
      LexSuccessor(usymbol,v,t);
	 
      //transfer (0,3) to (0_0,1_0) mode 
      for (int ii=0;ii<t;ii++){
      	colVal c_colval; //c_colval=corresponding col and val
      	c_colval.val=usymbol[ii];
      	c_colval.col=getCol(c_colval.val);
    	c_colval.val=getIndex(c_colval.col,c_colval.val); //get index
      	trans_usymbol.push_back(c_colval);
		smt_usymbol.push_back(c_colval.val);
      }

    		 // then justify if it follows implicit or explicit avoids constraint
			cover_smt->addScope();
			cover_smt->add_constraints(smt_usymbol);
			int satOrnot = cover_smt->checkSAT(false);
			cover_smt->clearScope();
			 if(satOrnot==1){ // it follows implicit or explicit constraints, aetg should produce it
				
				 //then update kvcombo[column][index]++
				 for (int ii=0;ii<t;ii++){
      				int column=(trans_usymbol.at(ii)).col;
					int sequence=(trans_usymbol.at(ii)).val;

					kvcombo[column][sequence]++;
				}
			 }
	 }
	delete []usymbol;

}


