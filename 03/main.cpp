#include <iostream>
#include "matrix.hpp"
#include <cassert>
#include <fstream>

matrix make_mat(int rows_num, int cols_num, int multiplier = 1){
	matrix M(rows_num, cols_num);
	for (int i = 0; i < rows_num; i++){
		for (int j = 0; j < cols_num; j++){
			M[i][j] = i * j * multiplier;
		}
	}
	return M;
}
void check_equality_non_equality(){
	matrix M1_eq = make_mat(21,31), M2_eq = make_mat(21,31);
	matrix M1_neq = make_mat(20,10), M2_neq = make_mat(20,10,3);
	assert((M1_eq == M2_eq) && (M1_neq != M2_neq));
}
void check_get_cols_and_rows(){
	matrix M(20,10);
	assert((M.get_cols() == 10) && (M.get_rows() == 20));
}

void check_get_element(){
	matrix M = make_mat(20,10);
	assert(M[19][9] == 171);
}

void check_multiplication(){
	matrix M1 = make_mat(10,10), M2 = make_mat(10,10,2);
	M1*=2;
	assert(M1 == M2);
}

void check_output(){
	std::ofstream f_outp;
	std::ifstream f_inp;
	int num;
	bool check;
	f_outp.open("check.txt");
	matrix M = make_mat(10,10);
	f_outp << M;
	f_outp.close();
	f_inp.open("check.txt");
	for (int i = 0; i < M.get_rows(); i++){
		for (int j = 0; j < M.get_cols(); j++){
			f_inp >> num;
			check = (num == M[i][j]);
			if (!check) break;
		}
		if (!check) break;
	}
	f_inp.close();
	assert(check);

}

void check_sum(){
	matrix M1 = make_mat(20,10), M2 = make_mat(20,10,2), M3 = M1 + M2, M_check = make_mat(20,10,3);
	bool check = (M3 == M_check);
	assert(check);

}

void assignment_check(){
	matrix M1(make_mat(20,10)), M2(make_mat(20,10,2)), M3(make_mat(20,10,3));
	matrix M_test(10,10);
	M_test = M1 + M2;
	assert(M_test == M3);
}

int main(){
	try{
		check_equality_non_equality();
		check_get_cols_and_rows();
		check_get_element();
		check_multiplication();
		check_output();
		check_sum();
		assignment_check();
	}
	catch(const char* s){
		std::cout << s << std::endl;
	}
	std::cout << "All tests were passed!" << std::endl;
}