#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace naruto
{
	enum TokenCodes
	{
		identifier = 0,
		int_val,
		float_val,
		no_jutsu,
		//yosh, //treated as ints
		//iee,
		sayonara,
		chan,
		nani,
		baka,
		suki,
		wa,
		desu,
		//desu_ga, are binary operators
		//to,
		bin_op,
		paren_open,
		paren_close,
		newline
	};
	
	struct Lex
	{
		Lex(int c, std::string i, long l, double d) : 
		code(c), 
		info(i), 
		int_val(l), 
		float_val(d) 
		{}
		
		int code;
		std::string info;
		long int_val;
		double float_val;
	};
	
	std::vector<std::string> split(std::string input);
	void naruto_lexize(std::string input, std::vector<Lex> & lexes);
	std::vector<Lex> naruto_lexize_file(std::string input_file);
}
