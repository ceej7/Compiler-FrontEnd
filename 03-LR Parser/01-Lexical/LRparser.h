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
#include <list>
#include <functional>
using namespace std;

const int inf = 0x3f3f3f3f;
//-----------------------------------------------------------------------------------
//----------------------------------Data Structure-----------------------------------
enum Symbol
{
	NT,
	//Non-terminal
	SPrime, //Agumented Gramma with SPrime -> program
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

class LR0Item
{
public:
	int prod_num;
	int dot_pos; //Note: 这里dotPos代表位置，假如有XYZ则pos的范围是[0,3]->0X1Y2Z3
	LR0Item(int prod, int pos)
	{
		prod_num = prod;
		dot_pos = pos;
	}
	bool operator==(LR0Item& item) // operator overload
	{
		return prod_num == item.prod_num&&dot_pos == item.dot_pos;
	}
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
		NT = nt;
		if (a1 != E)alpha.push_back(a1);
		if (a2 != ENDDING && a2 != E)alpha.push_back(a2);
		if (a3 != ENDDING && a3 != E)alpha.push_back(a3);
		if (a4 != ENDDING && a4 != E)alpha.push_back(a4);
		if (a5 != ENDDING && a5 != E)alpha.push_back(a5);
		if (a6 != ENDDING && a6 != E)alpha.push_back(a6);
		if (a7 != ENDDING && a7 != E)alpha.push_back(a7);
		if (a8 != ENDDING && a8 != E)alpha.push_back(a8);
		isNullable = false;
		if (a1 == E || a2 == E || a3 == E || a4 == E ||
			a6 == E || a7 == E || a8 == E || a5 == E)
			isNullable = true;
	}
};

class LR0ItemSet
{
public:
	vector<LR0Item> core;
	vector<LR0Item> extension;

	bool flg[T]; //优化7：标记closure是否考虑过当前start = NT的产生式被closure加入过
	list<LR0Item> LR0ItemsAfterDot[ENDDING + 1];//优化：维护dot后为A的LR0Item,这里不需要存储reference因为每个LR0Item都是两个int
	//使用list的原因,无随机访问 + 没有vector allocate space的特性
	int gotoMapping[ENDDING + 1];//标记goto[me,symbol]->的set


	LR0ItemSet()
	{
		memset(flg, false, sizeof(flg));
		memset(gotoMapping, 0x3f3f3f3f, sizeof(gotoMapping));
	}

	void addCore(LR0Item& item);
	void insert(LR0Item& item);
	void closure();

	bool operator==(LR0ItemSet& set) // operator overload
	{//优化：只对比core的关系来决定，加速判断Set是否已经存在
		
		if (this->core.size() != set.core.size())
		{
			return false;
		}
		/*for (auto& item : this->core)
		{
			bool notIn = true;
			for (auto& itemPrime : set.core)
			{
				if (item == itemPrime)
				{
					notIn = false;
					break;
				}
			}
			if (notIn)
				return false;
		}*/
		//优化: core一定是升序排列,对比复杂度O(n^2)->o(n)
		for (auto it1 = core.begin(), it2 = set.core.begin(); it1 != core.end();it1++,it2++)
		{
			if (*it1 == *it2)
			{

			}
			else
				return false;
		}
		return true;
	}
private:


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
//----------------------------------First Follow数据----------------------------------
//优化3：unordered_set内涵foward_list串联set，加速遍历至O(num_of_element)
unordered_set<Symbol> First[ENDDING]; //First集合
unordered_set<Symbol> Follow[T]; //Follow集合
//优化4：空间换时间，防止计算Follow时对First的二次计算
unordered_set<Symbol> ProductSentenceFormFirst[30][8]; //Production的First集合

//-----------------------------------------------------------------------------------
//----------------------------------------SLR数据------------------------------------
//项簇集 其中LR0ItemSet是reference 需要new and delete，同时gotoMapping包含了parsingtable
vector<reference_wrapper<LR0ItemSet>> CanonicalCollection;


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
	StrToTerminalMap.insert(std::pair<string, Symbol>("$", ENDDING));

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
	productions.push_back(Production(SPrime, program));//Agumented Gramma with SPrime -> program
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
	for (const auto & prod : productions)
	{
		if (s == prod.NT)
		{
			if (prod.isNullable == true)
				First[s].insert(E);
		}
	}
	// using production with left side of s to append X in First(s)
	int prod_num = 0;
	for (const auto & prod : productions)
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
	Follow[SPrime].insert(ENDDING);
	bool hasModified = true;

	while (hasModified)
	{
		hasModified = false;
		int prod_num = 0;
		for (const auto & prod : productions)
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

void LR0ItemSet::closure() //LR0ItemSet的Closure方法
{
	// for cores
	for (int i = 0; i < core.size(); i++)
	{
		LR0Item& item = core[i];
		if (item.dot_pos < productions[item.prod_num].alpha.size() && productions[item.prod_num].alpha[item.dot_pos] < T)
		{
			Symbol start = productions[item.prod_num].alpha[item.dot_pos];
			//如果以start为NT的production没有被加入过
			if (flg[start] == false)
			{
				for (int j = 0; j < productions.size(); j++)
				{
					auto& prod = productions[j];
					if (prod.NT == start)
					{
						LR0Item item_to_append(j, 0);
						//确认不在core中
						bool InCore = false;
						for (int k = 0; k < core.size(); k++)
						{
							if (item_to_append == core[k])
							{
								InCore = true;
								break;
							}
						}
						if (!InCore)
						{
							this->insert(item_to_append);
						}
					}
				}
				flg[start] = true;
			}
		}
	}

	// for closure
	for (int extension_num = 0; extension_num < extension.size(); extension_num++)
	{
		LR0Item& item = extension[extension_num];
		if (item.dot_pos < productions[item.prod_num].alpha.size() && productions[item.prod_num].alpha[item.dot_pos] < T)
		{
			Symbol start = productions[item.prod_num].alpha[item.dot_pos];
			//如果以start为NT的production没有被加入过
			if (flg[start] == false)
			{
				for (int j = 0; j < productions.size(); j++)
				{
					auto& prod = productions[j];
					if (prod.NT == start)
					{
						LR0Item item_to_append(j, 0);
						//确认不在core中
						bool InCore = false;
						for (int k = 0; k < core.size(); k++)
						{
							if (item_to_append == core[k])
							{
								InCore = true;
								break;
							}
						}
						if (!InCore)
						{
							this->insert(item_to_append);
						}
					}
				}
				flg[start] = true;
			}
		}
	}
}

void LR0ItemSet::addCore(LR0Item& item)
{
	core.push_back(item);
	if (item.dot_pos < productions[item.prod_num].alpha.size())
	{
		LR0ItemsAfterDot[productions[item.prod_num].alpha[item.dot_pos]].push_back(core[core.size() - 1]);
	}
	else {
		LR0ItemsAfterDot[ENDDING].push_back(core[core.size() - 1]);
	}
}

void LR0ItemSet::insert(LR0Item& item)
{
	extension.push_back(item);
	if (item.dot_pos < productions[item.prod_num].alpha.size())
	{
		LR0ItemsAfterDot[productions[item.prod_num].alpha[item.dot_pos]].push_back(extension[extension.size() - 1]);
	}
	else {
		LR0ItemsAfterDot[ENDDING].push_back(extension[extension.size() - 1]);
	}
}

//one pass process: canonical collection + parsing table
void InitialCanonicalCollection()
{
	//Initial First LR0Item Set
	LR0Item sPrimeItem(0, 0);
	LR0ItemSet& I0 = *(new LR0ItemSet());
	I0.addCore(sPrimeItem);
	I0.closure();
	CanonicalCollection.push_back(std::ref(I0));

	for (int i = 0; i < CanonicalCollection.size(); i++)
	{
		LR0ItemSet& itemSet = CanonicalCollection[i].get();
		//首先计算goto
		//Step1. 计算dot不在结尾的每一个goto
		for (int j = 0; j < ENDDING; j++)
		{
			Symbol gotoSymbol = (Symbol)j;
			list<LR0Item>::iterator it = (itemSet.LR0ItemsAfterDot)[gotoSymbol].begin();
			LR0ItemSet& temptSet = *(new LR0ItemSet());

			while (it != (itemSet.LR0ItemsAfterDot)[gotoSymbol].end())
			{
				LR0Item rightShiftedItem((*it).prod_num, (*it).dot_pos + 1);
				temptSet.addCore(rightShiftedItem);
				it++;
			}
			if (temptSet.core.size() != 0)//core中存在元素
			{
				//判断temptSet不存在于CanonicalCollections中
				bool isExisted = false;
				int k;
				for (k = 0; k < CanonicalCollection.size(); k++)
				{
					LR0ItemSet& existingSet = CanonicalCollection[k].get();
					if (existingSet == temptSet)
					{
						isExisted = true;
						break;
					}
				}
				//如果存在则需要释放内存
				if (isExisted)
				{
					delete &temptSet;
				}
				else {
					temptSet.closure();
					CanonicalCollection.push_back(std::ref(temptSet));
				}
				//维护itemSet的goto表
				itemSet.gotoMapping[gotoSymbol] = k;
			}
			else
			{
				delete &temptSet;
			}
		}

		auto LR0ItemsLast_it = (itemSet.LR0ItemsAfterDot)[ENDDING].begin();
		while (LR0ItemsLast_it != (itemSet.LR0ItemsAfterDot)[ENDDING].end())
		{//然后对A->a.的情况吧goto[A,b]in follow(A)设置为产生式的num
			LR0Item& tmptItem = *LR0ItemsLast_it;
			Symbol temptNT = productions[tmptItem.prod_num].NT;
			for (Symbol temptSymb : Follow[temptNT])
			{
				itemSet.gotoMapping[temptSymb] = -tmptItem.prod_num;
			}
			LR0ItemsLast_it++;
		}

	}
}

void InitialParsingTable()
{
	InitialFirst();
	InitialFollow();
	InitialCanonicalCollection();
}

int getParsingTable(int st, Symbol symb)
{
	return CanonicalCollection[st].get().gotoMapping[symb];
}

void ctrlFirstOpt()
{
	if (!isFirstOpt)
		printf("\n");
	isFirstOpt = false;
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
	prog += " $";
	/* 骚年们 请开始你们的表演 */
	/********* Begin *********/

	// initialization
	InitialMapping();
	InitialProduction();
	InitialParsingTable();

	//cout << "{" << endl;
	//for (int i=0;i<CanonicalCollection.size();i++)
	//{
	//	cout << "{";
	//	auto& vec = CanonicalCollection[i].get().gotoMapping;
	//	int j;
	//	for (j=0;j<ENDDING;j++)
	//	{
	//		int a = vec[j];
	//		if (a == inf)
	//			cout << "INF " << ",";
	//		else
	//			cout << a << " ,";
	//	}
	//	if (vec[j] == inf)
	//		cout << "INF ";
	//	else
	//		cout << vec[j];
	//	cout << "}" << endl;
	//}
	//cout << "}" << endl;


	// LL1 parsing
	stack<int> shiftreduceStack;
	vector<int> productionSeq;
	shiftreduceStack.push(0);

	istringstream progin(prog);
	string line;
	int line_count = 0;
	//error handler -- Phrase-Level Error Recovery
	string lastToken;
	bool hasErrored = false;

	// stacking
	while (getline(progin, line))
	{
		line_count++;

		istringstream linein(line);

		string token;
		if (hasErrored || (!hasErrored &&  linein >> token))
			if (hasErrored) {
				token = lastToken;
				hasErrored = false;
			}
		while (1)
		{
			Symbol inputSymb = StrToTerminalMap[token];
			int topState = shiftreduceStack.top();
			int operation = getParsingTable(topState, inputSymb);
			if (operation == inf)
			{
				//error handler routine
				ctrlFirstOpt();
				lastToken = token;
				token = ";";
				hasErrored = true;
				printf("语法错误，第%d行，缺少\"%s\"", line_count - 1, TerminalToStrMap[SEMICOLON].c_str());
			}
			else if (operation > 0)
			{
				//shift operation
				int shift = operation;
				//push symbol
				shiftreduceStack.push(inputSymb);
				//push state
				shiftreduceStack.push(shift);
				if (!hasErrored && !(linein >> token))
				{
					break;
				}
				else if (hasErrored) {
					token = lastToken;
					hasErrored = false;
				}
			}
			else if (operation < 0) {
				//reduce
				int reduce = -operation;
				for (int i = 0; i < productions[reduce].alpha.size() * 2; i++)
				{
					shiftreduceStack.pop();
				}
				//on the top should be the State, push the left side of production
				Symbol topSymb = productions[reduce].NT;
				int topGoState = shiftreduceStack.top();
				shiftreduceStack.push((int)topSymb);
				int state_to_push = getParsingTable(topGoState, topSymb);
				shiftreduceStack.push(state_to_push);
				productionSeq.push_back(reduce);
			}
			else {
				break;
			}
		}

	}

	vector<Symbol> Left_NTT_Stack;//当前串最右非终结符（包含）左侧
	vector<Symbol> Right_T_S;//当前串最右非终结符右侧串
	Left_NTT_Stack.push_back(program);
	ctrlFirstOpt();
	int j;
	for (j = 0; j < Left_NTT_Stack.size(); j++)
	{
		printf("%s ", TerminalToStrMap[Left_NTT_Stack[j]].c_str());
	}
	for (j = Right_T_S.size() - 1; j >= 0; j--)
	{
		printf("%s ", TerminalToStrMap[Right_T_S[j]].c_str());
	}
	for (int i = productionSeq.size() - 1; i >= 0; i--)
	{
		//优化：利用vector和queue优化输出至线性时间
		Production& prod = productions[productionSeq[i]];
		Left_NTT_Stack.pop_back();

		for (j = 0; j < prod.alpha.size(); j++)
		{
			Left_NTT_Stack.push_back(prod.alpha[j]);
		}
		while (Left_NTT_Stack.size() != 0 && Left_NTT_Stack[Left_NTT_Stack.size() - 1] > T)
		{
			Symbol temp_symb = Left_NTT_Stack[Left_NTT_Stack.size() - 1];
			Left_NTT_Stack.pop_back();
			Right_T_S.push_back(temp_symb);
		}
		//输出串
		printf("=> ");
		ctrlFirstOpt();
		for (j = 0; j < Left_NTT_Stack.size(); j++)
		{
			printf("%s ", TerminalToStrMap[Left_NTT_Stack[j]].c_str());
		}
		for (j = Right_T_S.size() - 1; j >= 0; j--)
		{
			printf("%s ", TerminalToStrMap[Right_T_S[j]].c_str());
		}

	}
	/********* End *********/

}
