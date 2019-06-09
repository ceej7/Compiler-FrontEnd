#pragma once
// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
using namespace std;

const int inf = 0x3f3f3f3f;
//-----------------------------------------------------------------------------------
//----------------------------------Data Structure-----------------------------------
enum Symbol
{
	NT,
	//Non-terminal
	program,
	stmt,
	compoundstmt,
	stmts,
	ifstmt,
	whilestmt,
	assgstmt,
	boolexpr,
	boolop,
	arithexpr,
	arithexprprime,
	multexpr,
	multexprprime,
	simpleexpr,

	T,
	//Terminal
	LABRACKET,
	RABRACKET,
	E,
	IF,
	LCBRACKET,
	RCBRACKET,
	THEN,
	ELSE,
	WHILE,
	ID,
	ASSIGN,
	LT,
	LE,
	GT,
	GE,
	EQ,
	ADD,
	MINUS,
	MULTIPLE,
	DIVIDE,
	NUM,
	SEMICOLON,

	//end
	ENDDING
};
class Production
{
public:
	Symbol NT;
	vector<Symbol> alpha;
	bool isNullable;
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
unordered_map<string, Symbol> StrToTerminalMap; //将读入的Symbol转化为编号的映射
string TerminalToStrMap[ENDDING]; //将编号转化为Symbol String的映射
vector<Production> productions; //生成式
bool isFirstOpt = true;

//-----------------------------------------------------------------------------------
//----------------------------------Data Precomputed --------------------------------
//优化3：unordered_set内涵foward_list串联set，加速遍历至O(num_of_element)
unordered_set<Symbol> First[ENDDING]; //First集合
unordered_set<Symbol> Follow[T]; //Follow集合
//优化4：空间换时间，防止计算Follow时对First的二次计算
unordered_set<Symbol> ProductSentenceFormFirst[28][8]; //Production的First集合
int parsingTable[ENDDING + 1][ENDDING + 1];//分析表

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
	StrToTerminalMap.insert(std::pair<string, Symbol>("while", WHILE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("While", WHILE));
	StrToTerminalMap.insert(std::pair<string, Symbol>("WHILE", WHILE));
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
	StrToTerminalMap.insert(std::pair<string, Symbol>("NUM", NUM));
	StrToTerminalMap.insert(std::pair<string, Symbol>("num", NUM));
	StrToTerminalMap.insert(std::pair<string, Symbol>(";", SEMICOLON));

	// mapping for NT to NT str
	TerminalToStrMap[program] = "program";
	TerminalToStrMap[stmt] = "stmt";
	TerminalToStrMap[compoundstmt] = "compoundstmt";
	TerminalToStrMap[stmts] = "stmts";
	TerminalToStrMap[ifstmt] = "ifstmt";
	TerminalToStrMap[whilestmt] = "whilestmt";
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
	TerminalToStrMap[WHILE] = "while";
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
	TerminalToStrMap[NUM] = "NUM";
	TerminalToStrMap[SEMICOLON] = ";";
}

void InitialProduction()//初始化产生式
{
	productions.push_back(Production(program, compoundstmt));
	productions.push_back(Production(stmt, ifstmt));
	productions.push_back(Production(stmt, whilestmt));
	productions.push_back(Production(stmt, assgstmt));
	productions.push_back(Production(stmt, compoundstmt));
	productions.push_back(Production(compoundstmt, LABRACKET, stmts, RABRACKET));
	productions.push_back(Production(stmts, stmt, stmts));
	productions.push_back(Production(stmts, E));
	productions.push_back(Production(ifstmt, IF, LCBRACKET, boolexpr, RCBRACKET, THEN, stmt, ELSE, stmt));
	productions.push_back(Production(whilestmt, WHILE, LCBRACKET, boolexpr, RCBRACKET, stmt));
	productions.push_back(Production(assgstmt, ID, ASSIGN, arithexpr, SEMICOLON));
	productions.push_back(Production(boolexpr, arithexpr, boolop, arithexpr));
	productions.push_back(Production(boolop, LT));
	productions.push_back(Production(boolop, GT));
	productions.push_back(Production(boolop, LE));
	productions.push_back(Production(boolop, GE));
	productions.push_back(Production(boolop, EQ));
	productions.push_back(Production(arithexpr, multexpr, arithexprprime));
	productions.push_back(Production(arithexprprime, ADD, multexpr, arithexprprime));
	productions.push_back(Production(arithexprprime, MINUS, multexpr, arithexprprime));
	productions.push_back(Production(arithexprprime, E));
	productions.push_back(Production(multexpr, simpleexpr, multexprprime));
	productions.push_back(Production(multexprprime, MULTIPLE, simpleexpr, multexprprime));
	productions.push_back(Production(multexprprime, DIVIDE, simpleexpr, multexprprime));
	productions.push_back(Production(multexprprime, E));
	productions.push_back(Production(simpleexpr, ID));
	productions.push_back(Production(simpleexpr, NUM));
	productions.push_back(Production(simpleexpr, LCBRACKET, arithexpr, RCBRACKET));
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

void ctrlFirstOpt()
{
	if (!isFirstOpt)
		printf("\n");
	isFirstOpt = false;
}

void outputParsingTree(vector<int>& seq, int level, int& index)
{
	if (index >= seq.size() || seq[index] >= productions.size())
		return;
	Production prod = productions[seq[index++]];

	//left side
	ctrlFirstOpt();
	for (int i = 0; i < level; i++)
	{
		printf("\t");
	}
	printf("%s", TerminalToStrMap[prod.NT].c_str());

	//right side
	level++;
	for (int x_num = 0; x_num < prod.alpha.size(); x_num++)
	{
		Symbol X = prod.alpha[x_num];
		if (X < T) // non-terminal
		{
			outputParsingTree(seq, level, index);
		}
		else // terminal
		{
			ctrlFirstOpt();
			for (int i = 0; i < level; i++)
			{
				printf("\t");
			}
			printf("%s", TerminalToStrMap[X].c_str());
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
		bool isAllNullable = true;
		//优化6：由于已经计算过每个产生式的右边句型的First集合，直接使用ProductSentenceFormFirst[prod_num][0]
		for (unordered_set<Symbol>::iterator it = ProductSentenceFormFirst[prod_num][0].begin(); it != ProductSentenceFormFirst[prod_num][0].end(); it++)
		{
			if (*it != E)
			{
				parsingTable[A][*it] = prod_num;
			}
		}
		if (ProductSentenceFormFirst[prod_num][0].count(E) != 0)
		{
			for (unordered_set<Symbol>::iterator it = Follow[A].begin(); it != Follow[A].end(); it++)
			{
				if (*it != E)
				{
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
	string prog;
	read_prog(prog);
	/* 骚年们 请开始你们的表演 */
	/********* Begin *********/

	// initialization
	InitialMapping();
	InitialProduction();
	InitialParsingTable();

	// LL1 parsing
	stack<Symbol> symbStack;
	vector<int> productionSeq;
	symbStack.push(ENDDING);
	symbStack.push(program);

	istringstream progin(prog);
	string line;
	int line_count = 0;
	// stacking
	while (getline(progin, line))
	{
		line_count++;

		istringstream linein(line);
		string token;
		if (linein >> token)
			while (1)
			{
				Symbol inputSymb = StrToTerminalMap[token];
				Symbol topSymb = symbStack.top();
				if (topSymb < T)
				{
					//if on the stack is Non-Terminal
					//turning to parsing table
					int prod_num = parsingTable[topSymb][inputSymb];
					if (prod_num != inf)
					{
						// pop
						symbStack.pop();
						// push n
						Production prod = productions[prod_num];
						for (int i = prod.alpha.size() - 1; i >= 0; i--)
						{
							if (prod.alpha[i] != E)
							{
								symbStack.push(prod.alpha[i]);
							}
						}
						// note production num
						productionSeq.push_back(prod_num);
					}
					else {
						//ERROR
						//skip all nonterminal
						symbStack.pop();
						for (int k = 0; k < productions.size(); k++)
						{
							Production prod = productions[k];
							if (prod.NT == topSymb && prod.alpha.size() == 1 && prod.alpha[0] == E)
							{
								productionSeq.push_back(k);
								break;
							}
						}
					}
				}
				else if (topSymb != ENDDING) {
					//if is terminal
					if (inputSymb == topSymb)
					{
						symbStack.pop();
						if (!(linein >> token))
						{
							break;
						}
					}
					else {
						//ERROR
						ctrlFirstOpt();
						printf("语法错误,第%d行,缺少\"%s\"", line_count - 1, TerminalToStrMap[topSymb].c_str());
						symbStack.pop();
					}
				}
				else {
					break;
				}
			}

	}
	int level = 0;
	int index = 0;
	outputParsingTree(productionSeq, level, index);
	/********* End *********/

}
