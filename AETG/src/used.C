#include "used.h"
#include "assert.h"



ostream & operator<<(ostream & os, const Used &uv){

  os << "least: " << uv.least << "countleast " << uv.count[uv.least];
  return os;



}

void Used::Clear2(Cover & cv) {
 

  // this is used in Repeat loop for LeastUsed refreshing itself because of Covered and KVcombo recomputations. 

  v=cv.getV();

  k=cv.getK();

  // find the maximum size from kcombo
  m=0; // initialize this before looking for max
  // after this m will be the largest number of times something is used

  for(int i=0; i<k;i++){
    if(cv.getkCombo(i)>m)
      m=cv.getkCombo(i);
  }
			  
			

  //least=m;  // last row is least used

  /*symbols=new int*[m+1];
  for(int i=0; i<=m; i++){
    symbols[i]=new int[v];
  }*/


	//initialize symbol[][]=0
  for(int i=0 ; i<=m ; i++)
    for(int j=0; j<v ; j++)
      symbols[i][j]=0;


  //count=new int[m+1];
  // initialize the values in symbol
   for(int i=0; i<=m; i++)  // all are -1 
    count[i]=-1;  

  //symcount=new int[v];
	//initialize the values in symbcount
  for(int j=0; j<v ; j++)
      symcount[j]=-1;
	//debug	
 // for(int j=0; j<v ; j++)
	//	 cout<<symcount[j];
	//	cout<<endl;

  //symindex=new int[v];
  //initialize the values in symindex
	 for(int j=0; j<v ; j++)
      symindex[j]=-1;

	 //debug
		//for(int j=0; j<v ; j++)
		// cout<<symindex[j];
		//cout<<endl;
 

  int x;
  int kcomb;

   //int vcount[m+1];
   //for(int c=0; c<=m; c++)
   //   vcount[c]=0;
   vector<int> vcount;
   vcount.assign(m+1,0);
   
   int index_vcount=0;

  for (int i=0; i< k; i++){
    x=cv.getVsize(i);
    //kcomb=cv.getkCombo(i);
		
	for(int b=0; b<x; b++){
		//change here
	  
		kcomb=cv.getkvCombo(i,b);

      symbols[kcomb][vcount[kcomb]]=cv.getSym(i,b);
	   //debug
	   //cout<<"factor:"<<i<<" No. "<<b <<"'s value="<<cv.getSym(i,b)<<endl;
	   //cout<<"symbol["<<kcomb<<"]["<<vcount[kcomb]<<"]="<<symbols[kcomb][vcount[kcomb]]<<endl;

      symcount[cv.getSym(i,b)]=kcomb;
      //symindex[cv.getSym(i,b)]=vcount[kcomb];
	  symindex[cv.getSym(i,b)]=vcount[kcomb];
      vcount[kcomb]=vcount[kcomb]+1;
	  index_vcount++;
    }
     //count[kcomb]=count[kcomb]+x;
  }
//copy vcount to count except 0
 
  for(int c=0; c<=m; c++){
	  if(vcount[c]!=0){
		count[c]=vcount[c]-1;
	  }else {
		count[c]=-1;
	  }
			
  }


  //pick the occurence number of least symbols 
  int tmpleast=symcount[0];
  for(int c=1; c<k; c++)
	  if(tmpleast<symcount[c])
			tmpleast=symcount[c];

  least=tmpleast;
 
  //debug 

  /*cout<<"least"<<least<<" "<<endl;

   //debug
  for(int i=0; i<=m; i++)
	  cout << "  " << count[i] ;
  cout << endl;

  // debugging
  //symcount
  cout<<"symcount"<<" ";
	for(int c=0; c<v; c++)
      cout<<symcount[c]<<" " ;
   cout<<endl;

  //symindex
    cout<<"symindex"<<" ";
	 for(int c=0; c<v; c++)
      cout<<symindex[c]<<" " ;
   cout<<endl;
  //vcount
    cout<<"vcount"<<" ";
   for(int c=0; c<=m; c++)
      cout<<vcount[c]<<" " ;	
   cout<<endl;
	// symbols
  cout<<"symbols[][]"<<" "<<endl;
  for(int i=0 ; i<=m ; i++){
    for(int j=0; j<v ; j++)
      cout <<symbols[i][j] << "  " ;
  cout << endl;
  }
  */
 
}



void Used::Clear(Cover &cv){

	//pick the occurence number of least symbols 
  int tmpleast=symcount[0];
  for(int c=1; c<k; c++)
	  if(tmpleast<symcount[c])
			tmpleast=symcount[c];

  least=tmpleast;

  
	
  for(int i=0; i<=m; i++)
    count[i]=-1;

  // initialize the rows


  // initialize the values in symbol

  int x;
  int kcomb;

   //int vcount[m+1];
   //for(int c=0; c<=m; c++)
   //   vcount[c]=0;
   vector<int> vcount;
   vcount.assign(m+1,0);
   
	//changed by Jiangfan Shi	
   int index_vcount;
	index_vcount=0;

  for (int i=0; i< k; i++){
    x=cv.getVsize(i);
    //kcomb=cv.getkCombo(i);
		
	for(int b=0; b<x; b++){
		//change here
	  
		kcomb=cv.getkvCombo(i,b);

      symbols[kcomb][vcount[kcomb]]=cv.getSym(i,b);
	   //debug
	   //cout<<"factor:"<<i<<" No. "<<b <<"'s value="<<cv.getSym(i,b)<<endl;
	   //cout<<"symbol["<<kcomb<<"]["<<vcount[kcomb]<<"]="<<symbols[kcomb][vcount[kcomb]]<<endl;

      symcount[cv.getSym(i,b)]=kcomb;
      //symindex[cv.getSym(i,b)]=vcount[kcomb];
	  symindex[cv.getSym(i,b)]=vcount[kcomb];
      vcount[kcomb]=vcount[kcomb]+1;
	  //vcount[kcomb]=index_vcount;
	  index_vcount++;
    }
     count[kcomb]=count[kcomb]+x;
  }

  //copy vcount to count
  for(int c=0; c<=m; c++)
	   if(vcount[c]!=0)
			count[c]=vcount[c]-1;

/*
  for (int i=0; i< k; i++){
    x=cv.getVsize(i);

    kcomb=cv.getkCombo(i);
    
    for(int b=0; b<x; b++){
      symbols[kcomb][vcount[kcomb]]=cv.getSym(i,b);
      symcount[cv.getSym(i,b)]=kcomb;
      symindex[cv.getSym(i,b)]=vcount[kcomb];
      vcount[kcomb]=vcount[kcomb]+1;
    }
     count[kcomb]=count[kcomb]+x;

  }
*/

/*cout<<"symcount"<<" ";
	for(int c=0; c<v; c++)
      cout<<symcount[c]<<" " ;
   cout<<endl;

  //symindex
    cout<<"symindex"<<" ";
	 for(int c=0; c<v; c++)
      cout<<symindex[c]<<" " ;
   cout<<endl;
  //vcount
    cout<<"vcount"<<" ";
   for(int c=0; c<=m; c++)
      cout<<vcount[c]<<" " ;	
   cout<<endl;
	// symbols
  cout<<"symbols[][]"<<" "<<endl;
  for(int i=0 ; i<=m ; i++){
    for(int j=0; j<v ; j++)
      cout <<symbols[i][j] << "  " ;
  cout << endl;
  }*/

}




Used::Used(const Used & us){


  v=us.v;
  m=us.m;
  least=us.least;

  symbols=new int*[m+1];
  for(int i=0; i<=m; i++){
    symbols[i]=new int[v];
  }
  for(int i=0; i<=m; i++)
    for(int j=0; j<v; j++)
      symbols[i][j]=us.symbols[i][j];

   count=new int[m+1];
  for(int i=0; i<= m; i++)  // all others are zero
    count[i]=us.count[i]; 


   symcount=new int[v];

  for(int i=0; i< v; i++)
    symcount[i]=us.symcount[i];  
  symindex=new int[v];
  for(int i=0; i< v; i++)
    symindex[i]=us.symindex[i];   


}



int Used::pickLeastUsed()const{

// find the current least used row and then randomly pick from it
  int n;
  int x;

  
  //debug
 //for(int i=0; i<=m; i++)
//	  cout << "  " << count[i] ;
 //  cout << endl;

  n=count[least]+1;  // need to add one since count is a column 0 based
  //cout << " least[count]:  " << count[least] << endl;
  
  assert(n>0);

  x=(rand()%n);
  // get symbol in col x of least used symbols
  //  cout << "symbol chosen " << '\n' << x << " " << symbols[least][x] << endl ;
  return symbols[least][x];

}

Used & Used::operator=(const Used &us){

  // first check to see if you are assinging to oneself
  // then delete all memory


 if(this != & us){

 delete [] count;

 for(int i=0; i<= m; i++)
   delete [] symbols[i];
 delete [] symbols;
 delete []symcount;
 delete []symindex;
 // make new memory and copy 

  v=us.v;
  m=us.m;
  least=us.least;

  symbols=new int*[m+1];
  for(int i=0; i<= m; i++){
    symbols[i]=new int[v];
  }

  for(int i=0; i<= m; i++)
    for(int j=0; j<v; j++)
      symbols[i][j]=us.symbols[i][j];

   count=new int[m+1];
  for(int i=0; i<= m; i++)  
    count[i]=us.count[i]; 

   symcount=new int[v];

  for(int i=0; i< v; i++)
    symcount[i]=us.symcount[i];  
  symindex=new int[v];
  for(int i=0; i< v; i++)
    symindex[i]=us.symindex[i];   


 }    
  return *this;
}


void Used::updateUsed(int val){

  // first move the symbol to the end of the previous row

  int row;
  int col;
  row=symcount[val];
  col=symindex[val];
  //  cout << " col row val " << col << " " << row << " " << val<< endl;
  int newrowcount;

  int curcount=count[row];

  // change the original row by removing this symbol, and put last one over there

  if( col != curcount){
    // if this is not the last element
    // replace this with last element

   symbols[row][col]=symbols[row][curcount];
   // update its col
   symindex[symbols[row][col]]=col;

   } 
  curcount--;
	
  //update least row if applicable
  if((curcount==-1 && least==row)) // last one removed
    {
      least=row-1;
    }
  
  //
  count[row]=curcount;
  
  // find the next row's last possible poistion
  newrowcount=count[row-1]+1;
  //update count array (last position in such pairs number)
  count[row-1]=newrowcount;
  //move such symbol to the last position in such new row

  symbols[row-1][newrowcount]=val;

  // now adjust row and column for such val 
  symcount[val]=row-1;;
  symindex[val]=newrowcount;

  //cout << "printing symbols M:" <<m <<  endl;

   //debugging
  //symcount
  /*cout<<"symcount"<<" ";
	for(int c=0; c<v; c++)
      cout<<symcount[c]<<" " ;
   cout<<endl;

  //symindex
    cout<<"symindex"<<" ";
	 for(int c=0; c<v; c++)
      cout<<symindex[c]<<" " ;
   cout<<endl;
  //vcount
    cout<<"count"<<" ";
   for(int c=0; c<=m; c++)
      cout<<count[c]<<" " ;	
   cout<<endl;
	// symbols

  for(int i=0 ; i<=m ; i++){
    for(int j=0; j<v ; j++)
      cout << symbols[i][j] << " " ;
  cout << endl;
  }
  
*/
    
}



Used::Used(Cover & cv) {
 
  /* need to figure out how many times each symbol is involved in a pair.
     Then need to create the matrix from this. 
  */
	//cout << "DEBUG In USED .. " << endl;
  v=cv.getV();

  k=cv.getK();

  //find the maximum size from kcombo
  m=0; // initialize this before looking for max
  // after this m will be the largest number of times something is used
	
  for(int i=0; i<k;i++){
    if(cv.getkCombo(i)>m)
      m=cv.getkCombo(i);
	  //cout << "DEBUG Loop " << m << endl;
  }


  //least=m;  // last row is least used
	//cout << "Debug after loop" << endl;
	
  symbols=new int*[m+1];
  for(int i=0; i<=m; i++){
    symbols[i]=new int[v];
  }
  //cout << "Debug issue here 1 " << endl;
	//initialize symbol[][]=0
  for(int i=0 ; i<=m ; i++)
    for(int j=0; j<v ; j++)
      symbols[i][j]=0;
	//cout << "Debug issue here 2" << endl;

  count=new int[m+1];
  //cout << "Debug issue here 3" << endl;
  // initialize the values in symbol
   for(int i=0; i<=m; i++)  // all are -1 
    count[i]=-1;  
//cout << "Debug issue here 4 " << endl;
  symcount=new int[v];
  //cout << "Debug issue here 5 " << endl;
	//initialize the values in symbcount
  for(int j=0; j<v ; j++)
      symcount[j]=-1;
  //cout << "Debug issue here 6 " << endl;
	//debug	
 // for(int j=0; j<v ; j++)
	//	 cout<<symcount[j];
	//	cout<<endl;

  symindex=new int[v];
  //cout << "Debug issue here 7 " << endl;
  //initialize the values in symindex
	 for(int j=0; j<v ; j++)
      symindex[j]=-1;

	 //debug
		//for(int j=0; j<v ; j++)
		// cout<<symindex[j];
		//cout<<endl;
 

  int x; // levels for factors
  int kcomb;
	//cout << "Debug issue here 8 " << endl;
   //int vcount[m+1];
   vector<int> vcount;
   //cout << "DEBUG " << m << endl;
   vcount.assign(m+1,0);
   //cout << "DEBUG after vcount assign" << m << endl;
   //for(int c=0; c<=m; c++)
      //vcount[c]=0;

   int index_vcount=0;

  for (int i=0; i< k; i++){
    x=cv.getVsize(i);
    //kcomb=cv.getkCombo(i);
		
	for(int b=0; b<x; b++){
		//change here
	  
		kcomb=cv.getkvCombo(i,b); //kcombo=# of t-set for such symbol(i,b)
			

      symbols[kcomb][vcount[kcomb]]=cv.getSym(i,b);
	   //debug
	   //cout<<"factor:"<<i<<" No. "<<b <<"'s value="<<cv.getSym(i,b)<<endl;
	   //cout<<"symbol["<<kcomb<<"]["<<vcount[kcomb]<<"]="<<symbols[kcomb][vcount[kcomb]]<<endl;

      symcount[cv.getSym(i,b)]=kcomb; //row position in symbols[][] for symbol 
      //symindex[cv.getSym(i,b)]=vcount[kcomb];
	  symindex[cv.getSym(i,b)]=vcount[kcomb]; //column position in symbols[][] for symbol 
      vcount[kcomb]=vcount[kcomb]+1; //# of symbols for each row in symbols[][], row =[0,max#oftset involving one symbols of all symbols]
	  index_vcount++;
    }
     //count[kcomb]=count[kcomb]+x;
  }
//copy vcount to count except 0
 
  for(int c=0; c<=m; c++){
	  if(vcount[c]!=0){
		count[c]=vcount[c]-1;
	  }else {
		count[c]=-1;
	  }
			
  }
	  
		


  //pick the occurence number of least symbols 
  int tmpleast=symcount[0];
  for(int c=1; c<k; c++)
	  if(tmpleast<symcount[c])
			tmpleast=symcount[c];

  least=tmpleast;

 /* cout<<"least"<<least<<" "<<endl;

   //debug
  for(int i=0; i<=m; i++)
	  cout << "  " << count[i] ;
  cout << endl;

  // debugging
  //symcount
  cout<<"symcount"<<" ";
	for(int c=0; c<v; c++)
      cout<<symcount[c]<<" " ;
   cout<<endl;

  //symindex
    cout<<"symindex"<<" ";
	 for(int c=0; c<v; c++)
      cout<<symindex[c]<<" " ;
   cout<<endl;
  //vcount
    cout<<"vcount"<<" ";
   for(int c=0; c<=m; c++)
      cout<<vcount[c]<<" " ;	
   cout<<endl;
	// symbols
  cout<<"symbols[][]"<<" "<<endl;
  for(int i=0 ; i<=m ; i++){
    for(int j=0; j<v ; j++)
      cout <<symbols[i][j] << "  " ;
  cout << endl;
  }*/
 
}



Used::~Used(){
  
  for (int i=0; i<=m; i++)
    delete [] symbols[i];

  delete [] symbols;
  delete [] count;
  delete [] symcount;
  delete [] symindex;  
}














