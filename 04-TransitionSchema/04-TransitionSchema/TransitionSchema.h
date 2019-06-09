#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

#include <fstream>
#include <sstream>

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
using namespace std;

const int inf = 0x3f3f3f3f;
bool isFirstOpt = true;
bool isError = false;
string prog;
istringstream progin;
string line;
istringstream linein;
int line_num = 1;
void ctrlFirstOpt()
{
	if (!isFirstOpt)
		printf("\n");
	isFirstOpt = false;
}

//-----------------------------------------------------------------------------------
//----------------------------------Data Structure-----------------------------------
enum Symbol
{
	NT,
	//Non-terminal
	program, decls, decl, stmt, compoundstmt, stmts, ifstmt, assgstmt, boolexpr, boolop,
	arithexpr, arithexprprime, multexpr, multexprprime, simpleexpr,

	T,
	//Terminal
	LABRACKET, RABRACKET, E, IF, LCBRACKET, RCBRACKET, THEN, ELSE, ID, ASSIGN,
	LT, LE, GT, GE, EQ, ADD, MINUS, MULTIPLE, DIVIDE, SEMICOLON, MYINT, INTNUM, MYREAL, REALNUM,

	//end
	ENDDING
};

enum TS_type
{
    ts_int,ts_real,ts_bool
};
union TS_value
{
    int i;
    float r;
    bool b;
};
class TS_Attr
{
public:
    TS_value val;
    TS_type type;
    TS_Attr(int i=0):
        type(ts_int)
    {
        val.i=i;
    }

    TS_Attr(bool b):
    type(ts_bool)
    {
        val.b=b;
    }

    TS_Attr(float r):
    type(ts_real)
    {
        val.r=r;
    }
    TS_Attr& operator=(TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			this->val.i = attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			this->val.r = attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			this->val.r = attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			ctrlFirstOpt();
			isError = true;
			cout << "error message:line " << line_num << ",realnum can not be translated into int type";
			//this->val.i = attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			this->val.b = attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			ctrlFirstOpt();
			isError = true;
			cout << "error message:line " << line_num << ",intnum can not be translated into bool type";
			this->val.b = attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			this->val.i = attr.val.b;
		}
		else {
			//cout << "error: ";
			return *this;
		}
    }
    bool operator<(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return this->val.i < attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return this->val.r < attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return this->val.r < attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return this->val.i < attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			return this->val.b < attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			return this->val.b < attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			return this->val.i < attr.val.b;
		}
		else {
			//cout << "error: ";
			return false;
		}
    }
    bool operator<=(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return this->val.i <= attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return this->val.r <= attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return this->val.r <= attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return this->val.i <= attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			return this->val.b <= attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			return this->val.b <= attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			return this->val.i <= attr.val.b;
		}
		else {
			//cout << "error: ";
			return false;
		}
    }
    bool operator==(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return this->val.i == attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return this->val.r == attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return this->val.r == attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return this->val.i == attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			return this->val.b == attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			return this->val.b == attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			return this->val.i == attr.val.b;
		}
		else {
			//cout << "error: ";
			return false;
		}
    }
    bool operator>(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return this->val.i > attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return this->val.r > attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return this->val.r > attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return this->val.i > attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			return this->val.b > attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			return this->val.b > attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			return this->val.i > attr.val.b;
		}
		else {
			//cout << "error: ";
			return false;
		}
    }
    bool operator>=(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return this->val.i >= attr.val.i;
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return this->val.r >= attr.val.r;
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return this->val.r >= attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return this->val.i >= attr.val.r;
		}
		else if (type1 == ts_bool && type2 == ts_bool)
		{
			return this->val.b >= attr.val.b;
		}
		else if (type1 == ts_bool && type2 == ts_int)
		{
			return this->val.b >= attr.val.i;
		}
		else if (type1 == ts_int && type2 == ts_bool)
		{
			return this->val.i >= attr.val.b;
		}
		else {
			//cout << "error: ";
			return false;
		}
    }
    
    TS_Attr operator+(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return TS_Attr(this->val.i + attr.val.i);
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return TS_Attr(this->val.r + attr.val.r);
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return TS_Attr(this->val.r + attr.val.i);
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return TS_Attr(this->val.i + attr.val.r);
		}
		else {
			//cout << "error: ";
			return TS_Attr();
		}
    }
    TS_Attr operator-(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return TS_Attr(this->val.i - attr.val.i);
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return TS_Attr(this->val.r - attr.val.r);
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return TS_Attr(this->val.r - attr.val.i);
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return TS_Attr(this->val.i - attr.val.r);
		}
		else {
			//cout << "error: ";
			return TS_Attr();
		}
    }
    TS_Attr operator*(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type1 == ts_int && type2 == ts_int)
		{
			return TS_Attr(this->val.i * attr.val.i);
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return TS_Attr(this->val.r * attr.val.r);
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return TS_Attr(this->val.r * attr.val.i);
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return TS_Attr(this->val.i * attr.val.r);
		}
		else {
			//cout << "error: ";
			return TS_Attr();
		}

       
    }
    TS_Attr operator/(const TS_Attr& attr)
    {
		TS_type type1 = this->type;
		TS_type type2 = attr.type;
		if (type2 == ts_int && attr.val.i == 0)
		{
			ctrlFirstOpt();
			isError = true;
			cout << "error message:line "<<line_num<<",division by zero";
			return TS_Attr(1);
		}
		if (type2 == ts_real && attr.val.r == 0)
		{
			ctrlFirstOpt();
			isError = true;
			cout << "error message:line " << line_num << ",division by zero";
			return TS_Attr(1);
		}
		if (type1 == ts_int && type2 == ts_int)
		{
			return TS_Attr(this->val.i / attr.val.i);
		}
		else if (type1 == ts_real && type2 == ts_real)
		{
			return TS_Attr(this->val.r / attr.val.r);
		}
		else if (type1 == ts_real && type2 == ts_int)
		{
			return TS_Attr(this->val.r / attr.val.i);
		}
		else if (type1 == ts_int && type2 == ts_real)
		{
			return TS_Attr(this->val.i / attr.val.r);
		}
		else {
			//cout << "error: ";
			return TS_Attr();
		}
    }
};

class Production
{
public:
	Symbol NT;
	vector<Symbol> alpha;
	bool isNullable;
    std::function<TS_Attr(TS_Attr)> my_func;
	Production(Symbol nt, Symbol a1, Symbol a2 = ENDDING, Symbol a3 = ENDDING, Symbol a4 = ENDDING,
		Symbol a5 = ENDDING, Symbol a6 = ENDDING, Symbol a7 = ENDDING, Symbol a8 = ENDDING)
	{
		NT = nt; alpha.push_back(a1);
		if (a2 != ENDDING)alpha.push_back(a2);
		if (a3 != ENDDING)alpha.push_back(a3);
		if (a4 != ENDDING)alpha.push_back(a4);
		if (a5 != ENDDING)alpha.push_back(a5);
		if (a6 != ENDDING)alpha.push_back(a6);
		if (a7 != ENDDING)alpha.push_back(a7);
		if (a8 != ENDDING)alpha.push_back(a8);
		isNullable = false;
		if (a1 == E || a2 == E || a3 == E || a4 == E ||
			a6 == E || a7 == E || a8 == E || a5 == E)
			isNullable = true;
	}
};

//-----------------------------------------------------------------------------------
//----------------------------------Static Data--------------------------------------
//优化1：将读入的Symbol映射到对应的编号(enum = INT)，int作为基础加速parsing table生成
//优化2：使用哈希表加速映射查找
Symbol current_Symbol;
TS_Attr current_Attr;
string token = "";
unordered_map<string, Symbol> StrToTerminalMap; //将读入的Symbol转化为编号的映射
string TerminalToStrMap[ENDDING]; //将编号转化为Symbol String的映射
vector<Production> productions; //生成式


//-----------------------------------------------------------------------------------
//----------------------------------Data Precomputed --------------------------------
//优化3：unordered_set内涵foward_list串联set，加速遍历至O(num_of_element)
unordered_set<Symbol> First[ENDDING]; //First集合
unordered_set<Symbol> Follow[T]; //Follow集合
//优化4：空间换时间，防止计算Follow时对First的二次计算
unordered_set<Symbol> ProductSentenceFormFirst[33][8]; //Production的First集合
int parsingTable[ENDDING + 1][ENDDING + 1];//分析表

//-----------------------------------------------------------------------------------
//----------------------------------Data Table --------------------------------
unordered_map<string, TS_Attr> IdTable;

inline bool isInteger(const std::string& s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	char* p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}
inline bool isReal(const std::string& s)
{
	return true;
}

bool get_Symbol()
{
	if (linein >> token)
	{
		if (StrToTerminalMap.count(token) != 0)
		{
			current_Symbol = StrToTerminalMap[token];
		}
		else if (token.size() == 1 && isalpha(token[0]))
		{
			if (IdTable.count(token) == 0)
			{
				IdTable.emplace(token, TS_Attr());
			}
			current_Symbol = ID;
		}
		else if (isInteger(token))
		{
			current_Symbol = INTNUM;
			current_Attr.val.i = atoi(token.c_str());
			current_Attr.type = ts_int;
		}
		else
		{
			current_Symbol = REALNUM;
			current_Attr.val.r = atof(token.c_str());
			current_Attr.type = ts_real;
		}

		return true;
	}
	else {
		line_num++;
		getline(progin, line);
		if (line != "")
		{
			linein.clear();
			linein.str(line);
			return get_Symbol();
		}
		else {
			return false;
		}
	}
}

//-----------------------------------------------------------------------------------
//----------------------------------Functions----------------------------------------
void InitialMapping()//初始化Str->S Mapping and X->Str Mapping
{
	// mapping from T string to T
	StrToTerminalMap.insert(std::pair<string, Symbol>("{", LABRACKET));
	StrToTerminalMap.insert(std::pair<string, Symbol>("}", RABRACKET));
	StrToTerminalMap.insert(std::pair<string, Symbol>("E", E));
	StrToTerminalMap.insert(std::pair<string, Symbol>("e", E));
	StrToTerminalMap.insert(std::pair<string, Symbol>("if", IF));
	StrToTerminalMap.insert(std::pair<string, Symbol>("If", IF));
	StrToTerminalMap.insert(std::pair<string, Symbol>("IF", IF));
	StrToTerminalMap.insert(std::pair<string, Symbol>("(", LCBRACKET));
	StrToTerminalMap.insert(std::pair<string, Symbol>(")", RCBRACKET));
	StrToTerminalMap.insert(std::pair<string, Symbol>("then", THEN));
	StrToTerminalMap.insert(std::pair<string, Symbol>("Then", THEN));
	StrToTerminalMap.insert(std::pair<string, Symbol>("THEN", THEN));
	StrToTerminalMap.insert(std::pair<string, Symbol>("else", ELSE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("Else", ELSE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("ELSE", ELSE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("ID", ID));
	StrToTerminalMap.insert(std::pair<string, Symbol>("id", ID));
	StrToTerminalMap.insert(std::pair<string, Symbol>("Id", ID));
	StrToTerminalMap.insert(std::pair<string, Symbol>("=", ASSIGN));
	StrToTerminalMap.insert(std::pair<string, Symbol>("<", LT));
	StrToTerminalMap.insert(std::pair<string, Symbol>("<=", LE));
	StrToTerminalMap.insert(std::pair<string, Symbol>(">", GT));
	StrToTerminalMap.insert(std::pair<string, Symbol>(">=", GE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("==", EQ));
	StrToTerminalMap.insert(std::pair<string, Symbol>("+", ADD));
	StrToTerminalMap.insert(std::pair<string, Symbol>("-", MINUS));
	StrToTerminalMap.insert(std::pair<string, Symbol>("*", MULTIPLE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("/", DIVIDE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("int", MYINT));
	StrToTerminalMap.insert(std::pair<string, Symbol>("real", MYREAL));
	StrToTerminalMap.insert(std::pair<string, Symbol>(";", SEMICOLON));
	StrToTerminalMap.insert(std::pair<string, Symbol>("INTNUM", INTNUM));
	StrToTerminalMap.insert(std::pair<string, Symbol>("REALNUM", REALNUM));


	// mapping for NT to NT str
	TerminalToStrMap[program] = "program";
	TerminalToStrMap[decls] = "decls";
	TerminalToStrMap[decl] = "decl";
	TerminalToStrMap[stmt] = "stmt";
	TerminalToStrMap[compoundstmt] = "compoundstmt";
	TerminalToStrMap[stmts] = "stmts";
	TerminalToStrMap[ifstmt] = "ifstmt";
	TerminalToStrMap[assgstmt] = "assgstmt";
	TerminalToStrMap[boolexpr] = "boolexpr";
	TerminalToStrMap[boolop] = "boolop";
	TerminalToStrMap[arithexpr] = "arithexpr";
	TerminalToStrMap[arithexprprime] = "arithexprprime";
	TerminalToStrMap[multexpr] = "multexpr";
	TerminalToStrMap[multexprprime] = "multexprprime";
	TerminalToStrMap[simpleexpr] = "simpleexpr";

	// mapping for T to T str
	TerminalToStrMap[LABRACKET] = "{";
	TerminalToStrMap[RABRACKET] = "}";
	TerminalToStrMap[E] = "E";
	TerminalToStrMap[IF] = "if";
	TerminalToStrMap[LCBRACKET] = "(";
	TerminalToStrMap[RCBRACKET] = ")";
	TerminalToStrMap[THEN] = "then";
	TerminalToStrMap[ELSE] = "else";
	TerminalToStrMap[ID] = "ID";
	TerminalToStrMap[ASSIGN] = "=";
	TerminalToStrMap[LT] = "<";
	TerminalToStrMap[LE] = "<=";
	TerminalToStrMap[GT] = ">";
	TerminalToStrMap[GE] = ">=";
	TerminalToStrMap[EQ] = "==";
	TerminalToStrMap[ADD] = "+";
	TerminalToStrMap[MINUS] = "-";
	TerminalToStrMap[MULTIPLE] = "*";
	TerminalToStrMap[DIVIDE] = "/";
	TerminalToStrMap[SEMICOLON] = ";";
	TerminalToStrMap[MYINT] = "int";
	TerminalToStrMap[INTNUM] = "INTNUM";
	TerminalToStrMap[MYREAL] = "real";
	TerminalToStrMap[REALNUM] = "REALNUM";
}

TS_Attr handleNT(Symbol ts_NT, TS_Attr ts_Attr = 0)
{//隐式转换调用TS_ATTR(int i)的构造函数构造匿名对象然后默认拷贝构造函数拷贝
	int prodnum = parsingTable[ts_NT][current_Symbol];
	if (prodnum != inf)
	{
		return productions[prodnum].my_func(ts_Attr);
	}
	else {
		//cout << "error: ";
		return TS_Attr();
	}
}
bool handleT(Symbol ts_T)
{
	if (ts_T == current_Symbol)
	{
		return get_Symbol();
	}
	else {
		//cout << "error: ";
		return get_Symbol();
	}
}

void InitialProduction()//初始化产生式
{
	productions.push_back(Production(program, decls, compoundstmt));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(decls);
		handleNT(compoundstmt,TS_Attr(true));
        return TS_Attr();
    };
    
	productions.push_back(Production(decls, decl, SEMICOLON, decls));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(decl);
		handleT(SEMICOLON);
		handleNT(decls);
        return TS_Attr();
    };
    
	productions.push_back(Production(decls, E));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		//do nothing
        return TS_Attr();
    };
    
	productions.push_back(Production(decl, MYINT, ID, ASSIGN, INTNUM));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(MYINT);
		//在消耗我的id之前需要对id做已知的操作
		string tempt_id = token;

		IdTable[tempt_id].type = ts_int;
		handleT(ID);
		handleT(ASSIGN);
		IdTable[tempt_id] = current_Attr;
		handleT(INTNUM);
        return TS_Attr();
    };
    
	productions.push_back(Production(decl, MYREAL, ID, ASSIGN, REALNUM));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(MYREAL);
		//在消耗我的id之前需要对id做已知的操作
		string tempt_id = token;

		IdTable[tempt_id].type = ts_real;
		handleT(ID);
		handleT(ASSIGN);
		IdTable[tempt_id] = current_Attr;
		handleT(REALNUM);
        return TS_Attr();
    };
    
	productions.push_back(Production(stmt, ifstmt));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(ifstmt,attr);  
        return TS_Attr();
    };
    
	productions.push_back(Production(stmt, assgstmt));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(assgstmt,attr);
        return TS_Attr();
    };
    
	productions.push_back(Production(stmt, compoundstmt));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(compoundstmt,attr);
        return TS_Attr();
    };
    
	productions.push_back(Production(compoundstmt, LABRACKET, stmts, RABRACKET));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(LABRACKET);
		handleNT(stmts,attr);
		handleT(RABRACKET);
        return TS_Attr();
    };
    
	productions.push_back(Production(stmts, stmt, stmts));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleNT(stmt,attr);
		handleNT(stmts,attr);
        return TS_Attr();
    };
    
	productions.push_back(Production(stmts, E));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		//do nothing
        return TS_Attr();
    };
    
	productions.push_back(Production(ifstmt, IF, LCBRACKET, boolexpr, RCBRACKET, THEN, stmt, ELSE, stmt));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(IF);
		handleT(LCBRACKET);
		TS_Attr boolattr = handleNT(boolexpr);
		bool isFirst = boolattr.val.b;
		bool todo = attr.val.b;
		handleT(RCBRACKET);
		handleT(THEN);
		handleNT(stmt, TS_Attr(isFirst && todo));
		handleT(ELSE);
		handleNT(stmt, TS_Attr((!isFirst) && todo));
        return TS_Attr();
    };
    
	productions.push_back(Production(assgstmt, ID, ASSIGN, arithexpr, SEMICOLON));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		string tempt_id = token;
		handleT(ID);
		handleT(ASSIGN);
		TS_Attr arithexpr_result = handleNT(arithexpr);
		if (attr.val.b)
		{
			IdTable[tempt_id] = arithexpr_result;
		}
		handleT(SEMICOLON);
        return TS_Attr();
    };
    
	productions.push_back(Production(boolexpr, arithexpr, boolop, arithexpr));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		TS_Attr arith_result1 = handleNT(arithexpr);
		string op = token;
		handleNT(boolop);
		TS_Attr arith_result2 = handleNT(arithexpr);
		if (op == "<")
		{
			return TS_Attr(arith_result1 < arith_result2);
		}
		else if (op == "<=")
		{
			return TS_Attr(arith_result1 <= arith_result2);
		}
		else if (op == ">")
		{
			return TS_Attr(arith_result1 > arith_result2);
		}
		else if (op == ">=")
		{
			return TS_Attr(arith_result1 >= arith_result2);
		}
		else if (op == "==")
		{
			return TS_Attr(arith_result1 == arith_result2);
		}
		else {
			return TS_Attr(false);
		}
    };
    
	productions.push_back(Production(boolop, LT));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(LT);
        return TS_Attr();
    };
    
	productions.push_back(Production(boolop, GT));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(GT);
        return TS_Attr();
    };
    
	productions.push_back(Production(boolop, LE));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(LE);
        return TS_Attr();
    };
    
	productions.push_back(Production(boolop, GE));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(GE);
        return TS_Attr();
    };
    
	productions.push_back(Production(boolop, EQ));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(EQ);
        return TS_Attr();
    };
    
	productions.push_back(Production(arithexpr, multexpr, arithexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
 		TS_Attr param = handleNT(multexpr);
		return handleNT(arithexprprime, param);
    };
    
	productions.push_back(Production(arithexprprime, ADD, multexpr, arithexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(ADD);
		TS_Attr result = handleNT(multexpr);
		return handleNT(arithexprprime, attr + result);
    };
    
	productions.push_back(Production(arithexprprime, MINUS, multexpr, arithexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(MINUS);
		TS_Attr result = handleNT(multexpr);
		return handleNT(arithexprprime, attr - result);
    };
    
	productions.push_back(Production(arithexprprime, E));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
        return TS_Attr(attr);
    };
    
	productions.push_back(Production(multexpr, simpleexpr, multexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		TS_Attr param = handleNT(simpleexpr);
		return handleNT(multexprprime, param);
    };
    
	productions.push_back(Production(multexprprime, MULTIPLE, simpleexpr, multexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(MULTIPLE);
		TS_Attr result = handleNT(simpleexpr);
		return handleNT(multexprprime, attr * result);
    };
    
	productions.push_back(Production(multexprprime, DIVIDE, simpleexpr, multexprprime));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(DIVIDE);
		TS_Attr result = handleNT(simpleexpr);
		return handleNT(multexprprime, attr / result);
    };
    
	productions.push_back(Production(multexprprime, E));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
        return TS_Attr(attr);
    };
    
	productions.push_back(Production(simpleexpr, ID));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		string tempt_id = token;
		handleT(ID);
		return IdTable[tempt_id];
    };
    
	productions.push_back(Production(simpleexpr, INTNUM));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		TS_Attr tempt_attr = current_Attr;
		handleT(INTNUM);
        return tempt_attr;
    };
    
	productions.push_back(Production(simpleexpr, REALNUM));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		TS_Attr tempt_attr = current_Attr;
		handleT(REALNUM);
		return tempt_attr;
    };
    
	productions.push_back(Production(simpleexpr, LCBRACKET, arithexpr, RCBRACKET));
    productions[productions.size()-1].my_func = [](TS_Attr attr)->TS_Attr{
		handleT(LCBRACKET);
		TS_Attr tempt_attr = handleNT(arithexpr);
		handleT(RCBRACKET);
		return tempt_attr;
    };
    
}

void calculateFirst(Symbol s)//已优化，计算First数组同时维护ProductSentenceFormFirst(额外开销，时间复杂度级别不上升)
{
	if (!First[s].empty())
		return;
	// decide whether to append E in First(s)
	for (const auto& prod : productions)
	{
		if (s == prod.NT)
		{
			if (prod.isNullable == true)
				First[s].insert(E);
		}
	}
	// using production with left side of s to append X in First(s)
	int prod_num = 0;
	for (const auto& prod : productions)
	{
		if (s == prod.NT)
		{
			bool isAllNullable = true;
			int rSym_num = 0;
			for (const auto& X : prod.alpha)
			{
				calculateFirst(X);
				//add all in First(X) except E into first(s)
				for (unordered_set<Symbol>::iterator it = First[X].begin(); it != First[X].end(); it++)
				{
					if (*it != E)
					{
						First[s].insert(*it);
						//优化4，维护ProductSentenceFormFirst数组
						for (int i = 0; i <= rSym_num; i++)
						{
							ProductSentenceFormFirst[prod_num][i].insert(*it);
						}
					}
				}
				// if E not in First(x) then break the inserting and  set allNullable to false
				if (First[X].count(E) == 0)
				{
					isAllNullable = false;
					//优化4，维护ProductSentenceFormFirst数组
					for (int i = 0; i < rSym_num; i++)
					{
						ProductSentenceFormFirst[prod_num][i].insert(E);
					}
					break;
				}
				rSym_num++;
			}
			//if allnullable, add E into First(s)
			if (isAllNullable)
			{
				First[s].insert(E);
				//优化4，维护ProductSentenceFormFirst数组
				for (int i = 0; i < rSym_num; i++)
				{
					ProductSentenceFormFirst[prod_num][i].insert(E);
				}
			}
		}
		prod_num++;
	}
}

void InitialFirst()
{
	//Initial First for Terminals
	for (int i = (int)(T + 1); i < (int)ENDDING; i++)
	{
		First[i].insert((Symbol)i);
	}
	for (int i = NT + 1; i < T; i++)
	{
		calculateFirst((Symbol)i);
	}
}

void InitialFollow()//由于Follow集合的计算是循环到终止，因此必须优化<-已优化
{
	Follow[program].insert(ENDDING);
	bool hasModified = true;

	while (hasModified)
	{
		hasModified = false;
		int prod_num = 0;
		for (const auto& prod : productions)
		{//appending into follow set for NT
			Symbol NT = prod.NT;
			for (int i = 0; i < prod.alpha.size(); i++)
			{
				Symbol Xi = prod.alpha[i];
				if ((int)Xi < (int)T)
				{//for every Xi (NT) and scan its right side for appending
					//优化5：若ProductSentenceFormFirst已经被计算了则不重复计算 O(n^2)->O(n)
					if (i < prod.alpha.size() - 1 && ProductSentenceFormFirst[prod_num][i + 1].size() != 0)
					{
						for (unordered_set<Symbol>::iterator it = ProductSentenceFormFirst[prod_num][i + 1].begin(); it != ProductSentenceFormFirst[prod_num][i + 1].end(); it++)
						{
							if (*it != E && Follow[Xi].count(*it) == 0)//not E and not existing in Follow(Xi)
							{
								Follow[Xi].insert(*it);
								hasModified = true;
							}
						}
						if (ProductSentenceFormFirst[prod_num][i + 1].count(E) != 0)
						{
							for (unordered_set<Symbol>::iterator it = Follow[NT].begin(); it != Follow[NT].end(); it++)
							{
								if (*it != E && Follow[Xi].count(*it) == 0)//not E and not existing in Follow(Xi)
								{
									Follow[Xi].insert(*it);
									hasModified = true;
								}
							}
						}
					}
					else {

						bool isAllNullable = true;
						for (int j = i + 1; j < prod.alpha.size(); j++)
						{//for every xj right to Xi
							Symbol Xj = prod.alpha[j];
							for (unordered_set<Symbol>::iterator it = First[Xj].begin(); it != First[Xj].end(); it++)
							{
								if (*it != E && Follow[Xi].count(*it) == 0)//not E and not existing in Follow(Xi)
								{
									Follow[Xi].insert(*it);
									hasModified = true;
								}
							}
							if (First[Xj].count(E) == 0)
							{
								isAllNullable = false;
								break;
							}
						}
						if (isAllNullable)//right side of Xi is e or first(right side) has e
						{
							for (unordered_set<Symbol>::iterator it = Follow[NT].begin(); it != Follow[NT].end(); it++)
							{
								if (*it != E && Follow[Xi].count(*it) == 0)//not E and not existing in Follow(Xi)
								{
									Follow[Xi].insert(*it);
									hasModified = true;
								}
							}
						}

					}
				}
			}
			prod_num++;
		}
	}
}



void InitialParsingTable()
{
	memset(parsingTable, 0x3f, sizeof(parsingTable));
	InitialFirst();
	InitialFollow();
	for (int prod_num = 0; prod_num < productions.size(); prod_num++)
	{
		const Production& prod = productions[prod_num];
		Symbol A = prod.NT;
		// prod: A->X1X2X3...
		//bool isAllNullable = true;
		//优化6：由于已经计算过每个产生式的右边句型的First集合，直接使用ProductSentenceFormFirst[prod_num][0]
		for (unordered_set<Symbol>::iterator it = ProductSentenceFormFirst[prod_num][0].begin(); it != ProductSentenceFormFirst[prod_num][0].end(); it++)
		{
			if (*it != E)
			{
				//if (parsingTable[A][*it] != inf)
				//	cout << "conflict!!!!!!!!!!!!!!!!!!!!" << endl;
				parsingTable[A][*it] = prod_num;
			}
		}
		if (ProductSentenceFormFirst[prod_num][0].count(E) != 0)
		{
			for (unordered_set<Symbol>::iterator it = Follow[A].begin(); it != Follow[A].end(); it++)
			{
				if (*it != E)
				{
					//if (parsingTable[A][*it] != inf)
					//	cout << "conflict!!!!!!!!!!!!!!!!!!!!" << endl;
					parsingTable[A][*it] = prod_num;
				}
			}
		}
	}

}



/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
	char c;
	while (scanf("%c", &c) != EOF) {
		prog += c;
	}
}



/* 你可以添加其他函数 */
void Analysis()
{
	read_prog(prog);
    progin.str(prog);
    getline(progin,line);
    linein.clear();
    linein.str(line);
    
    
//    string tmpt = get_Symbol();
//    while(tmpt!="")
//    {
//        printf("line%d, %s\n",line_num,tmpt.c_str());
//
//        tmpt = get_Symbol();
//    }
    /* 骚年们 请开始你们的表演 */
	/********* Begin *********/

	// initialization
	InitialMapping();
	InitialProduction();
	InitialParsingTable();

    get_Symbol();
	productions[0].my_func(TS_Attr());

	if (!isError)
	{
		for (auto tid = IdTable.begin(); tid != IdTable.end(); tid++)
		{
			switch (tid->second.type)
			{
			case ts_int:
			{
				ctrlFirstOpt();
				cout << tid->first << ": " << tid->second.val.i;
				break;
			}
			case ts_real:
			{
				ctrlFirstOpt();
				cout << tid->first << ": " << tid->second.val.r;
				break;
			}
			case ts_bool:
			{
				ctrlFirstOpt();
				cout << tid->first << ": " << tid->second.val.b;
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}
	
	
	/********* End *********/

 }
