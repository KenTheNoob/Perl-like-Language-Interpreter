/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 2
 * Spring 2023
*/

#include "parserInt.h"
#include <stack>
#include <queue>

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempResults;
std::stack<Value> s;
queue<Value> * ValQue;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << error_count << ". Line # " << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog ::= StmtList
bool Prog(istream& in, int& line)
{
	bool f1;
	LexItem tok = Parser::GetNextToken(in, line);
	
	
	if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	Parser::PushBackToken(tok);
	f1 = StmtList(in, line); 
			
	if(!f1 || ErrCount() != 0) 
	{
		ParseError(line, "Missing Program");
		return false;
	}
	else
		return true;
}//End of Prog

//StmtList ::= Stmt; { Stmt; }
bool StmtList(istream& in, int& line){
	bool status;
		
	LexItem tok;
	
	status = Stmt(in, line);
	while(status )
	{
		tok = Parser::GetNextToken(in, line);
		if(tok == DONE)
		{
            cout << endl << "(" << "DONE" << ")" << endl;
			Parser::PushBackToken(tok);
			return true;
		}
		else if(tok == RBRACES)
		{
			Parser::PushBackToken(tok);
			return true;
		}
		
		if(tok != SEMICOL)
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
		
		status = Stmt(in, line);
		
	}
			
	tok = Parser::GetNextToken(in, line);
	if(tok == ELSE )
	{
		ParseError(line, "Missing right brace.");
		return false;
	}
	else if(tok == RBRACES)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	else 
	{
		ParseError(line, "Syntactic error in Program Body.");
		return false;
	}
	
}//End of StmtList function

//Stmt ::= AssignStme | WriteLnStmt | IfStmt  
bool Stmt(istream& in, int& line){
	bool status=false;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case SIDENT: case NIDENT: 
		Parser::PushBackToken(t);

		status = AssignStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Assignment Statement.");
			return status;
		}
		break;
	case WRITELN:
		
		status = WritelnStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect Writeln Statement.");
			return status;
		}
		break;
	case IF:  
		status = IfStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect If-Statement.");
			return status;
		}
		break;
	case ELSE:
		Parser::PushBackToken(t);
		return false;
		break;
	case IDENT:
		ParseError(line, "Invalid variable name");
	
		Parser::PushBackToken(t);
		return false;
		break;
	default:
		Parser::PushBackToken(t);
		return true;
	}
	return status;
}//End of Stmt function


//WritelnStmt:= WRITELN (ExpreList) 
bool WritelnStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis of Writeln Statement");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis of Writeln Statement");
		return false;
	}	
	while (!(*ValQue).empty()){
		cout << (*ValQue).front();
		ValQue->pop();
	}
	cout << endl;
	return true;
}//End of WritelnStmt

//IfStmt:= IF (Expr) '{' StmtList '}' [ Else '{' StmtList '}' ] 
bool IfStmt(istream& in, int& line) {
	bool ex = false, status ; 
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis of If condition");
		return false;
	}
	Value val;
	ex = Expr(in, line, val);
	if(!val.IsBool()){
		ParseError(line, "Illegal Type for If statement condition.");
		return false;
	}

	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
if(val.GetBool() == true){
//added if
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis of If condition");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != LBRACES)
	{
		ParseError(line, "If Statement Syntax Error: Missing left brace");
		return false;
	}
	status = StmtList(in, line);
	if(!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Clause");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if( t != RBRACES)
	{
		ParseError(line, "If Statement Syntax Error: Missing right brace.");
		return false;
	}
}
else{
	while(t != RBRACES){
		t = Parser::GetNextToken(in, line);
	}
}
	
	t = Parser::GetNextToken(in, line);

if(val.GetBool() == false){
	if( t == ELSE ) {
		t = Parser::GetNextToken(in, line);
		if(t != LBRACES)
		{
			ParseError(line, "If Statement Syntax Error: Missing left brace");
			return false;
		}
		status = StmtList(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for Else-Clause");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if( t != RBRACES)
		{
			Parser::PushBackToken(t);
			ParseError(line, "If Statement Syntax Error: Missing right brace.");
			return false;
		}
		//Parser::PushBackToken(t);
	}
	else
	{
		Parser::PushBackToken(t);
		return true;
	}
}
else{
	while(t != RBRACES){
		t = Parser::GetNextToken(in, line);
	}
}
	return true;
}//End of IfStmt function

//Var ::= NIDENT | SIDENT
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	idtok = tok;
	if (tok == NIDENT || tok == SIDENT){
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			SymTable[identstr] = tok.GetToken();
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		
		return false;
	}
	
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem t;

	varstatus = Var( in, line, t);

	int type = t.GetToken();
	string var = t.GetLexeme();

	if (varstatus){
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP){
			Value val;
			status = Expr(in, line, val);
			TempResults[var] = val;
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statement");
				return status;
			}
			if(type == 4){
				if(val.IsString()){
					ParseError(line, "Invalid assignment statement conversion of a string value to a double variable.");
					return false;
				}
				if(val.IsBool()){
					ParseError(line, "Illegal Assignment of a boolean value to a numeric or string variable.");
					return false;
				}
			}
			if(type == 5){
				if(val.IsBool()){
					ParseError(line, "Illegal Assignment of a boolean value to a numeric or string variable.");
					return false;
				}
			}
			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value val;
	status = Expr(in, line, val);
	ValQue->push(val);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Expr ::= EqualExpr ::= RelExpr [(-EQ|==) RelExpr ]
bool Expr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	bool t1 = RelExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == NEQ || tok == SEQ ) 
	{
		t1 = RelExpr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		if(tok == NEQ){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			if((val2==val1).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2==val1;
		}
		if(tok == SEQ){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			if((val2.SEqual(val1)).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2.SEqual(val1);
		}

		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/EqualExpr

//RelExpr ::= AddExpr [ ( -LT | -GT | < | > )  AddExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	bool t1 = AddExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == NGTHAN || tok == NLTHAN || tok == SGTHAN || tok == SLTHAN ) 
	{
		t1 = AddExpr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if(tok == NGTHAN){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			retVal = Value(false);
			if((val2>val1).IsErr()){
				ParseError(line, "Illegal Relational operation.");
				return false;
			}
			retVal = val2<val1;
			s.push(retVal);
		}
		else if(tok == NLTHAN){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			retVal = Value(false);
			if((val2<val1).IsErr()){
				ParseError(line, "Illegal Relational operation.");
				return false;
			}
			retVal = val2<val1;
			s.push(retVal);
		}
		else if(tok == SGTHAN){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			retVal = Value(false);
			if((val2.SLthan(val1)).IsErr()){
				ParseError(line, "Illegal Relational operation.");
				return false;
			}
			retVal = val2.SGthan(val1);
			s.push(retVal);
		}
		else if(tok == SLTHAN){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			retVal = Value(false);
			if((val2.SLthan(val1)).IsErr()){
				ParseError(line, "Illegal Relational operation.");
				return false;
			}
			retVal = val2.SLthan(val1);
			s.push(retVal);
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//AddExpr :: MultExpr { ( + | - | .) MultExpr }
bool AddExpr(istream& in, int& line, Value & retVal) {
	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS || tok == CAT) 
	{
		t1 = MultExpr(in, line, retVal);

		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if(tok == PLUS){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "+" << val1 << "=" << val2-val1 << endl;
			if((val2+val1).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2+val1;
			s.push(retVal);
		}
		if(tok == MINUS){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "-" << val1 << "=" << val2-val1 << endl;
			if((val2-val1).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2-val1;
			s.push(retVal);
		}
		if(tok == CAT){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "." << val1 << "=" << val2.Catenate(val1) << endl;
			if((val2.Catenate(val1)).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2.Catenate(val1);
			s.push(retVal);
		}

		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);

	return true;
}//End of AddExpr

//MultExpr ::= ExponExpr { ( * | / | **) ExponExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	
	bool t1 = ExponExpr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  || tok == SREPEAT)
	{
		t1 = ExponExpr(in, line, retVal);

		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if(tok == MULT){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "*" << val1 << "=" << val2*val1 << endl;
			if((val2*val1).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2*val1;
			s.push(retVal);
		}
		else if(tok == DIV){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "/" << val1 << "=" << (val2/val1) << endl;
			if((val2/val1).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2/val1;
			s.push(retVal);
		}
		else if(tok == SREPEAT){
			Value val1(s.top());
			s.pop();
			Value val2(s.top());
			s.pop();
			//cout << val2 << "." << val1 << "=" << val2.Repeat(val1) << endl;
			if((val2.Repeat(val1)).IsErr()){
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = val2.Repeat(val1);
			s.push(retVal);
		}
	
		tok	= Parser::GetNextToken(in, line);

		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//ExponExpr ::= UnaryExpr { ^ UnaryExpr }
//enforcing right associativity using right recursiveness
bool ExponExpr(istream& in, int& line, Value & retVal)
{
	bool status;
		
	status = UnaryExpr(in, line, retVal);
	if( !status ) {
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == EXPONENT)
	{
		status = ExponExpr(in, line, retVal);
		if( !status ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		Value val1(s.top());
		s.pop();
		Value val2(s.top());
		s.pop();
		//cout << val2 << "^" << val1 << "=" << (val2^val1) << endl;
		if((val2^val1).IsErr()){
			ParseError(line, "Illegal exponentiation operation.");
			return false;
		}
		retVal = val2^val1;
		s.push(retVal);

		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	Parser::PushBackToken(tok);
	return true;
}//End of ExponExpr

//UnaryExpr ::= ( - | + ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	status = PrimaryExpr(in, line, sign, retVal);
	return status;
}//End of UnaryExpr


//PrimaryExpr ::= IDENT | NIDENT | SIDENT | ICONST | RCONST | SCONST | ( Expr )
bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	if( tok == NIDENT || tok == SIDENT) {
		
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		if(tok == SIDENT){
			retVal = TempResults.at(lexeme);
		}
		if(tok == NIDENT){
			if(sign == -1){
				if(TempResults.at(lexeme).IsInt())
					retVal = Value(-1*TempResults.at(lexeme).GetInt());
				if(TempResults.at(lexeme).IsReal())
					retVal = Value(-1*TempResults.at(lexeme).GetReal());
			}
			else{
				if(TempResults.at(lexeme).IsInt())
					retVal = Value(TempResults.at(lexeme).GetInt());
				if(TempResults.at(lexeme).IsReal())
					retVal = Value(TempResults.at(lexeme).GetReal());
			}
		}
		s.push(retVal);
		//cout << "LINE: " << line << " IDENT: " << retVal << " Lexeme: " << TempResults.at(lexeme) << endl;
		return true;
	}
	else if( tok == ICONST ) {
		if(sign == -1){
			retVal = Value(-1*stoi(tok.GetLexeme()));
		}
		else
			retVal = Value(stoi(tok.GetLexeme()));
		s.push(retVal);
		//cout << "LINE: " << line << " ICONST: " << retVal << " Lexeme: " << tok.GetLexeme() << endl;
		return true;
	}
	else if( tok == SCONST ) {
		retVal = Value(tok.GetLexeme());
		s.push(retVal);
		if(sign != 0){
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		//cout << "LINE: " << line << " SCONST: " << retVal << " Lexeme: " << tok.GetLexeme() << endl;
		return true;
	}
	else if( tok == RCONST ) {
		if(sign == -1){
			retVal = Value(-1*stod(tok.GetLexeme()));
		}
		else
			retVal = Value(stod(tok.GetLexeme()));
		s.push(retVal);
		//cout << "LINE: " << line << " RCONST: " << retVal << " Lexeme: " << tok.GetLexeme() << endl;
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}