/*

FileName: avoid_check_sat.cpp

Object: 
1. Transform the CA(k,g) + updated test vector to SAT constraint model for reading
2. Transform SAT constraint model to zchaff model for getting SAT or UNSAT

input:  k(variables), g(value), avoids.txt, and test vector
output: SAT or UNSAT for the transformed zchaff model

format:
        k,g : integer;
        avoids.txt:  0:0:1:1:e
        						 0:1:1:1:e
        						 e
        test vector: 0:2:1:3:2:2:e
        
*/

#ifndef AVOID_H
#define AVOID_H


#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "math.h"
#include <fstream>
#include <set>
#include <dirent.h>
#include "util.h"
#include "SAT.h"
#include <sys/time.h>   // needed for timing
#include <sys/resource.h>   // needed for timing

//#include "cover.h"

using namespace std;

 class CA_SAT{
 	
			int MAX_SIZE; // for integer transformation from string sprintf()
			int MAX_AVOIDS_LENGTH;
			int MAX_LINE_LENGTH;    
			int MAX_WORD_LENGTH;    
			
			struct var_num{ //for transforming from sat model to zchaff model 
				string variable; // variable 
				int num; // index
			};
			
			struct avoid_unit {
				string column;
				string value;
			};
			
			vector < vector <struct avoid_unit> > all_avoids;
			
			vector <struct var_num> VARSET; // all variables k*g, such as m_0(col)_0(value)
			int** VARSET2; // all variables k*karray size, such as (k=0,index=0)=unique num for zChaff

		 	vector <vector <struct avoid_unit> > allTsetConstraints; //exstracting all explicit t-sets
			set <string > allInvolvedFactors; //exstracting all involved factors from the constraints set
			
		public:

			SAT_Manager mng ; //initialize the sat manager
			SAT_Manager mng_withFixedClauses; //initialize the sat manager
			
			vector<vector <int> > _atleastAvoids; //at least constraints + avoids constraints 
			vector <vector<int> > allTransformedConstraints; // all transformed constraints
			vector <string> clause;// base clause set
			vector <string> finalClause;// base+testcase, used for constructing final zchaff model
			vector<vector <int> > allCNF_before,allCNF_after; // for debugging the addClause()

			CA_SAT(void);
			~CA_SAT();
			void printClause();
			void printAllBeforeAfter();
			void printavoids();
			void print(vector < vector <struct avoid_unit> > &,const vector<colVal>&);

			int  getNum(vector <struct var_num >,string);
			void transSAT_Zchaff(int,int);
			void transSAT_Zchaff(int ,int* );

			void writeToFile(const  char*);
			void read_cnf(SAT_Manager mng, char * );
			void read_cnf(SAT_Manager mng);

			void write_avoids(int ,int);
			void write_avoids(int ,int* );


			void write_avoids_to_zChaff(int ,int* ,vector < vector <struct avoid_unit> > & );

			void write_avoids(int,int* ,vector < vector <struct avoid_unit> > &);


			void read_one_avoid_to_all_avoids(char[] );
			void read_avoids(const char*);
			void initAndCreateAvoidsCNF(const char*,int ,int);
			void initAndCreateAvoidsCNF(const char* ,int ,int*);
			
			void initAndCreateAvoidsCNF_to_zChaff(const char* ,int ,int*);
			

			void initForLegalCNF(int k,int* karray); //for testing t-set legal 

			void createVectorCNF(int[], int);
			int  isSAT(int[], int, int);
					
			int  isSAT(const vector <colVal> &, int ,int);		

			int  isSAT(const vector<colVal>& , int, int*);

			int  isSAT(const vector<colVal>& , int, int*, float&, float&,float&,float&,
				float&,float&,
				float&,float&
				);


			

			void createVectorCNF(const vector<colVal>& );
			
			void createVectorCNF_to_zChaff(const vector<colVal>& );
			
			
			int  isSAT_vector(const vector <int>&, int ,int);
			void createVectorCNF_vector(const vector<int>&, int);
			
			void write_atleast(int,int);
			void write_atmost(int,int);

			void write_atleast(int,int*);
			void write_atmost(int,int*);

			void extractAllTsetConstraints(const char* ,int );
			void extractAllTsetConstraints(int );

			int isImplicit(vector<string> & );

			void extractAllFactors(const char*, int );
			void extractAllFactors(int);
			int isInvolvedFactor(int);
			int isInvolvedFactor(vector<int>);

			void transfromConstraints(const char*,int);
			void transfromConstraints(const char*);
			int getSym(int field, int index)const{return varray[field][index];}

			vector<string> & getBasicClausesRef(){return clause;}
			void setClause(vector<string> & bk_clause){clause=bk_clause;}
			void setMng(){mng = SAT_InitManager();}
			void releaseMng(SAT_Manager mng){SAT_ReleaseManager(mng);}
			void releaseCASAT(){SAT_ReleaseManager(mng);}
			
			void printFinalClause();
			int filterFinal(vector < vector <struct avoid_unit> > & , const vector<colVal>& );
			int filterFinal_SymbolLevel(vector < vector <struct avoid_unit> > & , const vector<colVal>& );
			
			void write_atleast_to_zChaff(int ,int* );
			void addClause(vector<int> , SAT_Manager& );
			void addAtleastAvoids(vector<vector <int> >, SAT_Manager& );

			int **varray;
			int *factorArray; //size=k
			int varNum; //for zChaff set
};

#endif
