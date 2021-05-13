#pragma once
#include <iostream>
#include <sstream>

enum class Error{
	NoError,
	CorruptedArchive,
	OtherType
};

struct data_correct{
	uint64_t a;
	bool b;
	bool c;
	uint64_t d;
	template <class Serializer>
	Error serialize(Serializer& serializer){
		return serializer(a,b,c,d);
	}
	data_correct(uint64_t, bool, bool, uint64_t);
};

struct data_alt{
	uint64_t a;
	uint64_t b;
	bool c;
	template <class Serializer>
	Error serialize(Serializer& serializer){
		return serializer(a,b,c);
	}
	data_alt( uint64_t, uint64_t, bool);
};

struct data_false_type{
	uint64_t a;
	std::string b;
	bool c;
	template <class Serializer>
	Error serialize(Serializer& serializer){
		return serializer(a,b,c);
	}
	data_false_type(uint64_t, const std::string&, bool);
};

class Serializer{
	private:
		std::ostream& outp;
		static constexpr char Serialize_sep = ' ';

		Error ser_outp(uint64_t);
		Error ser_outp(bool);
		template<class T>
		Error ser_outp(const T& arg){ return Error::OtherType;};

		template<class First>
		Error process(const First& arg){
			Error er = ser_outp(arg);
			if (er == Error::OtherType) return er;
			return Error::NoError;
		}

		template <class First, class... ArgT>
		Error process(const First& arg,  ArgT&&... args){
			Error er = ser_outp(arg);
			if (er == Error::OtherType) return er;
			outp << Serialize_sep; 
			return process(std::forward<ArgT>(args)...);
		}

	public:
		explicit Serializer(std::ostream& out): outp(out){
		}
		template <class T>
		Error save(T& object){
			return object.serialize(*this);
		}

		template<class... ArgT>
		Error operator()( ArgT&&... args){
			return process(std::forward<ArgT>(args)...);
		}
};

class Deserializer{
	private:
		std::istream& inp;

		Error ser_inp(uint64_t&);
		Error ser_inp(bool&);

		template<class ArgT>
		Error ser_inp(const ArgT& arg){return Error::OtherType;}

		template<class First>
		Error process(First& arg){
			Error er = ser_inp(arg);
			if (er == Error::CorruptedArchive || er == Error::OtherType) return er;
			return Error::NoError;
		}

		template<class First, class... ArgT>
		Error process(First& arg, ArgT&&... args){
			Error er = ser_inp(arg);
			if (er == Error::CorruptedArchive || er == Error::OtherType) return er;
			return process(std::forward<ArgT>(args)...);
		}

	public:
		explicit Deserializer(std::istream& in): inp(in){}
		
		template<class T>
		Error load(T& object){
			return object.serialize(*this);
		}

		template<class... ArgT>
		Error operator()(ArgT&&...args){
			return process(std::forward<ArgT>(args)...);
		}
};