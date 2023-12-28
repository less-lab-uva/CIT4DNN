#include "solve_smt.h"

CA_SMT::CA_SMT(int latent_dims, int no_parts, char *d_range) {
	strcpy(density_range, d_range);
	k = latent_dims;
	no_bins = no_parts;
	
	s = new solver(c);
	set_param("pp.decimal", true); // set decimal notation
	
	//set partitions structure
	partitions = new float*[no_parts];
	ifstream parts_file;
	if (*density_range == '\0')
		parts_file.open("../../Config/partitions_z"+to_string(k)+"_p"+to_string(no_bins)+".txt");
	else
		parts_file.open("../../Config/partitions_z"+to_string(k)+"_p"+to_string(no_bins)+"_"+density_range+".txt");
		
	string line;
	int i = 0;
	string st;
	while(getline(parts_file, line)) {
		partitions[i] = new float[2];
		int j = 0;
		stringstream ss(line);
		while(getline(ss, st, ' ')) {
			partitions[i][j] = stof(st);
			j++;
		}
		i++;
	}
	parts_file.close();
		
	//read radin and radout from a file
	ifstream bounds_file;
	if (*density_range == '\0')
		bounds_file.open("../../Config/boundaries_z"+to_string(k)+"_p"+to_string(no_bins)+".txt");
	else	
		bounds_file.open("../../Config/boundaries_z"+to_string(k)+"_p"+to_string(no_bins)+"_"+density_range+"_i.txt");
	getline(bounds_file, line);
	radin = stof(line);
	getline(bounds_file, line);
	radout = stof(line);
	cout << "radin " << radin << " radout " << radout << endl;
	bounds_file.close();
	if (*density_range != '\0'){
		bounds_file.open("../../Config/boundaries_z"+to_string(k)+"_p"+to_string(no_bins)+"_"+density_range+"_o.txt");
		getline(bounds_file, line);
		radin2 = stof(line);
		getline(bounds_file, line);
		radout2 = stof(line);
		cout << "radin2 " << radin2 << " radout2 " << radout2 << endl;
		bounds_file.close();
	}
}

CA_SMT::~CA_SMT() {
	set_param("pp.decimal", false); // set decimal notation
}

void CA_SMT::setup_distance_constraints() {
	
	for (int i=0; i<k; i++) {
		string name = "z_"+to_string(i);
		Z.push_back(c.real_const(name.c_str()));
	}
	
	expr partmax_sq = c.real_val(to_string(radout*radout).c_str());
	expr partmin_sq = c.real_val(to_string(radin*radin).c_str());
	
	//initializing below with dummy values
	expr partmax_sq2 = c.real_val(to_string(0).c_str());
	expr partmin_sq2 = c.real_val(to_string(0).c_str());
		
	if (*density_range != '\0'){
		partmax_sq2 = c.real_val(to_string(radout2*radout2).c_str());
		partmin_sq2 = c.real_val(to_string(radin2*radin2).c_str());
	}
	
	for (int i=0; i<k; i++) (*s).add(Z[i] >= 0); //lowerbound
	if (*density_range != '\0')
		for (int i=0; i<k; i++) (*s).add(Z[i] < partmax_sq2);//upperbound
	else
		for (int i=0; i<k; i++) (*s).add(Z[i] < partmax_sq);//upperbound
	//checkSAT(s);
	
    expr zsum = c.real_val("0");
    for (int i=0; i<k; i++) zsum = zsum + Z[i];
	

	if (*density_range != '\0'){
		//(*s).add((zsum >= partmin_sq, zsum < partmax_sq) || (zsum >= partmin_sq2, zsum < partmax_sq2));
		(*s).add((zsum >= partmin_sq && zsum < partmax_sq) || (zsum >= partmin_sq2 && zsum < partmax_sq2));
	} else {
		(*s).add(zsum >= partmin_sq);
		(*s).add(zsum < partmax_sq);
	}
	//checkSAT(s);
}



void CA_SMT::add_constraints(vector<int> tc) {
	for (int i=0; i < tc.size(); i++) {
		//cout << partitions[tc[i]][0] << " " << partitions[tc[i]][1] << endl;
		expr min = c.real_val(to_string(partitions[tc[i]][0]*partitions[tc[i]][0]).c_str());
		expr max = c.real_val(to_string(partitions[tc[i]][1]*partitions[tc[i]][1]).c_str());
		if (partitions[tc[i]][0] < 0) {
			(*s).add(Z[i] <= min);
			(*s).add(Z[i] >  max);
		} else {
			//cout << partitions[tc[i]][0]*partitions[tc[i]][0] << " " << partitions[tc[i]][1]*partitions[tc[i]][1] << endl;
			(*s).add(Z[i] >= min);
			(*s).add(Z[i] <  max);
		}
	}
}


void CA_SMT::add_constraints(int tc, int i) {
	//cout << partitions[tc][0] << " " << partitions[tc][1] << endl;
	expr min = c.real_val(to_string(partitions[tc][0]*partitions[tc][0]).c_str());
	expr max = c.real_val(to_string(partitions[tc][1]*partitions[tc][1]).c_str());
	if (partitions[tc][0] < 0) {
		(*s).add(Z[i] <= min);
		(*s).add(Z[i] >  max);
	} else {
		//cout << partitions[tc][0]*partitions[tc][0] << " " << partitions[tc][1]*partitions[tc][1] << endl;
		(*s).add(Z[i] >= min);
		(*s).add(Z[i] <  max);
	}
}


int CA_SMT::checkSAT(bool showModel) {
	if ((*s).check() == sat) {
		if (showModel) {
			model m = (*s).get_model();
			cout << m << "\n";
		}
		return 1;
	}else {
		return 0;
	}
}
