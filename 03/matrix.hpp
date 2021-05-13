#pragma once
#include <iostream>


class row{
		int* row_ptr;
		int size_of_row;
	public:
		row(){
			row_ptr = nullptr;
			size_of_row = 0;
		};
		void initialize(int);
		int & operator[](int);
		const int & operator[](int) const;
		int* & get_ptr();
};


class matrix{

	private:
		int rows_num, cols_num;
		row * sub_matrix;
	public:
		matrix();
		matrix (int, int);
		matrix(const matrix &);
		row& operator[](int);
		const row& operator[](int) const;
		matrix & operator *= (int);
		int get_rows () const;
		int get_cols () const;
		void init(int, int);
		matrix operator+(const matrix &)const;
		bool operator ==(const matrix &)const;
		bool operator !=(const matrix &)const;
		matrix & operator =(const matrix &);
		~matrix();
};

std::ostream & operator << (std::ostream &, const matrix &);