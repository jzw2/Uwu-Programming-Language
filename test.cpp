#include <iostream>
#include <fstream>

#include "lexer.h"

std::string type_to_string(int code)
{
	switch(code)
	{	
	case naruto::TokenCodes::identifier:
		return "Identifier";
	case naruto::TokenCodes::int_val:
		return "Int Val";
	case naruto::TokenCodes::float_val:
		return "Float Val";
	case naruto::TokenCodes::no_jutsu:
		return "No Jutsu";
	case naruto::TokenCodes::sayonara:
		return "Sayonara";
	case naruto::TokenCodes::chan:
		return "Chan";
	case naruto::TokenCodes::nani:
		return "Nani";
	case naruto::TokenCodes::baka:
		return "Baka";
	case naruto::TokenCodes::suki:
		return "Suki";
	case naruto::TokenCodes::wa:
		return "Wa";
	case naruto::TokenCodes::desu:
		return "Desu";
	case naruto::TokenCodes::bin_op:
		return "Binary Operator";
	case naruto::TokenCodes::paren_open:
		return "(";
	case naruto::TokenCodes::paren_close:
		return ")";
	case naruto::TokenCodes::newline:
		return "Newline";
	}
}

int main(void)
{
	auto out = naruto::naruto_lexize_file("fib.uwu");	
	
	for(auto item : out)
	{
		std::cout << "------------" << std::endl << 
		"Obj type: " << type_to_string(item.code) << std::endl << 
		"Obj info: '" << item.info << "'" << std::endl << 
		"Int val: " << item.int_val << std::endl << 
		"Flt val: " << item.float_val << std::endl;
	}

	return 0;
}
