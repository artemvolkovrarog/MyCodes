#pragma once
#include<iostream>

template<class T>
class MyAllocator{
	std::size_t AllocatedSize;
public:

	T* allocate(std::size_t n){
		AllocatedSize = n;
		T* AllocatedMemory = static_cast<T*>(::operator new[](n * sizeof(T)));
		return AllocatedMemory;
	}

	T* deallocate(T* p, std::size_t n){
		if (n != AllocatedSize) throw "Error of deallocation";
		else{
			if (p != nullptr) delete[]p;
			p = nullptr;
		}
	}
	

	void construct(T* p, const T& val){
		std::cout << "CONSTRUCT\n";
		new(p) T(val);
	}

	template<class... ArgS>
	void construct(T* p, ArgS&&... args){
		std::cout << "CONSTRUCT &&\n";
		new(p) T(std::forward<ArgS>(args)...);
	}

	void destroy(T* p){
		((T*)p) -> ~T();
	}
};