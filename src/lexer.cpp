#include <cctype>
#include <fstream>
#include <iostream>

#include "lexer.h"

#define EMP_STR std::string("")

namespace naruto
{	
	std::vector<std::string> split(std::string input)
	{
		std::vector<std::string> output;
		std::string delim_space = " ";
		std::string delim_tab = "	";
		
		size_t current;
		size_t next = -1;
		size_t pos_next = 0;
		
		do
		{
			current = next + 1;
			next = input.find_first_of(delim_tab, current);
			pos_next = input.find_first_of(delim_space, current);
			if(next != std::string::npos && pos_next != std::string::npos)
				next = next < pos_next ? next : pos_next;
			else if(next == std::string::npos)
				next = pos_next;
			output.push_back(input.substr(current, next - current));
			std::cout << input.substr(current, next - current) << std::endl;
		}
		while (next != std::string::npos);
		return output;
	}
	
	void naruto_lexize(std::string input, std::vector<Lex> & lexes)
	{
		auto items = split(input);
		for(int i = 0; i < items.size(); i++)
		{
			if(items[i] == "")
				continue;
			else if(items[i] == "no") 
			{
				if(i < items.size() && items[i+1] == "jutsu")
				{
					lexes.push_back(Lex(TokenCodes::no_jutsu, EMP_STR, 0, 0));
					i++;
				}
			}
			else if(items[i] == "yosh")
				lexes.push_back(Lex(TokenCodes::int_val, EMP_STR, 1, 0));
			else if(items[i] == "iee")
				lexes.push_back(Lex(TokenCodes::int_val, EMP_STR, 0, 0));
			else if(items[i] == "sayonara")
				lexes.push_back(Lex(TokenCodes::sayonara, EMP_STR, 0, 0));
			else if(items[i] == "chan")
				lexes.push_back(Lex(TokenCodes::chan, EMP_STR, 0, 0));
			else if(items[i] == "nani")
				lexes.push_back(Lex(TokenCodes::nani, EMP_STR, 0, 0));
			else if(items[i] == "baka")
				lexes.push_back(Lex(TokenCodes::baka, EMP_STR, 0, 0));
			else if(items[i] == "suki")
				lexes.push_back(Lex(TokenCodes::suki, EMP_STR, 0, 0));
			else if(items[i] == "wa")
				lexes.push_back(Lex(TokenCodes::wa, EMP_STR, 0, 0));
			else if(items[i] == "desu")
			{
				if(i < items.size() && items[i+1] == "ga")
				{
					lexes.push_back(Lex(TokenCodes::bin_op, "==", 0, 0));
					i++;
				}
				else
				{	
					lexes.push_back(Lex(TokenCodes::desu, EMP_STR, 0, 0));
				}
			}
			else if(items[i] == "to")
				lexes.push_back(Lex(TokenCodes::bin_op, "&&", 0, 0));
			else if(items[i] == "\n")
				lexes.push_back(Lex(TokenCodes::newline, EMP_STR, 0, 0));
			else if(isdigit(items[i][0]) || (items[i].size() > 1 && items[i][0] == '-' && isdigit(items[i][0])))
			{
				if(items[i].find(".") != std::string::npos)
				{
					lexes.push_back(Lex(TokenCodes::float_val, 
					EMP_STR, 0, 
					std::stod(items[i])));
				}
				else
				{
					lexes.push_back(Lex(TokenCodes::int_val, 
					EMP_STR, 
					std::stoi(items[i]), 0));

				}
			}
			else if(items[i][0] == '(')
			{	
				lexes.push_back(Lex(TokenCodes::paren_open, EMP_STR, 0, 0));
			}
			else if(items[i][0] == ')')
			{	
				lexes.push_back(Lex(TokenCodes::paren_close, EMP_STR, 0, 0));
			}
			else if(isalpha(items[i][0]))
			{	
				lexes.push_back(Lex(TokenCodes::identifier, items[i], 0, 0));
			}
			else	
				lexes.push_back(Lex(TokenCodes::bin_op, items[i], 0, 0));
		}
	}
	
	std::vector<Lex> naruto_lexize_file(std::string input_file)
	{
		std::string line;
		std::ifstream file(input_file);
		std::vector<Lex> out;
		if(file.is_open())
		{
			while(getline(file, line))
			{
				naruto_lexize(line, out);
				out.push_back(Lex(TokenCodes::newline, EMP_STR, 0, 0));
			}
			file.close();
		}
		
		return out;
	}
}


