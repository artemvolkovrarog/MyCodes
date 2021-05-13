#include <iostream>
#include "serializer.hpp"
#include <sstream>
data_correct::data_correct(uint64_t a1, bool b1, bool c1, uint64_t d1){
	a = a1;
	b = b1;
	c = c1;
	d = d1;
}

data_alt::data_alt(uint64_t a1, uint64_t b1, bool c1){
	a = a1;
	b = b1;
	c = c1;
}

data_false_type::data_false_type(uint64_t a1, const std::string& b1, bool c1){
	a = a1;
	b = b1;
	c = c1;
}


Error Serializer::ser_outp(uint64_t arg){
	outp << arg;
	return Error::NoError;
}

Error Serializer::ser_outp(bool arg){
	if (arg){
		outp << "true";
	}
	else{
		outp << "false";
	}
	return Error::NoError;
}

Error Deserializer::ser_inp(bool& fl){
	std::string s;
	inp >> s;
	if (s == "true") fl = true;
	else if (s == "false") fl = false;
	else return Error::CorruptedArchive;
	return Error::NoError;
}

Error Deserializer::ser_inp(uint64_t& num){
	std::string s;
	inp >> s;
	try{
		num = stoull(s);
	}
	catch(const std::logic_error& e){
		return Error::CorruptedArchive;
	}
	return Error::NoError;
}
