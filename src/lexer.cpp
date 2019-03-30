#include <cctype>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#include "lexer.h"


namespace naruto
{	
	bool Lex::isKeyword()
	{
		return 
		code == TokenCodes::chan |
		code == TokenCodes::sayonara |
		code == TokenCodes::no_jutsu |
		code == TokenCodes::nani |
		code == TokenCodes::baka |
		code == TokenCodes::doki |
		code == TokenCodes::wa |
		code == TokenCodes::desu;
	}
	
	bool Lex::isIden() { return code == TokenCodes::identifier; }
	bool Lex::isOp() { return code == TokenCodes::bin_op; }
	bool Lex::isParenOpen() { return code == TokenCodes::paren_open; }
	bool Lex::isParenClose() { return code == TokenCodes::paren_close; }
	bool Lex::isDelim() { return code == TokenCodes::delim; }
	bool Lex::isVal() {
		return 
		code == TokenCodes::float_val |
		code == TokenCodes::int_val;
	}
	bool Lex::isColon() { return code == TokenCodes::colon; }
	bool Lex::isSayonara() { return code == TokenCodes::sayonara; }
	bool Lex::isNoJutsu() { return code == TokenCodes::no_jutsu; }
	bool Lex::isChan() { return code == TokenCodes::chan; }
	bool Lex::isBaka() { return code == TokenCodes::baka; }
	bool Lex::isNani() { return code == TokenCodes::nani; }
	bool Lex::isDoki() { return code == TokenCodes::doki; }
	bool Lex::isWa() { return code == TokenCodes::wa; }
	bool Lex::isDesu() { return code == TokenCodes::desu; }
	bool Lex::isNewline() { return code == TokenCodes::new_line; }
	bool Lex::isFnDelim() { return code == TokenCodes::fn_delim; }
	bool Lex::isMainFnDelim() { return code == TokenCodes::main_fn_delim; }
	bool Lex::isWhileDelim() { return code == TokenCodes::while_delim; }
	bool Lex::isIfDelim() { return code == TokenCodes::if_delim; }
	bool Lex::isThreadDelim() { return code == TokenCodes::fn_delim; }
	bool Lex::isFloatVal() { return code == TokenCodes::float_val; }
	bool Lex::isIntVal() { return code == TokenCodes::int_val; }

	inline bool should_lex(std::string input) {
		return input.length() && input != " " && input != "	" && input != "\n";
	}

	void lex_list(std::string input, std::vector<std::string> &split, std::vector<Lex> & lexes, 
	std::vector<std::string> &regex_words, std::vector<std::string> &words, int word)
	{
		if(word >= regex_words.size())
		{
			if(should_lex(input)) 
				split.push_back(input);
			return;
		}
		if(regex_words[word].length() < 3)
		{
			size_t current = 0;
			size_t next = 0;
			do {
				next = input.find(regex_words[word], current);
				if(next == std::string::npos) next = input.length();
				lex_list(input.substr(current, next - current), split, lexes, 
						regex_words, words, word+1);
				if(next != input.length() && should_lex(words[word])) 
					split.push_back(words[word]);
				current = next + std::string(words[word]).length();
			}
			while(current < input.length());
		}
		else
		{
			std::smatch m;
			std::regex e(regex_words[word]);
			std::vector<size_t> positions;	
			auto search_temp = input;
			size_t g_pos = 0;
			while(std::regex_search(search_temp, m, e)) 
			{
				positions.push_back(m.position(0) + g_pos);
				search_temp = m.suffix().str();
				g_pos += m.length() + m.position(0);
			}
			positions.push_back(std::string::npos);
		
			size_t current = 0;
			size_t next = 0;
			size_t i = 0;
			do {
				next = positions[i++];
				if(next == std::string::npos) next = input.length();
				lex_list(input.substr(current, next - current), split, lexes, 
						regex_words, words, word+1);
				if(next != input.length() && should_lex(words[word])) 
					split.push_back(words[word]);
				//-4 comes from the \b \b
				current = next + std::string(regex_words[word]).length() - 4; 
			}
			while(current < input.length());
		}
	}

	void naruto_lexize(std::string input, std::vector<Lex> & lexes)
	{
		std::vector<std::string> regex_words = {"\n", 
		"\\bno jutsu\\b", "\\bshadow clone jutsu\\b", "\\bdesu ga\\b", 
		" ", "	", ":", "~~", "~?", "~!", "!!", "~",
		"\\bdoki\\b", "\\bnani\\b", "\\bbaka\\b", "\\bnamae\\b", "\\bdesu\\b", "\\bwa\\b", 
		"\\bsayonara\\b", "\\bchan\\b",
		"(", ")", "+", "-", "*", "/", "%", "<<", ">>", ">=", "<=", "||", ">", "<"};
		std::vector<std::string> words = {"\n", 
		"no jutsu", "shadow clone jutsu", "desu ga", 
		" ", "	", ":", "~~", "~?", "~!", "!!", "~",
		"doki", "nani", "baka", "namae", "desu", "wa", 
		"sayonara", "chan",
		"(", ")", "+", "-", "*", "/", "%", "<<", ">>", ">=", "<=", "||", ">", "<"};
		std::vector<std::string> items;
		lex_list(input, items, lexes, regex_words, words, 0);
		for(auto item : items) {
			if(item == "\n")
				std::cout << "'\\n'" << std::endl;
			else
				std::cout << (int)(item.c_str()[0]) << ": '" << item << "'" << std::endl;
			if(item == "\n") lexes.push_back(Lex(TokenCodes::new_line));
			else if(item == ":") lexes.push_back(Lex(TokenCodes::colon));
			else if(item == "~") lexes.push_back(Lex(TokenCodes::delim));
			else if(item == "~~") lexes.push_back(Lex(TokenCodes::while_delim));
			else if(item == "~!") lexes.push_back(Lex(TokenCodes::main_fn_delim));
			else if(item == "~?") lexes.push_back(Lex(TokenCodes::if_delim));
			else if(item == "!!") lexes.push_back(Lex(TokenCodes::fn_delim));
			else if(item == "(") lexes.push_back(Lex(TokenCodes::paren_open));
			else if(item == ")") lexes.push_back(Lex(TokenCodes::paren_close));
			else if(item == "shadow clone jutsu") 
				lexes.push_back(Lex(TokenCodes::shadow_clone_jutsu));
			else if(item == "desu") lexes.push_back(Lex(TokenCodes::desu));
			else if(item == "nani") lexes.push_back(Lex(TokenCodes::nani));
			else if(item == "baka") lexes.push_back(Lex(TokenCodes::baka));
			else if(item == "wa") lexes.push_back(Lex(TokenCodes::wa));
			else if(item == "no jutsu") lexes.push_back(Lex(TokenCodes::no_jutsu));
			else if(item == "chan") lexes.push_back(Lex(TokenCodes::chan));
			else if(item == "sayonara") lexes.push_back(Lex(TokenCodes::sayonara));
			else if(item == "desu ga") 
				lexes.push_back(Lex(TokenCodes::bin_op, "=="));
			else if(item == "to") lexes.push_back(Lex(TokenCodes::bin_op, "&&"));
			else if(item == "-") lexes.push_back(Lex(TokenCodes::bin_op, "-"));
			else if(item == "+") lexes.push_back(Lex(TokenCodes::bin_op, "+"));
			else if(isdigit(item[0])) {
				if(item.find(".", 0) != std::string::npos)
					lexes.push_back(Lex(TokenCodes::float_val, 
										EMP_STR, 0, stod(item)));
				else
					lexes.push_back(Lex(TokenCodes::int_val, 
										EMP_STR, stol(item), 0));
			}
			else if(isalpha(item[0])) 
				lexes.push_back(Lex(TokenCodes::identifier, item));
			else lexes.push_back(Lex(TokenCodes::bin_op, item));
		}
	}
		
	std::vector<Lex> naruto_lexize_file(std::string input_file)
	{
		std::ifstream in(input_file, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			std::vector<Lex> out;
			naruto_lexize(contents, out);
			return out;
		}
		throw(errno);
	}
}


