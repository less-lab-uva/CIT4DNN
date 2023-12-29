#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <z3++.h>

using namespace std;
using namespace z3;

class CA_SMT{
	float radin, radout; //inner and outer radius of shell1, i.e., first input of density range
	float radin2, radout2; //inner and outer radius of shell2, i.e., second input of density range
	float **partitions; //this contains the boudaries of the intervals across each latent dimension
	char density_range[50];
	int k; //latent dimensions
	int no_bins; //number of partitions across each latent dimension
	
	//z3 solver
	context c;
	vector<expr> Z;
	solver *s;
	
	public:
	CA_SMT(int latent_dims, int no_parts, char *density_range);
	~CA_SMT();
	void setup_distance_constraints();
	void add_constraints(vector<int> tc);
	void add_constraints(int tc, int index);
	void addScope() {(*s).push();} ;
	void clearScope() {(*s).pop();} ;
	int checkSAT(bool showModel);
};
