#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
    int 	Itemp;
	double   Rtemp;
    string	Stemp;
    
       
public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(double vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
    
    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}
    bool IsInt() const { return T == VINT; }
    
    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }
    
    double GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}
    
    void SetType(ValType type)
    {
    	T = type;
	}
	
	void SetInt(int val)
    {
    	Itemp = val;
	}
	
	void SetReal(double val)
    {
    	Rtemp = val;
	}
	
	void SetString(string val)
    {
    	Stemp = val;
	}
	
	void SetBool(bool val)
    {
    	Btemp = val;
	}
    Value val1() const{
        if(this->IsInt())       return Value((double)(this->GetInt()));
        else if(this->IsReal()) return Value(this->GetReal());
        else if(this->IsString()){
            try {
                return Value(stod(this->GetString()));
            }
            catch( invalid_argument & arg){
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
        }
        return Value();
    }
    Value val(Value val) const{
        if(val.IsInt())       return Value((double)val.GetInt());
        else if(val.IsReal()) return Value(val.GetReal());
        else if(val.IsString()){
            try {
                return Value(stod(val.GetString()));
            }
            catch( invalid_argument & arg){
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
        }
        return Value();
    }
    // numeric overloaded add op to this
    Value operator+(const Value& op) const{
        if(val1().IsErr() || val(op).IsErr()){
            return Value();
        }
        else if(this->IsInt() && op.IsInt()){
            return Value(this->GetInt() + op.GetInt());
        }
        else if(val1().IsReal() || val(op).IsReal())
            return Value(val1().GetReal() + val(op).GetReal());
        return Value();
    };
    
    // numeric overloaded subtract op from this
    Value operator-(const Value& op) const{
        if(val1().IsErr() || val(op).IsErr()){
            return Value();
        }
        else if(this->IsInt() && op.IsInt()){
            return Value(this->GetInt() - op.GetInt());
        }
        else if(val1().IsReal() || val(op).IsReal())
            return Value(val1().GetReal() - val(op).GetReal());
        return Value();
    };
    
    // numeric overloaded multiply this by op
    Value operator*(const Value& op) const{
        if(val1().IsErr() || val(op).IsErr()){
            return Value();
        }
        else if(this->IsInt() && op.IsInt()){
            return Value(this->GetInt() * op.GetInt());
        }
        else if(val1().IsReal() || val(op).IsReal())
            return Value(val1().GetReal() * val(op).GetReal());
        return Value();
    };
    
    // numeric overloaded divide this by op
    Value operator/(const Value& op) const{
        if(val(op).IsReal()){
            if (val(op).GetReal() == 0){
                cout << "Run-Time Error-Illegal Division by Zero" << endl;
                return Value();
            }
        }
        if(val1().IsErr() || val(op).IsErr()){
            return Value();
        }
        else if(this->IsInt() && op.IsInt()){
            return Value(this->GetInt() / op.GetInt());
        }
        else if(val1().IsReal() || val(op).IsReal())
            return Value(val1().GetReal() / val(op).GetReal());
        return Value();
    };
    //numeric overloaded equality operator of this with op
    Value operator==(const Value& op) const{
        if(val1().IsErr())      return Value();
        if(op.IsInt()){
            if(val1().GetReal() == op.GetInt())     return Value(true);
            return Value(false);
        }
        else if(op.IsReal()){
            if(val1().GetReal() == op.GetReal())    return Value(true);
            return Value(false);
        }
        else if(op.IsString()){
            if(val1().GetString() == op.GetString())    return Value(true);
            return Value(false);
        }
        return Value();
        };
	//numeric overloaded greater than operator of this with op
	Value operator>(const Value& op) const{
        if(val1().IsErr())      return Value();
        if(op.IsInt())     return Value(val1().GetReal() > op.GetInt());
        else if(op.IsReal())         return Value(val1().GetReal() > op.GetReal());
        else if(op.IsString()){
            try {
                return Value(val1().GetReal() > stod(op.GetString()));
            }
            catch( invalid_argument & arg){
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
        }
        return Value();
    };
	//numeric overloaded less than operator of this with op
	Value operator<(const Value& op) const{
        if(val1().IsErr())      return Value();
        if(op.IsInt())          return Value(val1().GetReal() < op.GetInt());
        else if(op.IsReal())    return Value(val1().GetReal() < op.GetReal());
        else if(op.IsString()){
            try {
                return Value(val1().GetReal() < stod(op.GetString()));
            }
            catch( invalid_argument & arg){
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
        }
        return Value();
    };
	
	//Numeric exponentiation operation this raised to the power of op 
	Value operator^(const Value& oper) const{
        if(val1().IsErr())      return Value();
        if(oper.IsInt())          return Value(pow(val1().GetReal(), oper.GetInt()));
        else if(oper.IsReal())    return Value(pow(val1().GetReal(), oper.GetReal()));
        else if(oper.IsString()){
            try {
                return Value(pow(val1().GetReal(), stod(oper.GetString())));
            }
            catch( invalid_argument & arg){
                cout << "Invalid conversion from string to double." << endl;
                return Value();
            }
        }
        return Value();
    };
	
	//string concatenation operation of this with op
	Value Catenate(const Value& oper) const{
        string str1;
        string str2;
        if(this->IsInt())
            str1 = to_string(this->GetInt());
        else if(this->IsReal()){
            str1 = to_string(this->GetReal());
            while(str1.at(str1.length()-1) == '0'){
                str1 = str1.substr(0, str1.length()-1);
            }
            if(str1.at(str1.length()-1) == '.'){
                str1 = str1.substr(0, str1.length()-1);
            }
        }
        else if(this->IsString()){
            str1 = this->GetString();
        }
        else{
            return Value();
        }
        if(oper.IsInt()){
            str2 = to_string(oper.GetInt());
        }
        else if(oper.IsReal()){
            str2 = to_string(oper.GetReal());
            while(str2.at(str2.length()-1) == '0'){
                str2 = str2.substr(0, str2.length()-1);
            }
            if(str2.at(str2.length()-1) == '.'){
                str2 = str2.substr(0, str2.length()-1);
            }
        }
        else if(oper.IsString()){
            str2 = oper.GetString();
        }
        else{
            return Value();
        }
        return Value(str1+str2);
    };
	//string repetition operation of this with op
	Value Repeat(const Value& oper) const{
        if(this->IsString()){
            string word;
            int repeat;
            if(oper.IsInt())        repeat = oper.GetInt();
            else if(oper.IsReal())  repeat = (int)oper.GetReal();
            else                    return Value();
            for(int x = 0; x < repeat; x++){
                word.append(this->GetString());
            }
            return Value(word);
        }
        return Value();
    };
	//string equality (-eq) operator of this with op
	Value SEqual(const Value& oper) const{
        if(this->IsString() && oper.IsString()){
            return Value(bool(this->GetString() == oper.GetString()));
        }
        return Value();
    };
	//string greater than (-gt) operator of this with op
	Value SGthan(const Value& oper) const{
        string str1;
        string str2;
        if(this->IsInt())
            str1 = to_string(this->GetInt());
        else if(this->IsReal()){
            str1 = to_string(this->GetReal());
            while(str1.at(str1.length()-1) == '0'){
                str1 = str1.substr(0, str1.length()-1);
            }
            if(str1.at(str1.length()-1) == '.'){
                str1 = str1.substr(0, str1.length()-1);
            }
        }
        else if(this->IsString()){
            str1 = this->GetString();
        }
        else{
            return Value();
        }
        if(oper.IsInt()){
            str2 = to_string(oper.GetInt());
        }
        else if(oper.IsReal()){
            str2 = to_string(oper.GetReal());
            while(str2.at(str2.length()-1) == '0'){
                str2 = str2.substr(0, str2.length()-1);
            }
            if(str2.at(str2.length()-1) == '.'){
                str2 = str2.substr(0, str2.length()-1);
            }
        }
        else if(oper.IsString()){
            str2 = oper.GetString();
        }
        else{
            return Value();
        }
        return Value(str1>str2);
    };
	//string less than operator of this with op
	Value SLthan(const Value& oper) const{
        string str1;
        string str2;
        if(this->IsInt())
            str1 = to_string(this->GetInt());
        else if(this->IsReal()){
            str1 = to_string(this->GetReal());
            while(str1.at(str1.length()-1) == '0'){
                str1 = str1.substr(0, str1.length()-1);
            }
            if(str1.at(str1.length()-1) == '.'){
                str1 = str1.substr(0, str1.length()-1);
            }
        }
        else if(this->IsString()){
            str1 = this->GetString();
        }
        else{
            return Value();
        }
        if(oper.IsInt()){
            str2 = to_string(oper.GetInt());
        }
        else if(oper.IsReal()){
            str2 = to_string(oper.GetReal());
            while(str2.at(str2.length()-1) == '0'){
                str2 = str2.substr(0, str2.length()-1);
            }
            if(str2.at(str2.length()-1) == '.'){
                str2 = str2.substr(0, str2.length()-1);
            }
        }
        else if(oper.IsString()){
            str2 = oper.GetString();
        }
        else{
            return Value();
        }
        return Value(str1<str2);
    };
	    
    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << fixed << setprecision(1) << double(op.Itemp);
        else if( op.IsString() ) out << op.Stemp;
        else if( op.IsReal()) out << fixed << setprecision(1) << op.Rtemp;
        else if(op.IsBool()) out << (op.GetBool()? "true" : "false");
        else out << "ERROR";
        return out;
    }
};
#endif