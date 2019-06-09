#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <ctype.h>
using namespace::std;

//---------------------------------------------------------------------------------------------------------------------
//key到编号的映射关系
enum class TAG
{
	//keyword
	AUTO = 1,
	BREAK = 2,
	CASE = 3,
	CHAR = 4,
	CONST = 5,
	CONTINUE = 6,
	DEFAULT = 7,
	DO = 8,
	DOUBLE = 9,
	ELSE = 10,
	ENUM = 11,
	EXTERN = 12,
	FLOAT = 13,
	FOR = 14,
	GOTO = 15,
	IF = 16,
	INT = 17,
	LONG = 18,
	REGISTER = 19,
	RETURN = 20,
	SHORT = 21,
	SIGNED = 22,
	SIZEOF = 23,
	STATIC = 24,
	STRUCT = 25,
	SWITCH = 26,
	TYPEDEF = 27,
	UNION = 28,
	UNSIGNED = 29,
	VOID = 30,
	VOLATILE = 31,
	WHILE = 32,

	//op and boundary
	MINUS = 33,
	DECREMENT = 34,
	MINUSEQUAL = 35,
	ARROW = 36,
	NOT = 37,
	NE = 38,
	MOD = 39,
	MODEUQAL = 40,
	BITWISEAND = 41,
	AND = 42,
	BITWISEANDEQUAL = 43,
	LBRACKET = 44,
	RBRACKET = 45,
	MULTIPLY = 46,
	MULTIPLYEQUAL = 47,
	COMMA = 48,
	DOT = 49,
	DIVIDE = 50,
	DIVIDEEQUAL = 51,
	COLON = 52,
	SEMICOLON = 53,
	CONDTERNARY = 54,
	LSBRACKET = 55,
	RSBRACKET = 56,
	BITWISEEXCLUSIVEOR = 57,
	BITWISEEXCLUSIVEOREQUAL = 58,
	LCBRACKET = 59,
	BITWISEOR = 60,
	OR = 61,
	BITWISEOREQUAL = 62,
	RCBRACKET = 63,
	COMPLEMENT = 64,
	ADD = 65,
	INCREMENT = 66,
	ADDEQUAL = 67,
	LS = 68,
	LEFTSHIFT = 69,
	LEFTSHIFTEUQAL = 70,
	LE = 71,
	ASSIGN = 72,
	EQ = 73,
	GT = 74,
	GE = 75,
	RIGHTSHIFT = 76,
	RIGHTSHIFTEUQAL = 77,
	QUOTE = 78,
	COMMET = 79,
	NUM = 80,

	//id
	ID = 81,

	//ERROR
	ERROR = 251
};

//---------------------------------------------------------------------------------------------------------------------
//基类Token
class Token
{
public:
	const TAG tag;
	string lexeme;
	Token(TAG t, string lex) :tag{ t }
	{
		lexeme = lex;
	}
	virtual void get() const//使用virtual方法输出
	{
		printf("%s", lexeme.c_str());
	}
};
//常数：继承Token
class Num:public Token
{
public:
	int value;
	Num(int n) :Token{ TAG::NUM,"" }, value{ n }
	{

	}

	virtual void get() const
	{
		printf("%d", value);
	}
};
//自然数：继承Token
class Real: public Token
{
public:
	float value;
	Real(float n) :Token{ TAG::NUM,"" }, value{ n }
	{

	}
	virtual void get() const
	{
		printf("%f", value);
	}
};

//---------------------------------------------------------------------------------------------------------------------
//全局变量
unordered_map<string, Token> tokens;//保留字
char nextC=' ';//peek的字符
int pos =0;//peek的位置
int count_tokens = 1;
bool isFirst = true;

//---------------------------------------------------------------------------------------------------------------------
//存储保留字至map
void saveToTokens(Token token)
{
	tokens.insert(pair<string, Token>(token.lexeme, token));
}
//保留字初始化
void tokens_init()
{
	//keyword
	saveToTokens(Token(TAG::AUTO, "auto"));	//AUTO = 1,
	saveToTokens(Token(TAG::BREAK, "break"));	//BREAK = 2,
	saveToTokens(Token(TAG::CASE, "case"));	//CASE = 3,
	saveToTokens(Token(TAG::CHAR, "char"));	//CHAR = 4,
	saveToTokens(Token(TAG::CONST, "const"));	//CONST = 5,
	saveToTokens(Token(TAG::CONTINUE, "continue"));	//CONTINUE = 6,
	saveToTokens(Token(TAG::DEFAULT, "default"));	//DEFAULT = 7,
	saveToTokens(Token(TAG::DO, "do"));	//DO = 8,
	saveToTokens(Token(TAG::DOUBLE, "double"));	//DOUBLE = 9,
	saveToTokens(Token(TAG::ELSE, "else"));	//ELSE = 10,
	saveToTokens(Token(TAG::ENUM, "enum"));	//ENUM = 11,
	saveToTokens(Token(TAG::EXTERN, "extern"));	//EXTERN = 12,
	saveToTokens(Token(TAG::FLOAT, "float"));	//FLOAT = 13,
	saveToTokens(Token(TAG::FOR, "for"));	//FOR = 14,
	saveToTokens(Token(TAG::GOTO, "goto"));	//GOTO = 15,
	saveToTokens(Token(TAG::IF, "if"));	//IF = 16,
	saveToTokens(Token(TAG::INT, "int"));	//INT = 17,
	saveToTokens(Token(TAG::LONG, "long"));	//LONG = 18,
	saveToTokens(Token(TAG::REGISTER, "register"));	//REGISTER = 19,
	saveToTokens(Token(TAG::RETURN, "return"));	//RETURN = 20,
	saveToTokens(Token(TAG::SHORT, "short"));	//SHORT = 21,
	saveToTokens(Token(TAG::SIGNED, "signed"));	//SIGNED = 22,
	saveToTokens(Token(TAG::SIZEOF, "sizeof"));	//SIZEOF = 23,
	saveToTokens(Token(TAG::STATIC, "staitc"));	//STATIC = 24,
	saveToTokens(Token(TAG::STRUCT, "struct"));	//STRUCT = 25,
	saveToTokens(Token(TAG::SWITCH, "switch"));	//SWITCH = 26,
	saveToTokens(Token(TAG::TYPEDEF, "typedef"));	//TYPEDEF = 27,
	saveToTokens(Token(TAG::UNION, "union"));	//UNION = 28,
	saveToTokens(Token(TAG::UNSIGNED, "unsigned"));	//UNSIGNED = 29,
	saveToTokens(Token(TAG::VOID, "void"));	//VOID = 30,
	saveToTokens(Token(TAG::VOLATILE, "volatitle"));	//VOLATILE = 31,
	saveToTokens(Token(TAG::WHILE, "while"));	//WHILE = 32,
}

//---------------------------------------------------------------------------------------------------------------------
//程序读取：1返回next的实际字符 2位置右移 3更新peek的字符
char my_getc(string &prog)
{
	if (pos >= (signed)(prog.length()))
		return EOF;
	else
	{
		char tc = prog[pos++];
		if (pos >= (signed)(prog.length()))
		{
			nextC=EOF;
		}
		else {
			nextC = prog[pos];
		}
		return tc;
	}

}
//确定性有限自动机运行方法
Token* my_run_NFA(string &prog)
{
	bool isComment = false;

	//skip直到nextc不是/w符号
	while (nextC != EOF)
	{
		if (nextC == ' ' || nextC == '\n' || nextC == '\t')
		{
			my_getc(prog);
			continue;
		}
		else
		{
			break;
		}
	}

	//如果nextc是EOF则终止，异常退出
	if (nextC == EOF)
	{
		return new Token(TAG::ERROR, " ");
	}

	//取出真实的nextc并且nextc右移
	char tempt_char = my_getc(prog);

	//界符&运算符的自动机
	//trick：使用nextC=' '来占有nextC保证自动机跳过nextC的读取，否则不占有nextC保证自动机的读取
	switch (tempt_char)
	{
	case '-':
	{
		if (nextC == '-')
		{
			nextC = ' ';
			return new Token(TAG::DECREMENT, "--");
		}
		else if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::MINUSEQUAL, "-=");
		}
		else if (nextC == '>') {
			nextC = ' ';
			return new Token(TAG::ARROW, "->");
		}
		else
			return new Token(TAG::MINUS, "-");
	}
	case '!':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::NE, "!=");
		}
		else
			return new Token(TAG::NOT, "!");
	}
	case '%':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::MODEUQAL, "%=");
		}
		else
			return new Token(TAG::MOD, "%");
	}
	case '&':
	{
		if (nextC == '&') {
			nextC = ' ';
			return new Token(TAG::AND, "&&");
		}
		else if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::BITWISEANDEQUAL, "&=");
		}
		else
			return new Token(TAG::BITWISEAND, "&");
	}
	case '(':
	{
		return new Token(TAG::LBRACKET, "(");
	}
	case ')':
	{
		return new Token(TAG::RBRACKET, ")");
	}
	case '*':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::MULTIPLYEQUAL, "*=");
		}
		else
			return new Token(TAG::MULTIPLY, "*");
	}
	case ',':
	{
		return new Token(TAG::COMMA, ",");
	}
	case '.':
	{
		return new Token(TAG::DOT, ".");
	}
	case '/':
	{
		// comment的特殊处理
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::DIVIDEEQUAL, "/=");
		}
		else if (nextC == '/' || nextC == '*') {
			//todo: we got two types of commets
			isComment = true;
			break;
		}
		else
			return new Token(TAG::DIVIDE, "/");
	}
	case ':':
	{
		return new Token(TAG::COLON, ":");
	}
	case ';':
	{
		return new Token(TAG::SEMICOLON, ";");
	}
	case '?':
	{
		return new Token(TAG::CONDTERNARY, "?");
	}
	case '[':
	{
		return new Token(TAG::LSBRACKET, "[");
	}
	case ']':
	{
		return new Token(TAG::RSBRACKET, "]");
	}
	case '^':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::BITWISEEXCLUSIVEOREQUAL, "^=");
		}
		else
			return new Token(TAG::BITWISEEXCLUSIVEOR, "^");
	}
	case '{':
	{
		return new Token(TAG::LCBRACKET, "{");
	}
	case '}':
	{
		return new Token(TAG::RCBRACKET, "}");
	}
	case '|':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::BITWISEANDEQUAL, "|=");
		}
		else if (nextC == '|') {
			nextC = ' ';
			return new Token(TAG::OR, "||");
		}
		else
			return new Token(TAG::BITWISEAND, "|");
	}
	case '~':
	{
		return new Token(TAG::COMPLEMENT, "~");
	}
	case '+':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::ADDEQUAL, "+=");
		}
		else if (nextC == '+') {
			nextC = ' ';
			return new Token(TAG::INCREMENT, "++");
		}
		else
			return new Token(TAG::ADD, "+");
	}
	case '<':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::LE, "<=");
		}
		else if (nextC == '<') {
			my_getc(prog);
			if (nextC == '=') {
				nextC = ' ';
				return new Token(TAG::LEFTSHIFTEUQAL, "<<=");
			}
			else {
				return new Token(TAG::LEFTSHIFT, "<<");
			}
		}
		else
			return new Token(TAG::LS, "<");
	}
	case '=':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::EQ, "==");
		}
		else
			return new Token(TAG::ASSIGN, "=");
	}
	case '>':
	{
		if (nextC == '=') {
			nextC = ' ';
			return new Token(TAG::GE, ">=");
		}
		else if (nextC == '>') {
			my_getc(prog);
			if (nextC == '=') {
				nextC = ' ';
				return new Token(TAG::RIGHTSHIFTEUQAL, ">>=");
			}
			else {
				return new Token(TAG::RIGHTSHIFT, ">>");
			}
		}
		else
			return new Token(TAG::GT, ">");
	}
	case '\"':
	{
		if (!isFirst)
		{
			printf("\n");
		}
		else {
			isFirst = false;
		}
		printf("%d: <\",%d>\n", count_tokens,(int)TAG::QUOTE);
		count_tokens++;
		string s;
		while (nextC != '\"')
		{
			s += nextC;
			my_getc(prog);
		}
		printf("%d: <%s,%d>", count_tokens,s.c_str(), (int)TAG::ID);
		count_tokens++;
		nextC = ' ';
		return new Token(TAG::QUOTE, "\"");
	}
	}

	//comment的自动机
	if (isComment)
	{
		if (nextC == '/') //comment
		{
			string s = "/";
			while ((nextC) != '\n')
			{
				s += nextC;
				my_getc(prog);
			}
			nextC = ' ';
			return new Token(TAG::COMMET, s);
		}
		else // /*comment*/
		{
			string s = "/*";
			my_getc(prog);
			while (1)
			{
				if (nextC == '*')
				{
					s += nextC;
					my_getc(prog);
					if (nextC == '/')
					{
						s += nextC;
						break;
					}
				}
				else {
					s += nextC;
					my_getc(prog);
				}
				// consider EOF here ERROR
			}
			nextC = ' ';
			return new Token(TAG::COMMET, s);
		}
	}

	char nowChar = prog[pos - 1];

	//标识符的自动机
	if (isalpha(nowChar))
	{
		string s;
		s += nowChar;
		while (isdigit(nextC) || isalpha(nextC))
		{
			s += nextC;
			my_getc(prog);
		}
		//保留字判断
		if (tokens.find(s) != tokens.end())
		{
			return &tokens.at(s);
		}
		//否则返回标识符
		else
		{
			return new Token(TAG::ID, s);
		}
	}

	//自然数与常数的自动机
	if (isdigit(nowChar))
	{
		int value = nowChar - '0';
		//常数读取
		while (isdigit(nextC))
		{
			value = value * 10 + nextC - '0';
			my_getc(prog);
		}
		//小数部分读取
		if (nextC == '.')
		{
			float fvalue = value;
			float dividing = 10;
			my_getc(prog);
			while (isdigit(nextC))
			{
				fvalue += (nowChar - '0' / dividing);
				dividing *= 10;
				my_getc(prog);
			}
			return new Real(fvalue);
		}
		else {
			return new Num(value);
		}
	}

	string errorMsg = "";
	errorMsg += tempt_char;
	//不被自动机接受，返回异常
	return new Token(TAG::ERROR, errorMsg);
}

//---------------------------------------------------------------------------------------------------------------------
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
	tokens_init();
	
	pos = 0;
	nextC = prog[0];
	Token* t_ptr = NULL;
	while ((t_ptr = my_run_NFA(prog)))
	{
		if (t_ptr->tag != TAG::ERROR)
		{
			if (!isFirst)
			{
				printf("\n");
			}
			else {
				isFirst = false;
			}
			printf("%d: <", count_tokens);
			t_ptr->get();
			printf(",%d>", (int)t_ptr->tag);
			count_tokens++;
		}
		else {
			break;
		}
	}
	/********* End *********/

}

//---------------------------------------------------------------------------------------------------------------------
int main()
{
	Analysis();
	return 0;
}



