#pragma once
#include <iostream>
#include "my_vec_alloc.hpp"
#include "my_vec_iter.hpp"
template <class T, class Allocator = MyAllocator<T>>
class MyVec{

public:

	std::size_t Capacity;
	std::size_t MaxSize;
	std::size_t VSize;
	Allocator alloc;
	T* AllocatedMemory;

	MyVec(std::size_t ms = 100, std::size_t cp = 10){
		Capacity = cp;
		MaxSize = ms;
		VSize = 0;
		alloc = Allocator();
		AllocatedMemory = alloc.allocate(Capacity);}


	MyVec(const MyVec<T, Allocator> & v ){
		alloc = v.alloc;
		Capacity = v.capacity();
		MaxSize = v.MaxSize;
		VSize = v.VSize;
		AllocatedMemory = alloc.allocate(Capacity);
		for (std::size_t i = 0; i < v.VSize; i++){
			(*this).push_back((v.AllocatedMemory)[i]);
		}

	}

	MyVec(MyVec<T, Allocator> && v){
		alloc = std::move(v.alloc);
		Capacity = std::move(v.Capacity);
		MaxSize = std::move(v.MaxSize);
		VSize = std::move(v.VSize);
		AllocatedMemory = v.AllocatedMemory;
		v.AllocatedMemory = nullptr;
	}

	MyVec<T> & operator =(const MyVec<T, MyAllocator<T>> & v){
		while(VSize > 0){
			alloc.destroy(&(AllocatedMemory[--VSize]));
		}

		if (AllocatedMemory != nullptr){
			::operator delete[] (AllocatedMemory);
		}

		alloc = v.alloc;
		Capacity = v.capacity();
		MaxSize = v.MaxSize;
		VSize = 0;
		AllocatedMemory = alloc.allocate(Capacity);
		for (std::size_t i = 0; i < VSize; i++){
			(*this).push_back(v.AllocatedMemory[i]);
		}
		return *this;

	}

	MyVec<T> & operator =(MyVec<T, MyAllocator<T>> && v){
		while(VSize > 0){
			alloc.destroy(&(AllocatedMemory[--VSize]));
		}

		if (AllocatedMemory != nullptr){
			::operator delete[] (AllocatedMemory);
		}

		alloc = v.alloc;
		Capacity = v.capacity();
		MaxSize = v.MaxSize;
		VSize = v.size();
		AllocatedMemory = v.AllocatedMemory;
		v.VSize = 0;
		v.AllocatedMemory = nullptr;
		return *this;
	}

	T& operator[](std::size_t n){
		if (n >= VSize){
			throw "Index is out of bounds";
		}
		else return AllocatedMemory[n];
	}

	void push_back(T&& val){
		if (VSize >= Capacity){
			(*this).reserve(Capacity + 5);
		}
		alloc.construct(&(AllocatedMemory[VSize++]), std::move(val));
	}

	void push_back(const T& val){
		if (VSize >= Capacity){
			(*this).reserve(Capacity + 5);
		}
		alloc.construct(&(AllocatedMemory[VSize++]), val);
	}


	template <class... ArgS>
	void emplace_back(ArgS && ... args){
		if (VSize >= Capacity){
			(*this).reserve(Capacity + 5);
		}
		alloc.construct(&(AllocatedMemory[VSize++]), args...);
	}

	void pop_back(){
		if (VSize == 0) throw "Vector is empty!";
		else {
			alloc.destroy(&(AllocatedMemory[--VSize]));
		}
	}

	bool empty()const{
		return (VSize == 0);
	}

	std::size_t size()const{
		return VSize;
	}

	void clear(){
		while (VSize > 0) {
			(*this).pop_back();
			--VSize;
		}
	}

	void resize(std::size_t count, const T& val){
		if (count > VSize){
			if (count > Capacity) {
				if (count > MaxSize) throw "Size for resize exceeds limits";
				(*this).reserve(count);
			}

			for (uint i = 0; i < count - VSize; i++){
				alloc.construct(&(AllocatedMemory[VSize++]),val);
			}
		}
		else if (count < VSize){
			for (uint i = 0; i < VSize - count; i++){
				alloc.destroy(&(AllocatedMemory[--VSize]));
			}
			VSize = count;
		}
	}

	void reserve(std::size_t n){
		if (n > MaxSize) throw "Reserve size exceeds limits";
		if (n > Capacity){
			T* new_area = (T*)::operator new[](n * sizeof(T));
			for (uint i = 0; i < VSize; i++){
				new_area[i] = std::move(AllocatedMemory[i]);
			}
			delete[]AllocatedMemory;
			AllocatedMemory = new_area;
			Capacity = n;
		}
	}

	std::size_t capacity()const{
		return Capacity;
	}

	Iterator<T> begin(){
		return Iterator<T>(AllocatedMemory);
	}

	Iterator<T> end(){
		return Iterator<T>(AllocatedMemory + VSize);
	}

	ReversedIterator<T> rbegin(){
		return ReversedIterator<T>(AllocatedMemory + VSize -1);
	}

	ReversedIterator<T> rend(){
		return ReversedIterator<T>(AllocatedMemory -1);
	}

	~MyVec(){
		while(VSize > 0){
			alloc.destroy(&(AllocatedMemory[--VSize]));
		}
		if (AllocatedMemory != nullptr){
			::operator delete[] (AllocatedMemory);
		}
	}
};