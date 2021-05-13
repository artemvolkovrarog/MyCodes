#include <iostream>
#pragma once
class Allocator{
	char* head;
	int offset;
	int block_size ;
	bool check_alloc;
	public:
		Allocator(){
			check_alloc = false;
			head = nullptr;
			offset = 0;
			block_size = 0;
		}
		void makeallocator(std::size_t max_size);
		char* alloc(std::size_t size);
		void reset();
		char* gethead(){
			return head;
		}
		int getofs(){
			return offset;
		}
		~Allocator(){
			if (head != nullptr) delete []head;
		}
};
