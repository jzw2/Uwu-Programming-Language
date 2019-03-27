#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace naruto
{
	enum TokenCodes
	{
		identifier = 0, //[a-Z][0-9a-Z]*
		int_val, //(\+|\-)?[0-9]+
		float_val, //(\+|\-)[0-9]+\.[0-9]+
		no_jutsu, //no jutsu
		//yosh, //treated as ints //yosh
		//iee, //iee
		sayonara, //sayonara
		chan, //chan
		nani, //nani
		baka, //baka
		doki, //doki
		wa, //wa
		desu, //desu
		shadow_clone_jutsu, //shadow clone jutsu
		//desu_ga, are binary operators //desu ga
		//to, //to
		bin_op, //this is anything that does not fit in another category
		paren_open, //\(
		paren_close, //\)
		colon, //:
		new_line, //\n
		delim, //~
		fn_delim, //!!
		main_fn_delim, //~!
		while_delim, //~~
		if_delim, //~?
		lambda_thread_delim //!!
	};
	
	struct Lex
	{
		Lex(int c, std::string i, long l, double d) : 
		code(c), 
		info(i), 
		int_val(l), 
		float_val(d) 
		{}
		
		bool isKeyword();
		bool isIden();
		bool isOp();
		bool isParenOpen();
		bool isParenClose();
		bool isDelim();
		bool isVal();
		bool isColon();

		int code;
		std::string info;
		long int_val;
		double float_val;
	};
	
	std::vector<std::string> split(std::string input);
	void naruto_lexize(std::string input, std::vector<Lex> & lexes);
	std::vector<Lex> naruto_lexize_file(std::string input_file);
}
