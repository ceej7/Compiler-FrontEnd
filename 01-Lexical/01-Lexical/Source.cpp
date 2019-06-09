#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <ctype.h>
using namespace::std;

//---------------------------------------------------------------------------------------------------------------------
//key����ŵ�ӳ���ϵ
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
//����Token
class Token
{
public:
	const TAG tag;
	string lexeme;
	Token(TAG t, string lex) :tag{ t }
	{
		lexeme = lex;
	}
	virtual void get() const//ʹ��virtual�������
	{
		printf("%s", lexeme.c_str());
	}
};
//�������̳�Token
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
//��Ȼ�����̳�Token
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
//ȫ�ֱ���
unordered_map<string, Token> tokens;//������
char nextC=' ';//peek���ַ�
int pos =0;//peek��λ��
int count_tokens = 1;
bool isFirst = true;

//---------------------------------------------------------------------------------------------------------------------
//�洢��������map
void saveToTokens(Token token)
{
	tokens.insert(pair<string, Token>(token.lexeme, token));
}
//�����ֳ�ʼ��
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
//�����ȡ��1����next��ʵ���ַ� 2λ������ 3����peek���ַ�
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
//ȷ���������Զ������з���
Token* my_run_NFA(string &prog)
{
	bool isComment = false;

	//skipֱ��nextc����/w����
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

	//���nextc��EOF����ֹ���쳣�˳�
	if (nextC == EOF)
	{
		return new Token(TAG::ERROR, " ");
	}

	//ȡ����ʵ��nextc����nextc����
	char tempt_char = my_getc(prog);

	//���&��������Զ���
	//trick��ʹ��nextC=' '��ռ��nextC��֤�Զ�������nextC�Ķ�ȡ������ռ��nextC��֤�Զ����Ķ�ȡ
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
		// comment�����⴦��
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

	//comment���Զ���
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

	//��ʶ�����Զ���
	if (isalpha(nowChar))
	{
		string s;
		s += nowChar;
		while (isdigit(nextC) || isalpha(nextC))
		{
			s += nextC;
			my_getc(prog);
		}
		//�������ж�
		if (tokens.find(s) != tokens.end())
		{
			return &tokens.at(s);
		}
		//���򷵻ر�ʶ��
		else
		{
			return new Token(TAG::ID, s);
		}
	}

	//��Ȼ���볣�����Զ���
	if (isdigit(nowChar))
	{
		int value = nowChar - '0';
		//������ȡ
		while (isdigit(nextC))
		{
			value = value * 10 + nextC - '0';
			my_getc(prog);
		}
		//С�����ֶ�ȡ
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
	//�����Զ������ܣ������쳣
	return new Token(TAG::ERROR, errorMsg);
}

//---------------------------------------------------------------------------------------------------------------------
/* ��Ҫ�޸������׼���뺯�� */
void read_prog(string& prog)
{
	char c;
	while (scanf("%c", &c) != EOF) {
		prog += c;
	}
}
/* ���������������� */
void Analysis()
{
	string prog;
	read_prog(prog);
	/* ɧ���� �뿪ʼ���ǵı��� */
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



