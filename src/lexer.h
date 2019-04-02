#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#define EMP_STR std::string("")

namespace naruto
{
	enum TokenCodes
	{
		identifier = 0, //[a-Z][0-9a-Z]*
		int_val, //(\+|\-)?[0-9]+
		float_val, //(\+|\-)[0-9]+\.[0-9]+
		string_val,
		no_jutsu, //no jutsu
		//yosh, //treated as ints //yosh
		//iee, //iee
		sayonara, //sayonara
		chan, //chan
		san, //int type
		kun, //float type
		sama, //string type
		senpai,
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
		delim, //~ 
		new_line, //\n
		fn_delim, //!!
		main_fn_delim, //~!
		while_delim, //~~
		if_delim, //~?
		//lambda_thread_delim //!!
	};
	
	struct Lex
	{
		Lex(int c, std::string i, long l, double d) : 
		code(c), 
		info(i), 
		int_val(l), 
		float_val(d) 
		{}
		
		Lex(int c) : 
		code(c), 
		info(EMP_STR), 
		int_val(0), 
		float_val(0) 
		{}

		Lex(int c, std::string i) : 
		code(c), 
		info(i), 
		int_val(0), 
		float_val(0) 
		{}

		bool isKeyword();
		bool isIden();
		bool isVal();
		bool isSayonara();
		bool isNoJutsu();
		bool isChan();
		bool isSenpai();
		bool isSama();
		bool isKun();
		bool isSan();
		bool isBaka();
		bool isNani();
		bool isDoki();
		bool isWa();
		bool isDesu();
		bool isOp();
		bool isParenOpen();
		bool isParenClose();
		bool isDelim();
		bool isFloatVal();
		bool isIntVal();
		bool isStrVal();
		bool isColon();
		bool isNewline();
		bool isFnDelim();
		bool isMainFnDelim();
		bool isWhileDelim();
		bool isIfDelim();
		bool isThreadDelim();
		bool isShadowCloneJutsu();

		int code;
		std::string info;
		long int_val;
		double float_val;
	};
	
	std::vector<std::string> split(std::string input);
	void naruto_lexize(std::string input, std::vector<Lex> & lexes);
	std::vector<Lex> naruto_lexize_file(std::string input_file);
}
