#include <iostream>
#include "parser.hpp"
#include <stack>
#include<fstream>

std::string start(const std::string& st){
	std::cout << "Hello, let's parse!" << std::endl;
	if (st == "INPFILE"){
		std::cout << "Please, enter the name of input - file" << std::endl;
		std::string s;
		std::cin >> s;
		return s;
	}
	else return st;
}

void finish(){
	std::cout << "Parsing is finished" << std::endl << std::endl;
}

void find(const std::string & s){
	std :: cout << "New token was found !" << std::endl << "The type if token - " << s << std::endl;
}

void stringhandler(const std::string & s, std :: stack<std::string> & check_callbacks){
	std::cout << "String - token: " << s << std::endl;
	check_callbacks.push("str");
}

void digithandler(const std::string & s,std::stack<std::string> & check_callbacks){
	std::cout << "Int - token: " << stoi(s) << std::endl;
	check_callbacks.push("dig");
}

std::string start_alternative(const std::string& st){
	return("NOFILE");
}

void TokenParser::SetDigitCallback(DigitMaintain  dig){
	digprs = dig;
}

void TokenParser::SetStringCallback(StringMaintain st){
	strprs = st;
}

void TokenParser::SetStartCallback(StartCallback stcb){
	start = stcb;
}
void TokenParser::SetFindCallback(FindCallback fcb){
	find = fcb;
}
void TokenParser::SetFinishCallback(FinishCallback fscb){
	finish = fscb;
}

bool check_dig(const std::string & s){
	bool dig = true;
	for (int i = 0; i < s.length(); i++){
		if (!isdigit(s[i])){
			dig = false;
			break;
		}
	}
	return dig;
}


void TokenParser::Parser(const std::string & s) {
	if ((strprs == nullptr) || (start == nullptr) || (digprs == nullptr) || (find == nullptr) || (finish == nullptr)){
		throw "Error, one or several funcion(-s) was(-were)n't set";
	}
	std :: string filename;
	filename = start(s);
	file.open(filename);

	if (!file.is_open()){
		throw ("Error while opening the file");
	}
	while(file >> token){
		if (check_dig(token)){
			find("digit");
			digprs(token, check_callbacks);
			token.clear();
		} 
		else{
			find("string");
			strprs(token, check_callbacks);
			token.clear();
		}
	}
	finish();
	file.close();
}