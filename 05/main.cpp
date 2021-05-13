#include <iostream>
#include <cassert>
#include "serializer.hpp"
#include <sstream>

void test_correct_alt(){
	data_correct x { 1, true, false, 10423251 };
	data_alt x1 {1241, 12414, false};

	std::stringstream stream;
	std::stringstream stream1;

	Serializer serializer(stream);
	Serializer serializer1(stream1);
	serializer1.save(x1);
	serializer.save(x);

	data_correct y { 0, false, false, 0 };
	data_alt y1 {0, 0, false};

	Deserializer deserializer(stream);
	Deserializer deserializer1(stream1);

	const Error err1 =  deserializer1.load(y1);
	const Error err =  deserializer.load(y);


	assert(err == Error::NoError && err1 == Error::NoError);
	assert(x.a == y.a && x1.a == y1.a);
	assert(x.b == y.b && x1.b == y1.b);
	assert(x.c == y.c && x1.c == y1.c);
	assert(x.d = y.d);
}

void test_false_or_othertype(){
	data_correct x {1, true, false, 124112313};
	data_false_type x1 {12312, "ft", false};
	std::stringstream stream;
	std::stringstream stream1;
	stream << "$!@#";

	Serializer serializer(stream);
	Serializer serializer1(stream1);
	const Error err1 = serializer1.save(x1);
	serializer.save(x);

	data_correct y {0, false, false, 0};
	Deserializer deserializer(stream);
	const Error err = deserializer.load(y);
	assert(err1 == Error::OtherType && err == Error::CorruptedArchive);

}


int main(){
	test_correct_alt();// тест для проверки корректности работы и инвариантности формы данных
	test_false_or_othertype();// тест для проверки реакции на неподдерживаемый тип данных или порчу данных в потоке
	std::cout << "All tests were passed!" << std::endl;
	return 0;
}