#pragma once

#include <string>

namespace bconv {
	
	using std::string;
	
	typedef enum
	{
		E_SYSTEM_NUMERATION_BIN   = 0x01,
		E_SYSTEM_NUMERATION_DEC   = 0x02,
		E_SYSTEM_NUMERATION_HEX   = 0x04
	} e_system_numeration;
	
	typedef enum
	{
		E_DIRECTION_ARRAY_VERTICAL   = 0x01,
		E_DIRECTION_ARRAY_HORISONTAL = 0x02
	} e_direction_array;
	
	struct s_prog_argument
	{
		string s_name;
		string s_notice;
		bool b_on;
	};
	
	string strtolower(const string& str) 
	{ 
		string result = ""; 
	  
		for (char ch : str) { 
			result += tolower(ch); 
		}
	  
		return result; 
	}
}