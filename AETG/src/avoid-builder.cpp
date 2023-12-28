#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "math.h"
#include <fstream>
#include <set>
#include <dirent.h>
#include "util.h"


using namespace std;


int MAX_LINE_LENGTH =300;
int MAX_WORD_LENGTH=100;
int MAX_NUM_CONTRAINTS=20;
int MAX_IMPLICIT_NUM=2;

struct factor_value {
			int factor;
			vector <int> value;
};

void  printavoids(vector<struct factor_value> fv){
			struct factor_value new_factor;
			for(int i=0;i<fv.size();i++){
				new_factor=fv.at(i);
				cout<<new_factor.factor<<"=";
				for(int j=0;j<new_factor.value.size();j++){
					cout<<new_factor.value.at(j)<<":";
				}
				cout<<endl;
			}
}

void read_paras(char* paraFile, vector <struct factor_value > & para_factor_value, int & k, int & v,int & t){
 		// read all avoids in file to all_avoids vector;
		
 	 char para[MAX_LINE_LENGTH];
	 char word_line[MAX_WORD_LENGTH];
	 int lineNum=0; // first line: k, second line:v, third line: t, start from fourth line to end: factors

	 ifstream inputfile (paraFile);

	 if (! inputfile.is_open())
	 { cout << "Error opening file"; exit (1); }
	
	while (! inputfile.eof() )
	{
		inputfile.getline (para,MAX_LINE_LENGTH); // read one line avoids
		
		if(para[0]=='e'||para[0]=='\t'||para[0]=='\n'||para[0]=='\0'){ // last line
			break;
		}

		char *lp=para;
		char *wp = word_line;
			
		while (*lp && (*lp != 'e')&& (*lp != '\t') && (*lp != '\n')) {
		    *(wp++) = *(lp++);
		}
		*wp = '\0';                                 // terminate string

		// so far collecting one word, one line

		if (lineNum==0){//k
			k=atoi(word_line);
			//cout<<k<<endl;
			
		}else if (lineNum==1){// v
			v=atoi(word_line);
			//cout<<v<<endl;
		}else if (lineNum==2){// t
			t=atoi(word_line);
			//cout<<t<<endl;
		}else {// factors 
			//cout<<"linenum:"<<lineNum<<endl;
			struct factor_value new_factor;
			new_factor.factor=atoi(word_line);
			//construct a value vector for such factor
			vector<int> valuetmp;
			int tmp;
			for(int i=0;i<v;i++){
				tmp=new_factor.factor*v+i;
				valuetmp.push_back(tmp);
			}
			new_factor.value=valuetmp;
			para_factor_value.push_back(new_factor);
		}

		lineNum++;
		
	}
	 //printavoids(para_factor_value);
}

void sysRandomContruct(vector<struct factor_value> constraints,int kk,int v){
	// write constraints to avoid.txt file
	//size from 1 to kk (including kk)

	ofstream mf ("avoid.txt");
	struct factor_value tmpstr;
	int tmpint,tmp2;
	set<int> factorSet;

	if (mf.is_open())
	{
		for(int i=0;i<kk;i++){	//sysmatically explore the size from 1 to kk
			factorSet.clear();
			while(factorSet.size()<(i+1)){
				tmpint=rand()%constraints.size();
				factorSet.insert(tmpint);
			}
			//so far, get the i factors
			set<int>::iterator it = factorSet.begin();
			while(it!=factorSet.end()){
				tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
				tmpint=rand()%tmpstr.value.size();
				//tmp2=tmpstr.factor*v+tmpint; //construct the value
				tmp2=tmpint;
				mf<<tmpstr.factor<<":"<<tmp2<<":";
				it++;
			}
			mf<<"e"<<endl; // finish one line
		}
		mf<<"e"<<endl; // last line of the contraint file
		mf.close();
	}else{
		cout<<"avoid.txt is not opened!"<<endl;
	}
	cout<<"SYSTEMATICALLY avoid.txt is built successfully!"<<endl;
}

void randomContruct(vector<struct factor_value> constraints,int v){
	// write constraints to avoid.txt file
	//random number of contraints(<=20) and random number of factors for each contraint 

	ofstream mf ("avoid.txt");
	struct factor_value tmpstr;
	int tmpint,tmp2;
	set<int> factorSet;

	if (mf.is_open())
	{
		int randSize=rand()%MAX_NUM_CONTRAINTS;
			
		int count=0;
		while(count<randSize){ // construct randSize number of contraints
			int conSize=rand()%constraints.size(); 
			
			factorSet.clear();
			while(factorSet.size()<(conSize+1)){ //construct conSize length of contraint for each constraint
				tmpint=rand()%constraints.size();
				factorSet.insert(tmpint);
			}
			//so far, get the i factors
			set<int>::iterator it = factorSet.begin();
			while(it!=factorSet.end()){
				tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
				tmpint=rand()%tmpstr.value.size();
				//tmp2=tmpstr.factor*v+tmpint; //construct the value
				tmp2=tmpint;
				mf<<tmpstr.factor<<":"<<tmp2<<":";
				it++;
			}
			mf<<"e"<<endl; // finish one line
			count++;
		}

		mf<<"e"<<endl; // last line of the contraint file
		mf.close();
	}else{
		cout<<"avoid.txt is not opened!"<<endl;
	}
	cout<<"RANDOMLY avoid.txt is built successfully!"<<endl;
}

void implicitContruct(vector<struct factor_value> constraints,int v){
	// write constraints to avoid.txt file
	//implicit constraints
	/*ofstream mf ("avoid.txt");
	struct factor_value tmpstr;
	int tmpint,tmp2;
	set<int> factorSet;
	
	//int ImplicitNum=rand()%MAX_IMPLICIT_NUM; 
	int ImplicitNum=1;
	set <int> implicitIndex;
	int implicitCount=0; // count the sequence of asigned values
	if (mf.is_open())
	{
		int randSize=rand()%MAX_NUM_CONTRAINTS;
		if(randSize<ImplicitNum){
			randSize=randSize+ImplicitNum;
		}
		
		//produce ImplicitNum of factors for the coming partition
		while(implicitCount<ImplicitNum){
			implicitIndex.push_back(rand()%constraints.size());
		}

		int count=0; //# of constraints 

		while(count<randSize){ // construct randSize number of contraints
			int conSize=rand()%constraints.size(); 

			factorSet.clear();
			set<int>::iterator it1 = implicitIndex.begin();
			while(it1!=implicitIndex.end()){ //copy those columns
				factorSet.insert(*it1);
				it1++;
			}
			

			while(factorSet.size()<conSize){ //construct conSize-1 length of contraint for each constraint
				tmpint=rand()%constraints.size();
				factorSet.insert(tmpint);
			}

			//so far, get the i factors
			set<int>::iterator it = factorSet.begin();
			//cout<<"-----"<<selectedFactor<<":"<<conSize<<endl;
			while(it!=factorSet.end()){
				//cout<<*it<<endl;
				if(*it==selectedFactor){
					tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
					tmp2=tmpstr.factor*v+selectedCount; //construct the value
					mf<<tmpstr.factor<<":"<<tmp2<<":";
					selectedCount++;
				}else{//other factors
					tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
					tmpint=rand()%tmpstr.value.size();
					tmp2=tmpstr.factor*v+tmpint; //construct the value
					mf<<tmpstr.factor<<":"<<tmp2<<":";
				}
				it++;
			}
			mf<<"e"<<endl; // finish one line
			count++;
		}

		mf<<"e"<<endl; // last line of the contraint file
		mf.close();
	}else{
		cout<<"avoid.txt is not opened!"<<endl;
	}
	cout<<"Implicit for factor ";
	for(int i=0;i<)
		selectedFactor<<" 's avoid.txt is built successfully!"<<endl;
	*/
}


void allValueforOneFactorContruct(vector<struct factor_value> constraints,int v){
	// write constraints to avoid.txt file
	// basically it is a random construction. But for a random factor, fill in all values 

	ofstream mf ("avoid.txt");
	struct factor_value tmpstr;
	int tmpint,tmp2;
	set<int> factorSet;
	
	int selectedFactor=rand()%constraints.size(); 
	int selectedCount=0; // count the sequence of asigned values
	if (mf.is_open())
	{
		int randSize=v;
		int count=0;

		while(count<randSize){ // construct randSize number of contraints
			int conSize=rand()%constraints.size(); 
			
			factorSet.clear();
			
			factorSet.insert(selectedFactor); // always insert such column 

			while(factorSet.size()<conSize){ //construct conSize-1 length of contraint for each constraint
				tmpint=rand()%constraints.size();
				factorSet.insert(tmpint);
			}

			//so far, get the i factors
			set<int>::iterator it = factorSet.begin();
			//cout<<"-----"<<selectedFactor<<":"<<conSize<<endl;
			while(it!=factorSet.end()){
				//cout<<*it<<endl;
				if(*it==selectedFactor){
					tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
					//tmp2=tmpstr.factor*v+selectedCount; //construct the value
					tmp2=selectedCount;
					mf<<tmpstr.factor<<":"<<tmp2<<":";
					selectedCount++;
				}else{//other factors
					tmpstr=constraints.at(*it); // *it = index of factor_value in constraints
					tmpint=rand()%tmpstr.value.size();
					//tmp2=tmpstr.factor*v+tmpint; //construct the value
					tmp2=tmpint;
					mf<<tmpstr.factor<<":"<<tmp2<<":";
				}
				it++;
			}
			mf<<"e"<<endl; // finish one line
			count++;
		}

		mf<<"e"<<endl; // last line of the contraint file
		mf.close();
	}else{
		cout<<"avoid.txt is not opened!"<<endl;
	}
	cout<<"All Values for factor "<<selectedFactor<<" 's avoid.txt is built successfully!"<<endl;
}




int main(int argc, char **argv){
		
	/*
		k: factor
		v: values for each factor
		k1: factor for contructing constraints
		k2: factor for contructing constraints
		...
		kn  factor for contructing constraints
	*/


   srand(int (time(NULL)));
	
	int k,v,kk;
	char * contraints_file;
	char *cons_para; // construction parameters. 
	// i:implicit, s:symtematically, r:random
	// a:all vlaues,u:UNSAT 
	vector <struct factor_value> constraints;

		if (argc==3){
		  	contraints_file=argv[1];
			cons_para=argv[2];
		}else{
			printf("constraint_file cons_para:\n");
		   return 0;
		}
	
	read_paras( contraints_file,  constraints,  k,v,kk); /*get the corresponding parameters*/
	if(cons_para[0]=='i'){//i: implicit
		implicitContruct(constraints, v);
	}else if (cons_para[0]=='s'){//s : symtematically 
		sysRandomContruct( constraints, kk, v);
	}else if (cons_para[0]=='r'){ // r:random
		randomContruct( constraints, v);

	}else if (cons_para[0]=='a'){ //a: all values for one random factor
		allValueforOneFactorContruct(constraints, v);

	}else { // wrong construction parameters
		cout<<"wrong parameters!";
		exit(1);
	}
 	
	
		

}





