#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/time.h>   // needed for timing
#include <sys/resource.h>   // needed for timing

using namespace std;

void methodRand(){
	cout<<"methd rand:"<<rand()<<endl;
}

int main(int argc, char **argv) {

 int randSeed;
 randSeed=(int (time(NULL))); // reseed random #   
 srand(randSeed);
 cout<<"main rand:"<<rand()<<endl;
 methodRand();
 return 0;

}







