#include <iostream>

#include "lexerUtils.h"

namespace naruto
{
	std::string type_to_string(int code)
	{
		switch(code)
		{	
		case TokenCodes::identifier:
			return "Identifier";
		case TokenCodes::int_val:
			return "Int Val";
		case TokenCodes::float_val:
			return "Float Val";
		case TokenCodes::no_jutsu:
			return "No Jutsu";
		case TokenCodes::sayonara:
			return "Sayonara";
		case TokenCodes::chan:
			return "Chan";
		case TokenCodes::nani:
			return "Nani";
		case TokenCodes::baka:
			return "Baka";
		case TokenCodes::doki:
			return "Doki";
		case TokenCodes::wa:
			return "Wa";
		case TokenCodes::desu:
			return "Desu";
		case TokenCodes::bin_op:
			return "Binary Operator";
		case TokenCodes::paren_open:
			return "(";
		case TokenCodes::paren_close:
			return ")";
		case TokenCodes::delim:
			return "Delim";
		case TokenCodes::colon:
			return ":";
		case TokenCodes::new_line: 
			return "\\n";
		case TokenCodes::fn_delim: 
			return "Fn Delim (!!)";
		case TokenCodes::main_fn_delim: 
			return "Main Fn Delim (~!)";
		case TokenCodes::while_delim: 
			return "While Delim (~~)";
		case TokenCodes::if_delim: 
			return "If Delim (~?)";
		case TokenCodes::shadow_clone_jutsu: 
			return "Shadow Clone";
		default:
			return "???";
		}

	}

	void printLex(Lex item)
	{
		std::cout << "------------" << std::endl << 
		"Obj type: " << naruto::type_to_string(item.code) << std::endl << 
		"Obj info: '" << item.info << "'" << std::endl << 
		"Int val: " << item.int_val << std::endl << 
		"Flt val: " << item.float_val << std::endl;	
	}
}
