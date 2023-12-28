#include "avoid-check-sat.h"


CA_SAT::CA_SAT(){
	MAX_SIZE=10;  // for integer transformation from string sprintf()
	MAX_AVOIDS_LENGTH=300;
	MAX_LINE_LENGTH=65536;
	MAX_WORD_LENGTH= 64;

	mng = SAT_InitManager();
	mng_withFixedClauses=SAT_InitManager();
	factorArray=NULL;
	
}

CA_SAT::~CA_SAT(){

	if(mng!=NULL) 
		SAT_ReleaseManager(mng); 

	if(mng_withFixedClauses!=NULL) 
		SAT_ReleaseManager(mng_withFixedClauses); 

	if(factorArray!=NULL){
		delete [] factorArray; 
		factorArray=NULL;
	}
}


/*void CA_SAT::printClause(){
	
	for(int i=0;i<clause.size();i++){
		cout<<clause.at(i)<<endl;
	}
}*/

void CA_SAT::printFinalClause(){
	
	for(int i=0;i<finalClause.size();i++){
		for(int j=0;j<finalClause.at(i).size();j++){
			cout<<finalClause.at(i)[j];
		}
		cout<<endl;
	}
}

void CA_SAT::printavoids(){
	
	for(int i=0;i<all_avoids.size();i++){
		for(int j=0;j<all_avoids.at(i).size();j++)
			cout<<":"<<all_avoids.at(i).at(j).column<<":"<<all_avoids.at(i).at(j).value;
		cout<<endl;	
	}
}

void CA_SAT::print(vector < vector <struct avoid_unit> > & avoids, const vector<colVal>& partialTC){
	
	for(int i=0;i<avoids.size();i++){
		for(int j=0;j<avoids.at(i).size();j++)
			cout<<":"<<avoids.at(i).at(j).column<<":"<<avoids.at(i).at(j).value;
		cout<<endl;	
	}

	for(int i=0;i<partialTC.size();i++){
		
			cout<<":"<<partialTC.at(i).col<<":"<<partialTC.at(i).val;

		cout<<endl;	
	}
}


int CA_SAT::getNum(vector <struct var_num> var_num_set,string str){
		
	struct var_num tmp;

	for(int i=0;i<var_num_set.size();i++){
		tmp= var_num_set.at(i);
		if(tmp.variable==str){
			return tmp.num;
		}
	}

	//cout<<"There is no such variable! The construction process of varialbe is Wrong!"<<endl;
}
void CA_SAT::transSAT_Zchaff(int k,int g){
	
	string tmp;
	int index;
	char strk[MAX_SIZE],strg[MAX_SIZE],tmpstr[MAX_SIZE]; // itoa 
	
	// VARSET variable
		for(int i=0;i<k;i++)
				for (int j=0;j<g;j++)
	      		{
								sprintf(strk,"%d",i);
								sprintf(strg,"%d",j);
								tmp="";
	
								tmp=tmp+"m_"+strk+"_"+strg;
								index=getNum(VARSET,tmp);
								sprintf(tmpstr,"%d",index);
							
								for(int f=0;f<finalClause.size();f++){
										int pos=finalClause.at(f).find(tmp,0);
										if(pos!= string::npos){ // pattern match  
											finalClause.at(f).replace(pos,tmp.length(),tmpstr);
										}
								} 
	      		}
        		
	for(int i=0;i<finalClause.size();i++){
		finalClause.at(i)=finalClause.at(i)+"   0"; // add 0 at each end of Clause

		for (int j=0; j<finalClause.at(i).length();j++){ // "V" =="  " 
			if (finalClause.at(i)[j]=='V'){
				finalClause.at(i)[j]=' ';
				finalClause.at(i).insert(j," ");
			}
		}
	} 
	
	finalClause.push_back("0"); // add 0 at the end of model

	//printFinalClause();

}


void CA_SAT::transSAT_Zchaff(int k,int* karray){
	
	string tmp;
	int index;
	char strk[MAX_SIZE],strg[MAX_SIZE],tmpstr[MAX_SIZE]; // itoa 
	
	
	// VARSET variable
		for(int i=k-1;i>=0;i--)
			if(karray[i]>0){
				for (int j=karray[i]-1;j>=0;j--)
	      		{
								sprintf(strk,"%d",i);
								sprintf(strg,"%d",j);
								tmp="";
	
								tmp=tmp+"m_"+strk+"_"+strg;
								index=getNum(VARSET,tmp);
								sprintf(tmpstr,"%d",index);
							
								for(int f=0;f<finalClause.size();f++){
										int pos=finalClause.at(f).find(tmp,0);
										if(pos!= string::npos){ // pattern match  
											finalClause.at(f).replace(pos,tmp.length(),tmpstr);
										}
								} 
	      		}
			}else{
				cout<<"karray["<<i<<"]="<<karray[i]<<endl;
			}
				

	char firstlineSize[MAX_SIZE], firstClauseSize[MAX_SIZE];
	
	sprintf(firstlineSize,"%d",VARSET.size());
	sprintf(firstClauseSize,"%d",finalClause.size()-1);

	string firstline="p cnf ";
	firstline.append(firstlineSize);
	firstline.append("  ");
	firstline.append(firstClauseSize);
	
	finalClause.insert(finalClause.begin(),firstline);

	for(int i=0;i<finalClause.size();i++){
		finalClause.at(i)=finalClause.at(i)+"   0"; // add 0 at the end of finalClause

		for (int j=0; j<finalClause.at(i).length();j++){ // "V" =="  " 
			if (finalClause.at(i)[j]=='V'){
				finalClause.at(i)[j]=' ';
				finalClause.at(i).insert(j," ");
			}
		}
	} 
	
	finalClause.push_back("0"); // add 0 at the end of model
	

}

void CA_SAT::writeToFile(const  char* filename){
	
	// add the p n variable_num clause_num
	
	ofstream mf (filename);
	
	if (mf.is_open())
	{
		//mf << "p cnf "<<VARSET.size() << "  " <<finalClause.size()-1<<endl;
		for(int i=0;i<finalClause.size();i++){
			mf << finalClause.at(i)<<endl; // add 0 at the end of clause
		}  
			
		mf.close();
	}
	
}

void CA_SAT::printAllBeforeAfter(){

	vector<int> tmp;
	
	cout<<"==============================="<<endl;

	for(int i=0;i<allCNF_before.size();i++){
		tmp=allCNF_before.at(i);
		for(int j=0;j<tmp.size();j++){
			cout<<tmp.at(j)<<" ";
		}
		cout<<endl;
	}
	
	cout<<"++++++++++++++++++++++++++++++++"<<endl;

	for(int i=0;i<allCNF_after.size();i++){
		tmp=allCNF_after.at(i);
		for(int j=0;j<tmp.size();j++){
			cout<<tmp.at(j)<<" ";
		}
		cout<<endl;
	}

}

void CA_SAT::addAtleastAvoids(vector<vector <int> > atleastAvoids, SAT_Manager& mng){
	
	SAT_SetNumVariables(mng, varNum); //varNum to zChaff mng
	vector<int> tmp;
	
	for(int i=0;i<atleastAvoids.size();i++){
		tmp=atleastAvoids.at(i);
		SAT_AddClause(mng, &tmp.begin()[0], tmp.size());
	}

}
void CA_SAT::addClause(vector<int> oneRow, SAT_Manager& mng){

		set<int> clause_vars;
		set<int> clause_lits;

		vector<int> tmpbefore,tmpafter;	
		
		for(int i=0;i<oneRow.size();i++){
					
					int var_idx = oneRow.at(i);
					
					tmpbefore.push_back (var_idx);

					int sign = 0;

					if( var_idx != 0) {
							if( var_idx < 0)  { 
								var_idx = -var_idx; sign = 1; 
							}
							clause_vars.insert(var_idx);
							clause_lits.insert( (var_idx << 1) + sign);
					} else {
							//add this clause
							if (clause_vars.size() != 0 && (clause_vars.size() == clause_lits.size())) { //yeah, can add this clause
								vector <int> temp;
								for (set<int>::iterator itr = clause_lits.begin();itr != clause_lits.end(); ++itr){
										temp.push_back (*itr);
										tmpafter.push_back (*itr);
										
								}
							
								SAT_AddClause(mng, & temp.begin()[0], temp.size() );
								//cout<<""<<temp.begin()[0]<<endl;

							}else {//it contain var of both polarity, so is automatically satisfied, just skip it
					
							} 
							
							allCNF_before.push_back(tmpbefore);
							allCNF_after.push_back(tmpafter);
							
							//_atleastAvoids.push_back(tmpafter);

							tmpafter.clear();
							tmpbefore.clear();

							clause_lits.clear();
							clause_vars.clear();
					}
		}
		 
}
void CA_SAT::read_cnf(SAT_Manager mng )
{
	//cout <<"read cnf "<<endl;

    char line_buffer[MAX_LINE_LENGTH];
    char word_buffer[MAX_WORD_LENGTH];
    set<int> clause_vars;
    set<int> clause_lits;
    int line_num = 0;
    
	//printFinalClause();

   	while (line_num<finalClause.size()){
	
		//copy each line in finalClause to such line_buffer
		//if(line_num==0){
			//mf << "p cnf "<<VARSET.size() << "  " <<finalClause.size()-1<<endl;
			//888line_buffer[]
		//}else{
		for (int j=0; j<finalClause.at(line_num).size();j++){ //		inp.getline(line_buffer, MAX_LINE_LENGTH)) ;
			line_buffer[j]=finalClause.at(line_num)[j];
		}
		line_buffer[finalClause.at(line_num).size()]='\0';
		//}

	++ line_num;
	if (line_buffer[0] == 'c') { 
	    continue; 
	}
	else if (line_buffer[0] == 'p') {
	    int var_num;
	    int cl_num;

	    int arg = sscanf (line_buffer, "p cnf %d %d", &var_num, &cl_num);
	    if( arg < 2 ) {
		cerr << "Unable to read number of variables and clauses"
		     << "at line " << line_num << endl;
		exit(3);
	    }
	    SAT_SetNumVariables(mng, var_num); //first element not used.
	}
	else {                             // Clause definition or continuation
	    char *lp = line_buffer;
	    do {
		char *wp = word_buffer;
		while (*lp && ((*lp == ' ') || (*lp == '\t'))) {
		    lp++;
		}
		while (*lp && (*lp != ' ') && (*lp != '\t') && (*lp != '\n')) {
		    *(wp++) = *(lp++);
		}
		*wp = '\0';                                 // terminate string

		if (strlen(word_buffer) != 0) {     // check if number is there
		    int var_idx = atoi (word_buffer);
		    int sign = 0;

		    if( var_idx != 0) {
			if( var_idx < 0)  { var_idx = -var_idx; sign = 1; }
			clause_vars.insert(var_idx);
			clause_lits.insert( (var_idx << 1) + sign);
		    } 	
		    else {
			//add this clause
			if (clause_vars.size() != 0 && (clause_vars.size() == clause_lits.size())) { //yeah, can add this clause
			    vector <int> temp;
			    for (set<int>::iterator itr = clause_lits.begin();
				 itr != clause_lits.end(); ++itr)
				temp.push_back (*itr);
			    SAT_AddClause(mng, & temp.begin()[0], temp.size() );
			}
			else {} //it contain var of both polarity, so is automatically satisfied, just skip it
			clause_lits.clear();
			clause_vars.clear();
		    }
		}
	    }
	    while (*lp);
	}
    }
   
	/*if (!inp.eof()) {
	cerr << "Input line " << line_num <<  " too long. Unable to continue..." << endl;
	exit(2);
    }*/

//    assert (clause_vars.size() == 0); 	//some benchmark has no 0 in the last clause
    if (clause_lits.size() && clause_vars.size()==clause_lits.size() ) {
	vector <int> temp;
	for (set<int>::iterator itr = clause_lits.begin();
	     itr != clause_lits.end(); ++itr)
	    temp.push_back (*itr);
	SAT_AddClause(mng, & temp.begin()[0], temp.size() );
    }
    clause_lits.clear();
    clause_vars.clear();
//    cout <<"done read cnf"<<endl;

	
}

void CA_SAT::read_cnf(SAT_Manager mng, char * filename )
{
//    cout <<"read cnf "<<endl;
    char line_buffer[MAX_LINE_LENGTH];
    char word_buffer[MAX_WORD_LENGTH];
    set<int> clause_vars;
    set<int> clause_lits;
    int line_num = 0;
    
    if(opendir(filename)){
        cerr << "Can't open input file, it's a directory" << endl;
	exit(1);
    }
    
    ifstream inp (filename, ios::in);
    if (!inp) {
	cerr << "Can't open input file" << endl;
	exit(1);
    }
    while (inp.getline(line_buffer, MAX_LINE_LENGTH)) {
		++ line_num;
		if (line_buffer[0] == 'c') { 
		  continue; 
		}
		else if (line_buffer[0] == 'p') {
			int var_num;
			int cl_num;

			int arg = sscanf (line_buffer, "p cnf %d %d", &var_num, &cl_num);
			if( arg < 2 ) {
				cerr << "Unable to read number of variables and clauses"
					<< "at line " << line_num << endl;
				exit(3);
			}
			SAT_SetNumVariables(mng, var_num); //first element not used.
		}
		else {                             // Clause definition or continuation
			char *lp = line_buffer;
			do {
				char *wp = word_buffer;
				while (*lp && ((*lp == ' ') || (*lp == '\t'))) {
					lp++;
				}
				while (*lp && (*lp != ' ') && (*lp != '\t') && (*lp != '\n')) {
					*(wp++) = *(lp++);
				}
				*wp = '\0';                                 // terminate string

				if (strlen(word_buffer) != 0) {     // check if number is there
						int var_idx = atoi (word_buffer);
						int sign = 0;

						 if( var_idx != 0) {
								if( var_idx < 0)  { var_idx = -var_idx; sign = 1; }
										clause_vars.insert(var_idx);
										clause_lits.insert( (var_idx << 1) + sign);
								}else {
										//add this clause
										if (clause_vars.size() != 0 && (clause_vars.size() == clause_lits.size())) { //yeah, can add this clause
											vector <int> temp;
											for (set<int>::iterator itr = clause_lits.begin();
											itr != clause_lits.end(); ++itr)
											temp.push_back (*itr);
											SAT_AddClause(mng, & temp.begin()[0], temp.size() );
										}else {
										} //it contain var of both polarity, so is automatically satisfied, just skip it
										clause_lits.clear();
										clause_vars.clear();
								}//var_idx < 0
						}//var_idx != 0
				} while (*lp); //end do 
		}//if(line_buffer[0] == 'c')
    }//while(inp.getline(line_buffer, MAX_LINE_LENGTH) )

    if (!inp.eof()) {
		cerr << "Input line " << line_num <<  " too long. Unable to continue..." << endl;
		exit(2);
    }
//    assert (clause_vars.size() == 0); 	//some benchmark has no 0 in the last clause
    if (clause_lits.size() && clause_vars.size()==clause_lits.size() ) {
	vector <int> temp;
	for (set<int>::iterator itr = clause_lits.begin();
	     itr != clause_lits.end(); ++itr)
	    temp.push_back (*itr);
	SAT_AddClause(mng, & temp.begin()[0], temp.size() );
    }
    clause_lits.clear();
    clause_vars.clear();
//    cout <<"done read cnf"<<endl;

	
}



void CA_SAT::write_avoids(int k,int g){
	// write all_avoids to clause
	
	vector <struct avoid_unit> onerowAvoid;
	string tmpstr="";
	struct avoid_unit one_unit;
	
	for(int i=0;i<all_avoids.size();i++){
		tmpstr="";
		onerowAvoid=all_avoids.at(i);
		char strb[MAX_SIZE];
  		for (int j=0; j<onerowAvoid.size();j++){
				one_unit=onerowAvoid.at(j);
  			tmpstr=tmpstr+"-m_"+one_unit.column+"_"+one_unit.value+"V";
  		}																																								
  		tmpstr=tmpstr.substr(0,tmpstr.length()-1); // delete the last V
  		clause.push_back(tmpstr);	 // base-clause 

		
	}
	
}

void CA_SAT::write_avoids(int k,int* karray){ 
	//handle multi-levels
	// this function DONOT use parameters at all! 
	//write all_avoids to clause
	
	vector <struct avoid_unit> onerowAvoid;
	string tmpstr="";
	struct avoid_unit one_unit;
	
	for(int i=0;i<all_avoids.size();i++){
		tmpstr="";
		onerowAvoid=all_avoids.at(i);
		char strb[MAX_SIZE];
  		for (int j=0; j<onerowAvoid.size();j++){
			one_unit=onerowAvoid.at(j);
  			tmpstr=tmpstr+"-m_"+one_unit.column+"_"+one_unit.value+"V";
  		}																																								
  		tmpstr=tmpstr.substr(0,tmpstr.length()-1); // delete the last V
  		clause.push_back(tmpstr);	
	}
	
}

void CA_SAT::write_avoids(int k,int* karray,vector < vector <struct avoid_unit> > & fileredAvoids){ 
	//handle multi-levels
	// this function DONOT use parameters at all! 
	//write all_avoids to clause
	
	vector <struct avoid_unit> onerowAvoid;
	string tmpstr="";
	struct avoid_unit one_unit;
	
	for(int i=0;i<fileredAvoids.size();i++){
		tmpstr="";
		onerowAvoid=fileredAvoids.at(i);
		char strb[MAX_SIZE];
  		for (int j=0; j<onerowAvoid.size();j++){
			one_unit=onerowAvoid.at(j);
  			tmpstr=tmpstr+"-m_"+one_unit.column+"_"+one_unit.value+"V";
  		}																																								
  		tmpstr=tmpstr.substr(0,tmpstr.length()-1); // delete the last V
  		finalClause.push_back(tmpstr);	
	}
	
}





void CA_SAT::write_avoids_to_zChaff(int k,int* karray,vector < vector <struct avoid_unit> > & fileredAvoids){ 
	//handle multi-levels
	// this function DONOT use parameters at all! 
	//write all_avoids to clause
	
	vector <struct avoid_unit> onerowAvoid;
	struct avoid_unit one_unit;
	vector <int> oneRow;

	for(int i=0;i<fileredAvoids.size();i++){

		onerowAvoid=fileredAvoids.at(i);
		
		for (int j=0; j<onerowAvoid.size();j++){
		
			one_unit=onerowAvoid.at(j);
			int column=atoi(one_unit.column.c_str()) ;
			int index=atoi(one_unit.value.c_str()) ;
			//cout<<column<<":"<<index<<endl;
  			oneRow.push_back(-VARSET2[column][index]);

  		}																																								
		oneRow.push_back(0);

		/*for (int j=0; j<oneRow.size();j++){
			cout<<oneRow.at(j)<<":";
							
		}
		cout<<endl;*/

		addClause(oneRow,mng_withFixedClauses);
		oneRow.clear();
  		//finalClause.push_back(tmpstr);	

	}
	
}



void CA_SAT::read_one_avoid_to_all_avoids(char avoidBuffer[]){
	// read one line avoid; write it to all_avoids vector
	
	vector<struct avoid_unit> oneavoid;
	
	int i=0;
	string word_column="", word_value="";
	
	while(avoidBuffer[i]!='e'){

			word_column="";
			word_value="";
			while(avoidBuffer[i]!=':'){ //collect one word for column
    			word_column=word_column+avoidBuffer[i];
					i++;
  		}
			i++; //jump ":"
			 
			while(avoidBuffer[i]!=':'){ //collect one word for value
    			word_value=word_value+avoidBuffer[i];
    			i++;
  		}
			i++;//jump ":"
			struct avoid_unit oneunit;
			oneunit.column=word_column;
			oneunit.value=word_value;
			oneavoid.push_back(oneunit);
	}
	
	all_avoids.push_back(oneavoid);
	
	

}

void CA_SAT::read_avoids(const char* avoidFile){
 		// read all avoids in file to all_avoids vector;
		
 	 char buffer[MAX_AVOIDS_LENGTH];

	 ifstream avoids (avoidFile);

	 if (! avoids.is_open())
	 { cout << "Error opening file"; exit (1); }

	while (! avoids.eof() )
	{
		avoids.getline (buffer,MAX_AVOIDS_LENGTH); // read one line avoids
		
		if(buffer[0]=='e'){
			break;
		}
		read_one_avoid_to_all_avoids(buffer);
	}
	
	 //printavoids();

	//delete []buffer;

}

void CA_SAT::write_atmost(int k,int g){
		 
		 char strk[MAX_SIZE],strg[MAX_SIZE],strg_2[MAX_SIZE];
		 

		 for (int j=0;j<k;j++)
				{
						string tmp;
				 		for (int l=0;l<g;l++){
								tmp="";
                tmp=tmp+"-"+"m"+"_";
								sprintf(strk,"%d",j);
								sprintf(strg,"%d",l);
								tmp=tmp+strk+"_"+strg;
								
								string tmp1,tmp2;
								
								for (int f=l+1;f<g;f++){
									tmp1="";
									tmp1=tmp1+"-"+"m"+"_";
									sprintf(strg_2,"%d",f);
									tmp1=tmp1+strk+"_"+strg_2;
									tmp2=tmp;
									tmp2=tmp2 + "V" + tmp1; 
									
									string newstr=tmp2;
									clause.push_back(newstr);
								}
						}
				}
				
}

void CA_SAT::write_atleast(int k,int g){
		  
		  int i,j;
		  char strk[MAX_SIZE],strb[MAX_SIZE],strg[MAX_SIZE];
          //int k_comb;
		  
	 		for (i=0;i<k;i++)
				{
						string tmp="";
				 		for (j=0;j<g;j++){
								tmp=tmp+"V"+"m"+"_";
								sprintf(strk,"%d",i); // column
								sprintf(strg,"%d",j); // value 
								tmp=tmp+strk+"_"+strg;
						}
						tmp=tmp.substr(1,tmp.length()); // delete first V
						string newstr=tmp;
						clause.push_back(newstr);
			}
				
}
void CA_SAT::write_atleast(int k,int* karray){ 
		  
		  int i,j;
		  char strk[MAX_SIZE],strb[MAX_SIZE],strg[MAX_SIZE];
          //int k_comb;
		  
	 		for (i=0;i<k;i++)
				{
						string tmp="";
				 		for (j=0;j<karray[i];j++){

								tmp=tmp+"V"+"m"+"_";
								sprintf(strk,"%d",i); // column
								sprintf(strg,"%d",j); // value 
								tmp=tmp+strk+"_"+strg;
						}
						tmp=tmp.substr(1,tmp.length()); // delete first V
						string newstr=tmp;
						clause.push_back(newstr);
			}
				
}

void CA_SAT::write_atleast_to_zChaff(int k,int* karray){ 
		  
		vector <int> oneRow;																	
		
		int i,j;
		int count=0;

		for (i=0;i<k;i++)
			{
				 		for (j=0;j<karray[i];j++){
							oneRow.push_back(VARSET2[i][j]);	
							count++;
						}
						oneRow.push_back(0);
						
						/*for (int j=0; j<oneRow.size();j++){
							cout<<oneRow.at(j)<<":";
						}
						cout<<endl;*/

						addClause(oneRow,mng_withFixedClauses);
						

						oneRow.clear();
			}
	varNum=count;
		
}

void CA_SAT::write_atmost(int k,int* karray){ // multiple levels for different factors
		 
		 char strk[MAX_SIZE],strg[MAX_SIZE],strg_2[MAX_SIZE];
		 
		 for (int j=0;j<k;j++)
				{
						string tmp;
				 		for (int l=0;l<karray[j];l++){
								tmp="";
								tmp=tmp+"-"+"m"+"_";
								sprintf(strk,"%d",j);
								sprintf(strg,"%d",l);
								tmp=tmp+strk+"_"+strg;
								
								string tmp1,tmp2;
								
								for (int f=l+1;f<karray[j];f++){
									tmp1="";
									tmp1=tmp1+"-"+"m"+"_";
									sprintf(strg_2,"%d",f);
									tmp1=tmp1+strk+"_"+strg_2;
									tmp2=tmp;
									tmp2=tmp2 + "V" + tmp1; 
									
									string newstr=tmp2;
									clause.push_back(newstr);
								}
						}
				}
			
}

void CA_SAT::initAndCreateAvoidsCNF(const char* avoidFile,int k,int* karray){

        // g = karray[] 
	struct var_num tmp;
	char strk[MAX_SIZE],strg[MAX_SIZE]; // itoa 

	int count=1; // folloq zchaff convention
	int i,j;
	
	//at least for Naive model 
	write_atleast(k,karray);
	//at most for Naive model
	//write_atmost(k,karray);
	
	//All variables for avoids and test vector
	//k*g
		for(int i=0;i<k;i++)
				for (j=0;j<karray[i];j++)
        		{
							sprintf(strk,"%d",i);
							sprintf(strg,"%d",j);
							tmp.variable="";

							tmp.variable=tmp.variable+"m_"+strk+"_"+strg;
							tmp.num=count;
						
							struct var_num new_m_var;
							new_m_var.variable=tmp.variable;
							new_m_var.num=tmp.num;
						
							VARSET.push_back(new_m_var);
        			count++;
        		}
        		
  read_avoids(avoidFile);  /*read avoids from file*/ 
  write_avoids(k,karray);


	
}



void CA_SAT::initAndCreateAvoidsCNF_to_zChaff(const char* avoidFile,int k,int* karray){

	VARSET2=new int*[k];

	int count=1; // follow zchaff convention
	 
	//write_atleast(k,karray);
	
	//write_atmost(k,karray);

	for(int i=0;i<k;i++){
		
			VARSET2[i]=new int[karray[i]];

        	for (int j=0;j<karray[i];j++)
        		{
							
							VARSET2[i][j]=count;
        					count++;
        		}
	}
    
  varNum=count;

  SAT_SetNumVariables(mng_withFixedClauses, varNum);

	for(int i=0;i<k;i++){
        	for (int j=0;j<karray[i];j++)
        		{
					cout<<VARSET2[i][j]<<"  " ;
				}
				cout<<endl;
	}

	write_atleast_to_zChaff(k,karray);
	read_avoids(avoidFile);  /*read avoids from file*/ 
	write_avoids_to_zChaff(k,karray,all_avoids);

	for(int i=0;i<allCNF_after.size();i++){
		_atleastAvoids.push_back(allCNF_after.at(i));
	}
						
}

void CA_SAT::initForLegalCNF(int k,int* karray){
	
	//for testing t-set legal 
	//only at-least at-most constraints

	struct var_num tmp;
	char strk[MAX_SIZE],strg[MAX_SIZE]; // itoa 

	int count=1; // folloq zchaff convention
	int i,j;
	
	//at least for Naive model 
	//write_atleast(k,karray);
	//at most for Naive model
	write_atmost(k,karray);
	
	//All variables for avoids and test vector
	//k*g
		for(int i=0;i<k;i++)
				for (j=0;j<karray[i];j++)
        		{
							sprintf(strk,"%d",i);
							sprintf(strg,"%d",j);
							tmp.variable="";

							tmp.variable=tmp.variable+"m_"+strk+"_"+strg;
							tmp.num=count;
						
							struct var_num new_m_var;
							new_m_var.variable=tmp.variable;
							new_m_var.num=tmp.num;
						
							VARSET.push_back(new_m_var);
        					count++;
	       		}


}


void CA_SAT::createVectorCNF(int testvector[], int k){
	
	/*input: clause vector with only AVOIDS CNFs
	  output: clause vector with AVOIDS and Vector CNFs
	
	  String vector format: 
	  										  0:0:1:0:...:k-1:g:e 
	                          	 
   */
	
	char str_value[MAX_SIZE],str_column[MAX_SIZE];
	
	string clausestr="";

	for(int i=0;i<k;i++){

		//if(i!=0){
				clausestr="";
				sprintf(str_value,"%d",testvector[i]);
				sprintf(str_column,"%d",i);
				clausestr=clausestr+"m_"+str_column+"_"+str_value;
				string newstr=clausestr;
				finalClause.push_back(clausestr);
		//}
		
	}
	

}

void CA_SAT::createVectorCNF(const vector<colVal>& t_set){
	
	/*input: clause vector with only AVOIDS CNFs
	  output: clause vector with AVOIDS and Vector CNFs
	
	  String vector format: 
	  								0:0:1:0:...:k-1:g:e 
	                          	 
   */
	
	char str_value[MAX_SIZE],str_column[MAX_SIZE];
	
	string clausestr="";
	
	for(int i=0;i<t_set.size();i++){
			    colVal tcolval=t_set.at(i);
				clausestr="";
				sprintf(str_value,"%d",tcolval.val);
				sprintf(str_column,"%d",tcolval.col);
				clausestr=clausestr+"m_"+str_column+"_"+str_value;
				string newstr=clausestr;
				finalClause.push_back(clausestr);
	}
	
	
	//printFinalClause();
	
	/*
	
	cout<<"*********************************"<<endl;
	printClause();
	cout<<"*********************************"<<endl;
	*/
	
	
}


void CA_SAT::createVectorCNF_to_zChaff(const vector<colVal>& t_set){
	
	/*input: clause vector with only AVOIDS CNFs
	  output: clause vector with AVOIDS and Vector CNFs
	
	  String vector format: 
	  								0:0:1:0:...:k-1:g:e 
	                          	 
   */
	
	vector <int> oneRow;
	
	for(int i=0;i<t_set.size();i++){
			    colVal tcolval=t_set.at(i);
				
				//cout<<tcolval.col<<":"<<tcolval.val<<endl;

				oneRow.push_back(VARSET2[tcolval.col][tcolval.val]);	
				oneRow.push_back(0);
				addClause(oneRow,mng);
				oneRow.clear();
	}

	
	/*for (int j=0; j<oneRow.size();j++){
			cout<<oneRow.at(j)<<":";
							
		}
		cout<<endl;*/
	


}
int CA_SAT::isSAT(const vector<colVal>& t_set, int k, int v){
		
	 /*
	 		input: test t-set (pairs,triples,...)
	 				v=g
	 				
	 				
	 		output: 1-> consistent with avoids
	 		        0-> inconsistent with avoids
	 */
	 		
		   finalClause=clause;// so far, finalClause has all at_least, at_most and constraints. Only waiting for test case
		  
	 	   createVectorCNF(t_set); 		 
	 	   //writeToFile("satmodel.cnf"); //only for debug from such intermediate human readable formula model
		   transSAT_Zchaff(k,v);
     	   //writeToFile("zchaffmodel.cnf");
		
		   if(mng!=NULL){
				SAT_ReleaseManager(mng);
				
		   }
		   mng = SAT_InitManager();
		   read_cnf (mng );
		 
		   int result = SAT_Solve(mng);
		  
		   if (result == SATISFIABLE){ //sat: consistent with avoids
				 			//cout<<"SAT"<<endl;
		 			return 1;
     		}else{                      //unsat: inconsistent with avoids
			 			//cout<<"UNSAT"<<endl;
			    	return 0;
			}

			 
}

int CA_SAT::filterFinal( vector < vector <struct avoid_unit> > & avoidbeforeCustomered, const vector<colVal>& partialTC){
	//cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
	//print(avoidbeforeCustomered,partialTC); // print before customed
	//factor level

	int size=partialTC.size();
	int avoidsize=avoidbeforeCustomered.size();

	vector <struct avoid_unit> onerowAvoid;
	string tmpstr="";
	struct avoid_unit one_unit;
	int eraseRow=0;
	int allextraFactor=1;

	vector<vector <struct avoid_unit > >::iterator the_iterator=avoidbeforeCustomered.begin();
	
	for(int i=0;i<avoidsize;i++){
		
		onerowAvoid=avoidbeforeCustomered.at(i);
		
		//begin to justify if such avoid following two cusomed rules: 1. size 2. all are Exactly involed factors
		eraseRow=0;
		allextraFactor=1;
		if(onerowAvoid.size()<=size){
			
  			for (int j=0; j<onerowAvoid.size();j++){  //check the unit one by one
				one_unit=onerowAvoid.at(j);
  				
				colVal tmp;

				for(int a=0;a<partialTC.size();a++){

					tmp=partialTC.at(a);

					int column=atoi(one_unit.column.c_str()) ;

					if(column==tmp.col){
						allextraFactor=0;
						break;
					}
				}//EndFor
				if(!allextraFactor){
					break;
				}
					
			} //EndFor

			if(allextraFactor){ //include all EXTRA factor, remove such avoid 
						avoidbeforeCustomered.erase(the_iterator);
						the_iterator++;
						avoidsize--;
						i--; 
						eraseRow=1;
						break; // if row is erased, so check next row
			}else{//such factor is included in the partial TC, so check next factor
						
			}

			if(!eraseRow) the_iterator++;
			

		}else{ //onerowAvoid.size()>size, so it should be deleted
			avoidbeforeCustomered.erase(the_iterator);
			avoidsize--;
			i--; //?
			the_iterator++;
		
		}// EndIf 
 		
	}

	
	
//	print(avoidbeforeCustomered,partialTC);  //print avoids after customed 
//	cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;

}



int CA_SAT::filterFinal_SymbolLevel( vector < vector <struct avoid_unit> > & avoidbeforeCustomered, const vector<colVal>& partialTC){
	//Symbol Level


	//cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
	//print(avoidbeforeCustomered,partialTC); // print before customed

	int size=partialTC.size();
	int avoidsize=avoidbeforeCustomered.size();

	vector <struct avoid_unit> onerowAvoid;
	string tmpstr="";
	struct avoid_unit one_unit;
	int eraseRow=0;
	int allextraFactor=1;

	vector<vector <struct avoid_unit > >::iterator the_iterator=avoidbeforeCustomered.begin();
	
	for(int i=0;i<avoidsize;i++){
		
		onerowAvoid=avoidbeforeCustomered.at(i);
		
		//begin to justify if such avoid following two cusomed rules: 1. size 2. all are Exactly involed factors
		eraseRow=0;
		allextraFactor=1;
		if(onerowAvoid.size()<=size){ //Size Filter
			
  			for (int j=0; j<onerowAvoid.size();j++){  //check the unit one by one
				one_unit=onerowAvoid.at(j);
  				
				colVal tmp;

				for(int a=0;a<partialTC.size();a++){

					tmp=partialTC.at(a);

					int column=atoi(one_unit.column.c_str()) ;

					if(column==tmp.col){ //factor level: constraint factor = partial test case factor
						int value=atoi(one_unit.value.c_str()) ;

						if(value==tmp.val){ // symbol level constriant factor::value= partial test case factor::value 
							allextraFactor=0;
							break;
						}
						
					}
				}//EndFor
				if(!allextraFactor){
					break;
				}
					
			} //EndFor

			if(allextraFactor){ //include all EXTRA factor, remove such avoid 
						avoidbeforeCustomered.erase(the_iterator);
						the_iterator++;
						avoidsize--;
						i--; 
						eraseRow=1;
						break; // if row is erased, so check next row
			}else{//such factor is included in the partial TC, so check next factor
						
			}

			if(!eraseRow) the_iterator++;
			

		}else{ //onerowAvoid.size()>size, so it should be deleted
			avoidbeforeCustomered.erase(the_iterator);
			avoidsize--;
			i--; //?
			the_iterator++;
		
		}// EndIf 
 		
	}

	
	
//	print(avoidbeforeCustomered,partialTC);  //print avoids after customed 
//	cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;

}


int CA_SAT::isSAT(const vector<colVal>& t_set, int k, int* karray, 
				  float& construtTime, float& construtTime_usec, 
				  float& resolveTime,float& resolveTime_usec,
				  float& construt1Time, float& construt1Time_usec,
				  float& construt2Time, float& construt2Time_usec
							  
				  ){
		
	 /*
	 		construt1Time=initMng_Time
			construt2Time=addTestCase_Time



	 */
			struct rusage before,after; //tmporary before and after
			struct rusage before2,after2; //for total sat solving time
			struct rusage before1,after1; //for total construct time

			//begin to compute resolving time

/**************************************/
			allCNF_before.clear();
			allCNF_after.clear();

			getrusage(RUSAGE_SELF, &before1);  // for all 

			getrusage(RUSAGE_SELF, &before);

			if(mng!=NULL){
				SAT_ReleaseManager(mng);
	    	}

			mng = SAT_InitManager();
			addAtleastAvoids(_atleastAvoids, mng);
			
			getrusage(RUSAGE_SELF, &after); 
			construt1Time=construt1Time+((float)after.ru_utime.tv_sec - (float)before.ru_utime.tv_sec);
			construt1Time_usec=construt1Time_usec+ ( (float)after.ru_utime.tv_usec - (float) before.ru_utime.tv_usec);

	
/**************************************/
			getrusage(RUSAGE_SELF, &before); 

	 	    createVectorCNF_to_zChaff(t_set); //write the partial test case into zChaff mng which has already include at least and customed constraints

			getrusage(RUSAGE_SELF, &after); 

			construt2Time=construt2Time+((float)after.ru_utime.tv_sec - (float)before.ru_utime.tv_sec);
			construt2Time_usec=construt2Time_usec+ ( (float)after.ru_utime.tv_usec - (float) before.ru_utime.tv_usec);

/**************************************/ //the total construct time

			getrusage(RUSAGE_SELF, &after1); 
			construtTime=construtTime+((float)after1.ru_utime.tv_sec - (float)before1.ru_utime.tv_sec);
			construtTime_usec=construtTime_usec+ ( (float)after1.ru_utime.tv_usec - (float) before1.ru_utime.tv_usec);

		
	   	//begin to compute resolving time
/**************************************/

			getrusage(RUSAGE_SELF, &before2); 
	
		   int result = SAT_Solve(mng);

		   getrusage(RUSAGE_SELF, &after2); 
		   resolveTime=resolveTime+((float)after2.ru_utime.tv_sec - (float)before2.ru_utime.tv_sec);
		   resolveTime_usec=resolveTime_usec+ ( (float)after2.ru_utime.tv_usec - (float) before2.ru_utime.tv_usec);


		   /**************************************/
		
		   if (result == SATISFIABLE){ //sat: consistent with avoids
		 			return 1;

     		}else{                      //unsat: inconsistent with avoids
			 	
			    	return 0;
			}

			 
}




int CA_SAT::isSAT(const vector<colVal>& t_set, int k, int* karray){
		
	 /*
	 		input: test t-set (pairs,triples,...)
	 				v=g
	 				
	 				
	 		output: 1-> consistent with avoids
	 		        0-> inconsistent with avoids
	 */
	 		finalClause=clause;  // so far, finalClause has all at_least, at_most and constraints. Only waiting for test case

			//vector < vector <struct avoid_unit> > tmp=all_avoids;

			//filterFinal_SymbolLevel(tmp,t_set);
			
			//write_avoids(k,karray,tmp); //write the customed constriants to clause which has already include at least and at most there

	 	    createVectorCNF(t_set); //write the partial test case into clause which has already include at least and at most and customed constraints
	 		
			//cout<<"+++++++++++++++++++++++++++++++++++++++++++"<<endl;
			//printavoids();
			//cout<<"+++++++++++++++++++++++++++++++++++++++++++"<<endl;
			//exit(0);

		   //writeToFile("satmodel.cnf");
		   transSAT_Zchaff(k,karray);
     	   //writeToFile("zchaffmodel.cnf");
			
		   if(mng!=NULL){
				SAT_ReleaseManager(mng);
		   }
			mng = SAT_InitManager();
		    read_cnf (mng );
			//read_cnf( mng, "zchaffmodel.cnf");
		 
		   int result = SAT_Solve(mng);
		  
		   if (result == SATISFIABLE){ //sat: consistent with avoids
				 			//cout<<"SAT"<<endl;
		 			return 1;
     		}else{                      //unsat: inconsistent with avoids
			 			//cout<<"UNSAT"<<endl;
			    	return 0;
			}

}


int CA_SAT::isSAT(int testvector[], int k, int g){
	 /*
	 		input: test vector string
	 				col is an array storing the column infor corresponding to testvector
	 				
	 		output: 1-> consistent with avoids
	 		        0-> inconsistent with avoids
	 */
	 		
			finalClause=clause;// so far, finalClause has all at_least, at_most and constraints. Only waiting for test case


			createVectorCNF(testvector,k);
			//writeToFile("satmodel.cnf");

   		    transSAT_Zchaff(k,g);

			//writeToFile("zchaffmodel.cnf");
	   
			if(mng!=NULL){
				SAT_ReleaseManager(mng);
		    }
			
			mng = SAT_InitManager();
		    read_cnf (mng);
		 
		   int result = SAT_Solve(mng);
			
			 if (result == SATISFIABLE){ //sat: consistent with avoids
				 			//cout<<"SAT"<<endl;
				 			return 1;
			 }else{                      //unsat: inconsistent with avoids
			 			//cout<<"UNSAT"<<endl;
						return 0;
			 }
			 
}
/**vector as the test case*********************/

void CA_SAT::createVectorCNF_vector(const vector<int>& testvector, int k){
	
	/*input: clause vector with only AVOIDS CNFs
	  output: clause vector with AVOIDS and Vector CNFs
	
	  String vector format: 
	  										  0:0:1:0:...:k-1:g:e 
	                          	 
   */
	
	char str_value[MAX_SIZE],str_column[MAX_SIZE];
	
	string clausestr="";

	for(int i=0;i<testvector.size();i++){

		clausestr="";
		sprintf(str_value,"%d",testvector.at(i));
		sprintf(str_column,"%d",i);
		clausestr=clausestr+"m_"+str_column+"_"+str_value;
		string newstr=clausestr;
		finalClause.push_back(clausestr);
		
	}
	
				
}

/******************Vector as a test case *************************/
int CA_SAT::isSAT_vector(const vector<int>& testvector, int k, int g){
 		
			finalClause=clause;// so far, finalClause has all at_least, at_most and constraints. Only waiting for test case


			createVectorCNF_vector(testvector,k);
			//writeToFile("satmodel.cnf");
  		    transSAT_Zchaff(k,g);
		   
			 //writeToFile("zchaffmodel.cnf");
		    	 
			 if(mng!=NULL){
				SAT_ReleaseManager(mng);
			   }
			
			 mng = SAT_InitManager();
			 read_cnf (mng );
		 
		   int result = SAT_Solve(mng);
			
			 if (result == SATISFIABLE){ //sat: consistent with avoids
				 			//cout<<"SAT"<<endl;
				 			return 1;
			 }else{                      //unsat: inconsistent with avoids
			 			//cout<<"UNSAT"<<endl;
						return 0;
			 }
}

void CA_SAT::extractAllTsetConstraints(const char* avoidFile,int t){
	/*
		Function: collect all t-set explicit constraints to one vector < vector<int> > 
		input: constraintFile, t 
	*/
	
	allTsetConstraints.clear();

	/*Loop all lines of constraint */

	char buffer[MAX_AVOIDS_LENGTH];
    ifstream avoids (avoidFile);

	 if (! avoids.is_open())
	 { cout << "Error opening file"; exit (1); }

	while (! avoids.eof() )
	{
		avoids.getline (buffer,MAX_AVOIDS_LENGTH); // read one line avoids
		
		if(buffer[0]=='e'){
			break;
		}

		/*write one line of constraint to allTsetConstraints vector */
		
		vector<struct avoid_unit> oneavoid;
			
		int i=0;
		string word_column="", word_value="";
		
		while(buffer[i]!='e'){

				word_column="";
				word_value="";
				while(buffer[i]!=':'){ //collect one word for column
    				word_column=word_column+buffer[i];
						i++;
  			}
				i++; //jump ":"
					
				while(buffer[i]!=':'){ //collect one word for value
    				word_value=word_value+buffer[i];
    				i++;
  			}
				i++;//jump ":"
				struct avoid_unit oneunit;
				oneunit.column=word_column;
				oneunit.value=word_value;
				oneavoid.push_back(oneunit);
		}
		
		if(oneavoid.size()==t){
			allTsetConstraints.push_back(oneavoid);
		}
		
	}

	//debug
	/*cout<<"--------"<<allTsetConstraints.size()<<" tset explicit constraints--------"<<endl;
	struct avoid_unit oneunit;
	vector<struct avoid_unit> tmpv;
	for(int i=0;i<allTsetConstraints.size();i++){
		tmpv=allTsetConstraints.at(i);
		for(int j=0;j<tmpv.size();j++){
			oneunit=tmpv.at(j);
			cout<<oneunit.column<<" "<< oneunit.value<<"  ";
		}
		cout<<endl;
	}
	*/
	
}

void CA_SAT::extractAllTsetConstraints(int t){
	/*
		Function: collect all t-set explicit constraints to one vector < vector<int> > 
		input: constraintFile, t 
	*/
	allTsetConstraints.clear();

	/*Loop all lines of constraint */
	vector <struct avoid_unit> onerowAvoid;
	for(int i=0;i<all_avoids.size();i++){
		onerowAvoid=all_avoids.at(i);
  		if(onerowAvoid.size()==t){
			allTsetConstraints.push_back(onerowAvoid);
		}																				
	}

	//debug
	/*cout<<"--------"<<allTsetConstraints.size()<<" tset explicit constraints--------"<<endl;
	struct avoid_unit oneunit;
	vector<struct avoid_unit> tmpv;
	for(int i=0;i<allTsetConstraints.size();i++){
		tmpv=allTsetConstraints.at(i);
		for(int j=0;j<tmpv.size();j++){
			oneunit=tmpv.at(j);
			cout<<oneunit.column<<" "<< oneunit.value<<"  ";
		}
		cout<<endl;
	}*/
}

int CA_SAT::isImplicit(vector<string> & tset){

   /*
		function: justify if tset is an implicit t-set

		input: tset vector, constraint set (implicit input)

		output: 1(yes) 0(no)
		
		limitation: 
			1.	Constraint file must be expressed from small factors to big factors
				for example, 0:1:2:2:e not 2:2:0:1:e
			2.	constraint generated from LexSuccess follows such order.  

		Basing on this, we can compare these two vectors by comparing each cell one by one.

	*/
	//debug
    



	int found;

	vector<struct avoid_unit> oneavoid;

	//char col[MAX_WORD_LENGTH];

	for(int i=0;i<allTsetConstraints.size();i++){
	
		oneavoid=allTsetConstraints.at(i);
		//assert(oneavoid.size()==tset.size());
		found=1;
		for(int j=0;j<oneavoid.size();j++){ //t comparison

			//transform oneavoid avoid_unit to int int mode

			string col=oneavoid.at(j).column;
			string value=oneavoid.at(j).value;
			string col2=tset.at(2*j);
			string value2=tset.at(2*j+1);

			//cout<<"explicit:"<<col<<":"<<value<<" Implicit: "<<col2<<":"<<value2<<endl;

			if((col.compare(col2)!=0) || (value.compare(value2)!=0)){ //compare:0 match, 1 not-match
				//match=>found
				found=0;	
				break;
			}
		}
		if(found)
			break;
		
	}
     	
	

	if(found){ // non implicit
		return 0;
	}else{ //implicit
		return 1;
	}
}

void CA_SAT::extractAllFactors(const char* avoidFile, int k){
	/*
		Function: collect all t-set explicit constraints to one vector < vector<int> > 
		input: constraintFile, t 
	*/
	factorArray=new int[k];

	for(int i=0;i<k;i++){
		factorArray[i]=0;// 0: not-involved, 1:involved
	}
	int index;	
	//allInvolvedFactors.clear();

	/*Loop all lines of constraint */
	
	char buffer[MAX_AVOIDS_LENGTH];
    ifstream avoids (avoidFile);

	 if (! avoids.is_open())
	 { cout << "Error opening file"; exit (1); }

	while (! avoids.eof() )
	{
		avoids.getline (buffer,MAX_AVOIDS_LENGTH); // read one line avoids
		
		if(buffer[0]=='e'){
			break;
		}

		/*write one line of constraint to allTsetConstraints vector */
		
		vector<struct avoid_unit> oneavoid;
			
		int i=0;
		string word_column="", word_value="";
		
		while(buffer[i]!='e'){

				word_column="";
				word_value="";
				while(buffer[i]!=':'){ //collect one word for column
    				word_column=word_column+buffer[i];
						i++;
  			}
				i++; //jump ":"
					
				while(buffer[i]!=':'){ //collect one word for value
    				word_value=word_value+buffer[i];
    				i++;
  			}
				i++;//jump ":"
				index=atoi(word_column.c_str());
				factorArray[index]=1;
				allInvolvedFactors.insert(word_column);
		}
		
	}

	//debug
	/*cout<<"---Factors------"<<endl;
	set<string>::iterator pos;
    for (pos = allInvolvedFactors.begin(); pos != allInvolvedFactors.end(); ++pos) {
		cout<<*pos<<" ";
    }
	cout<<endl;*/
	
	
}



void CA_SAT::extractAllFactors(int k){
	/*
		Function: collect all t-set explicit constraints to one vector < vector<int> > 
		input: constraintFile, t 
	*/
	factorArray=new int[k];
	for(int i=0;i<k;i++){
		factorArray[i]=0;// 0: not-involved, 1:involved
	}

	//allInvolvedFactors.clear();

	/*Loop all lines of constraint */
	vector <struct avoid_unit> onerowAvoid;
	struct avoid_unit one_unit;
	int index;

	for(int i=0;i<all_avoids.size();i++){
		onerowAvoid=all_avoids.at(i);
  		for (int j=0; j<onerowAvoid.size();j++){
			one_unit=onerowAvoid.at(j);
			index=atoi(one_unit.column.c_str());
			factorArray[index]=1;
			allInvolvedFactors.insert(one_unit.column);
  		}																						
	}

	

	//debug
	/*cout<<"---Factors------"<<endl;
	set<string>::iterator pos;
    for (pos = allInvolvedFactors.begin(); pos != allInvolvedFactors.end(); ++pos) {
		cout<<*pos<<" ";
    }
	cout<<endl;*/
	
	
}

int CA_SAT::isInvolvedFactor(int factor){

   /*
		function: justify if f is an related factor 
   */
	
	return factorArray[factor];
}

int CA_SAT::isInvolvedFactor(vector<int> factorvec){

   /*
		function: justify if f is an related factor 
   */
	int involved=0;
	for(int i=0;i<factorvec.size();i++){
		if(factorArray[factorvec[i]]==1){
			involved=1;
			break;
		}
	}
	
	return involved;
}


void CA_SAT::transfromConstraints(const char* avoidFile,int v){
	/*
		Function: transform constraints from one mode to another mode

		input:  1:0:2:0:e
				e
		output:
		       0(0,1) 1(2,3) 2(4,5)
			   2,4
	*/
	
	allTransformedConstraints.clear();

	/*Loop all lines of constraint */

	char buffer[MAX_AVOIDS_LENGTH];
    ifstream avoids (avoidFile);

	 if (! avoids.is_open())
	 { cout << "Error opening file"; exit (1); }

	while (! avoids.eof() )
	{
		avoids.getline (buffer,MAX_AVOIDS_LENGTH); // read one line avoids
		
		if(buffer[0]=='e'){
			break;
		}

		/*write one line of constraint to allTsetConstraints vector */
		
		vector<int> oneavoid;
			
		int i=0;
		char word_column[20], word_value[20];
		
		while(buffer[i]!='e'){

				//word_column="";
				//word_value="";
			    int index1=0;
				while(buffer[i]!=':'){ //collect one word for column
    				word_column[index1]=buffer[i];
						i++;
						index1++;
  				}
				word_column[index1]='\0';
				int length=i;

				i++; //jump ":"
				int index2=0;	
				while(buffer[i]!=':'){ //collect one word for value
    				word_value[index2]=buffer[i];
    				i++;
					index2++;
  				}
				word_value[index2]='\0';
				i++;//jump ":"

				int col=atoi(word_column);
				int value=atoi(word_value);
				
				//print out all constraints transformation with col:value mode one by one
				//cout<<t<<" "<<word_column<<":"<<word_value<<"  "<<col*v+value<<endl;

				oneavoid.push_back(col*v+value);

		}

		allTransformedConstraints.push_back(oneavoid);
		
		
	}

	

	//debug
	/*cout<<"--------"<<allTsetConstraints.size()<<" tset explicit constraints--------"<<endl;
	struct avoid_unit oneunit;
	vector<struct avoid_unit> tmpv;
	for(int i=0;i<allTsetConstraints.size();i++){
		tmpv=allTsetConstraints.at(i);
		for(int j=0;j<tmpv.size();j++){
			oneunit=tmpv.at(j);
			cout<<oneunit.column<<" "<< oneunit.value<<"  ";
		}
		cout<<endl;
	}*/

	
}


void CA_SAT::transfromConstraints(const char* avoidFile){
	/*
		Function: transform constraints from one mode to another mode

		input:  1:0:2:0:e
				e
		output:
		       0(0,1) 1(2,3) 2(4,5)
			   2,4
	*/
	
	allTransformedConstraints.clear();

	/*Loop all lines of constraint */

	char buffer[MAX_AVOIDS_LENGTH];
    ifstream avoids (avoidFile);

	 if (! avoids.is_open())
	 { cout << "Error opening file"; exit (1); }

	while (! avoids.eof() )
	{
		avoids.getline (buffer,MAX_AVOIDS_LENGTH); // read one line avoids
		
		if(buffer[0]=='e'){
			break;
		}

		/*write one line of constraint to allTsetConstraints vector */
		
		vector<int> oneavoid;
			
		int i=0;
		char word_column[20], word_value[20];
		
		while(buffer[i]!='e'){

				//word_column="";
				//word_value="";
			    int index1=0;
				while(buffer[i]!=':'){ //collect one word for column
    				word_column[index1]=buffer[i];
						i++;
						index1++;
  				}
				word_column[index1]='\0';
				int length=i;

				i++; //jump ":"
				int index2=0;	
				while(buffer[i]!=':'){ //collect one word for value
    				word_value[index2]=buffer[i];
    				i++;
					index2++;
  				}
				word_value[index2]='\0';
				i++;//jump ":"

				int col=atoi(word_column);
				int value=atoi(word_value);
				
				//print out all constraints transformation with col:value mode one by one
				//cout<<t<<" "<<word_column<<":"<<word_value<<"  "<<col*v+value<<endl;
				
				int transformedValue=getSym(col,value);
				oneavoid.push_back(transformedValue);

				//oneavoid.push_back(col*v+value);

		}

		allTransformedConstraints.push_back(oneavoid);
		
		
	}

	

	//debug
	/*cout<<"--------"<<allTsetConstraints.size()<<" tset explicit constraints--------"<<endl;
	struct avoid_unit oneunit;
	vector<struct avoid_unit> tmpv;
	for(int i=0;i<allTsetConstraints.size();i++){
		tmpv=allTsetConstraints.at(i);
		for(int j=0;j<tmpv.size();j++){
			oneunit=tmpv.at(j);
			cout<<oneunit.column<<" "<< oneunit.value<<"  ";
		}
		cout<<endl;
	}*/

	
}

/*int main(int argc, char *argv[]) {
		//test driver for isSAT()
		//string updated_testvector="18:0:19:0:e";
		
		int k,g;		
		const char* avoidfile;
		int updated_testvector[3];
		
		if (argc==4){
		  	k=atoi(argv[1]);
			g=atoi(argv[2]);
			avoidfile=argv[3];
			//updated_testvector=argv[4];
					
		}else{
			printf("Invalid parameters\n");
			printf("k g avoidfile,updated_testvector\n");
		   return 0;
		}
	
		updated_testvector[0]=0;
		updated_testvector[1]=2;
		updated_testvector[2]=0;

		CA_SAT* casat=new CA_SAT();
		casat->initAndCreateAvoidsCNF(avoidfile,k,g);
	  casat->isSAT(updated_testvector, k, g);
	
}*/

