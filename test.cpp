#include <iostream>
#include <fstream>

#include "lexer.h"

int main(void)
{
	auto out = naruto::naruto_lexize_file("fib.uwu");	
	
	for(auto item : out)
	{
		std::cout << "------------" << std::endl << 
		"Obj type: " << item.code << std::endl << 
		"Obj info: " << item.info << std::endl << 
		"Int val: " << item.int_val << std::endl << 
		"Flt val: " << item.float_val << std::endl;
	}

	return 0;
}
