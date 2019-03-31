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
		code == TokenCodes::int_val |
		code == TokenCodes::string_val;
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
	bool Lex::isStrVal() { return code == TokenCodes::string_val; }

	inline bool should_lex(std::string input) {
		return input.length() > 0 && input != " " && input != "	" && input != "\n";
	}

	void lex_list(std::string input, std::vector<std::string> &split, 
	std::vector<std::string> &regex_words, int word)
	{
		if(word >= regex_words.size())
		{
			if(should_lex(input)) 
				split.push_back(input);
			return;
		}
		std::smatch m;
		std::regex e(regex_words[word]);
		while(std::regex_search(input, m, e)) 
		{
			std::string prefix = m.prefix().str();
			std::string found = m.str();
			std::string suffix = m.suffix().str();
			input = suffix;
			lex_list(prefix, split, regex_words, word+1);
			if(should_lex(found)) split.push_back(found);
		}
		lex_list(input, split, regex_words, word+1);
	}

	void naruto_lexize(std::string input, std::vector<Lex> & lexes)
	{
		std::vector<std::string> regex_words = {"\\n", "\".*\"", 
		"\\bno jutsu\\b", "\\bshadow clone jutsu\\b", "\\bdesu ga\\b", 
		" ", "\\t", ":", "~~", "~\\?", "~!", "!!", "~",
		"\\bdoki\\b", "\\bnani\\b", "\\bbaka\\b", "\\bnamae\\b", "\\bdesu\\b", "\\bwa\\b", 
		"\\bsayonara\\b", "\\bchan\\b",
		"\\(", "\\)", "\\+", "-", "\\*", "/", "%", "<<", ">>", ">=", "<=", "\\|\\|", ">", "<"};
		std::vector<std::string> items(0);
		lex_list(input, items, regex_words, 0);
		for(int i = 0; i < items.size(); i++) {
			auto item = items[i];
			/*if(item == "\n")
				std::cout << "'\\n'" << std::endl;
			else
				std::cout << (int)(item.c_str()[0]) << ": '" << item << "'" << std::endl;*/
			if(item == "\n") lexes.push_back(Lex(TokenCodes::new_line));
			else if(item == ":") lexes.push_back(Lex(TokenCodes::colon));
			else if(item == "~") lexes.push_back(Lex(TokenCodes::delim));
			else if(item == "~~") lexes.push_back(Lex(TokenCodes::while_delim));
			else if(item == "~!") lexes.push_back(Lex(TokenCodes::fn_delim));
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
			else if(item == "-") 
			{
				if(lexes.size() > 0)
				{
					auto last = lexes.back();
					if(!last.isIden() && !last.isVal() && !last.isNoJutsu())
					{
						if(i+1 < items.size() && isdigit(items[i+1][0]))
						{
							i++;
							item = items[i];
							if(item.find(".", 0) != std::string::npos)
								lexes.push_back(Lex(TokenCodes::float_val, 
												EMP_STR, 0, -1*stod(item)));
							else
								lexes.push_back(Lex(TokenCodes::int_val, 
												EMP_STR, -1*stol(item), 0));
						}
						else
						{
							std::cout << "LEX ERROR: Not a negative number or subtraction at pos " << i << std::endl;
						}
						
					}
					else
					{
						lexes.push_back(Lex(TokenCodes::bin_op, "-"));
					}
				}
			}
			else if(item == "+") lexes.push_back(Lex(TokenCodes::bin_op, "+"));
			else if(isdigit(item[0])) {
				if(item.find(".", 0) != std::string::npos)
					lexes.push_back(Lex(TokenCodes::float_val, 
										EMP_STR, 0, stod(item)));
				else
					lexes.push_back(Lex(TokenCodes::int_val, 
										EMP_STR, stol(item), 0));
			}
			else if(item[0] == '\"')
				lexes.push_back(Lex(TokenCodes::string_val, item));	
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


