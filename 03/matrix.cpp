#include <iostream>
#include "matrix.hpp"

int* & row::get_ptr(){
	return row_ptr;
}
void row :: initialize (int row_size){
	row_ptr = new int [row_size];
	if (row_ptr == nullptr){
		throw ("Error while allocating memory");
	}
	for (int i = 0; i < row_size; i++) row_ptr[i] = 0;
	size_of_row = row_size;
}

int & row :: operator[](int pos){
	if (pos >= size_of_row){
		throw ("Index of columns is out of range"); 
	} 
	else return row_ptr[pos];
}
const int & row :: operator[](int pos)const{
	if (pos >= size_of_row){
		throw ("Index of columns is out of range"); 
	} 
	else return row_ptr[pos];
}

matrix::matrix(){
	rows_num = 0;
	cols_num = 0;
	sub_matrix = nullptr;
}
void matrix::init(int  rows_num, int  cols_num){
	sub_matrix = new row[rows_num];
	if (sub_matrix == nullptr) {
		throw("Error while allocating memory");
	}
	for (int i=0; i<rows_num; i++){
		sub_matrix[i].initialize(cols_num);
	}
}

row & matrix::operator [](int  pos){
	if (pos >= rows_num){
		throw ("Index of rows is out of range");
	}
	else return sub_matrix[pos];
}
const row & matrix::operator [](int pos)const{
	if (pos >= rows_num){
		throw ("Index of rows is out of range");
	}
	else return sub_matrix[pos];
}

matrix::~matrix(){
	if (sub_matrix != nullptr){
		for (int i = 0; i < rows_num; i++){
			if (sub_matrix[i].get_ptr() != nullptr) delete[]sub_matrix[i].get_ptr();
		}
		delete[]sub_matrix;
	}
}
matrix::matrix(const matrix & M){
	rows_num = M.get_rows();
	cols_num = M.get_cols();
	(*this).init(rows_num, cols_num);
	for (int i = 0; i < rows_num; i++){
		for (int j = 0; j < cols_num; j++){
			(*this)[i][j] = M[i][j];
		}
	}
}

matrix :: matrix (int rows,int cols){
	rows_num = rows;
	cols_num = cols;
	(*this).init(rows_num, cols_num);
}


int matrix::get_rows()const{
	return rows_num;
} 
int matrix::get_cols()const{
	return cols_num;
}

std::ostream & operator << (std::ostream & out, const matrix & M ){
	for (int i = 0; i < M.get_rows(); i++){
		for (int j =0; j < M.get_cols(); j++){
			out << M[i][j] << ' ';
		}
		out << std::endl;
	}
	return out;
}

matrix & matrix:: operator *= (int multiplier){
	for (int i = 0; i < rows_num; i++){
		for (int j = 0; j < cols_num; j++){
			(*this)[i][j] *= multiplier;
		}
	}
	return (*this);
}

bool matrix::operator == (const matrix & M) const{
	bool fl = true;
	if (((*this).get_rows() != M.get_rows()) || ((*this).get_cols() != M.get_cols())){
		throw("Error, matrices have different size");
	}
	else{
		for (int i = 0; i < (*this).get_rows(); i++){
			for (int j = 0; j < (*this).get_cols(); j++){
				if ((*this)[i][j] != M[i][j]){
					fl = false;
					break;
				}
				if (!fl) break;
			}
		}
	}
	return fl;
}

matrix matrix::operator + (const matrix & M) const{
	if (((*this).get_rows() != M.get_rows()) || ((*this).get_cols() != M.get_cols())){
		throw("Error, matrices have different size");
	}
	else{
		matrix Res((*this).get_rows(), (*this).get_cols());
		for (int i = 0; i < (*this).get_rows(); i++){
			for (int j = 0; j < (*this).get_cols(); j++){
				Res[i][j] = (*this)[i][j] + M[i][j];
			}
		}
		return Res;
	}
}

bool matrix::operator !=(const matrix & M) const{
	return !(*this == M);
}

matrix & matrix::operator =(const matrix & M){
	if (((*this).get_cols() == M.get_cols()) && ((*this).get_rows() == M.get_rows())){
		if ((*this) == M){
			return *this;
		}
	}
	if (sub_matrix != nullptr){
		for (int i = 0; i < rows_num; i++){
			if (sub_matrix[i].get_ptr() != nullptr) delete[]sub_matrix[i].get_ptr();
		}
		delete[]sub_matrix;
	}
	rows_num = M.get_rows();
	cols_num = M.get_cols();
	(*this).init(rows_num, cols_num);
	for (int i = 0; i < rows_num; i++){
		for (int j = 0; j < cols_num; j++){
			(*this)[i][j] = M[i][j];
		}
	}
	return (*this);

}

