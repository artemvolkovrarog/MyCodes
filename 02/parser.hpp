#include <iostream>
#include <stack>
#include<fstream>
#include <functional>
#pragma once

std::string start(const std::string& );
std::string start_alternative(const std::string& );
void finish();
void find(const std::string&);
void stringhandler(const std::string&, std::stack<std::string>&);
void digithandler(const std::string&, std::stack<std::string>&);


using DigitMaintain = std::function<void(const std::string &, std::stack<std::string> &)>;
using StringMaintain = std::function <void (const std::string &, std::stack<std::string> &)>;
using StartCallback = std::function <std::string(const std::string &)>;
using FindCallback = std::function <void(const std::string &)>;
using FinishCallback = std::function <void()>;


class TokenParser{
	DigitMaintain digprs;
	StringMaintain strprs;
	StartCallback start;
	FindCallback find;
	FinishCallback finish;
	std::ifstream file;
	std::string token;

	public:
		std::stack <std::string> check_parse; 
		std::stack <std::string> check_callbacks;
		TokenParser(){
			digprs= nullptr;
			strprs = nullptr;
			start = nullptr;
			find = nullptr;
			finish = nullptr;		
		}
		void SetDigitCallback(DigitMaintain);
		void SetStringCallback(StringMaintain);
		void SetStartCallback(StartCallback);
		void SetFindCallback(FindCallback);
		void SetFinishCallback(FinishCallback);
		void Parser(const std::string & s = "INPFILE");

};