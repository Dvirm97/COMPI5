#ifndef SOURCE
#define SOURCE
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include "hw3_output.hpp"
#include "bp.hpp"
#include <cstring>
#include <sstream>
#include <iostream>

using namespace std;

class Type{
    string* type;
public:
	string& get_type() const{
		return *type;
	}
	void set_type(const string& other_type){
        type = new string(other_type);
	}
};

class Num: public Type{
	string* value_str;
public:
	void set_num(const string& num){
		value_str = new string(num);
		int value_int = stoi(num);
		if (value_int <= 255){
			set_type("BYTE");
		}
		else{
			set_type("INT");
		}
	}
	
	string& get_value() const{
		return *value_str;
	}
};

class EnumType: public Type{};

class Statement: public Type{
	vector<pair<int,BranchLabelIndex>>* true_list;
	vector<pair<int,BranchLabelIndex>>* false_list;
	vector<pair<int,BranchLabelIndex>>* next_list;
public:	

    void set_type2(const string& type1, const string& type2){
        if (type1!="VOID" ){
			set_type(type1); 
		}
		else if(type2!="VOID" ){
			set_type(type2); 
		}
		else{
			set_type("VOID");
		}
    }
	vector<pair<int,BranchLabelIndex>>& get_true_list(){
		return *true_list;
	}
	vector<pair<int,BranchLabelIndex>>& get_false_list(){
		return *false_list;
	}
	void reset(){
		true_list = new vector<pair<int,BranchLabelIndex>>();
		false_list = new vector<pair<int,BranchLabelIndex>>();
		next_list = new vector<pair<int,BranchLabelIndex>>();
	}
	void set_next_list(vector<pair<int,BranchLabelIndex>>& list){
		next_list = new vector<pair<int,BranchLabelIndex>>(list);
	}
	vector<pair<int,BranchLabelIndex>>& get_next_list(){
		return *next_list;
	}

};

class Statements : public Statement{};

class RetType: public Type{};

class Id  {
    string* id;
public:
    void set_id(const string& other_id){
        id = new string(other_id);
    }
    string& get_id() const{
        return *id;
    }
};


class Exp: public Type{
	string* enum_value;
	string* place;
public:
	void set_enumVal(const string& enumVal){
        enum_value = new string(enumVal);
    }
	void set_place(const string& new_place){
		place = new string(new_place);
	}
	 string& get_enumVal() const{
        return *enum_value;
    }
	string& get_place() const{
		return *place;
	}
	void reset(){
		enum_value = new string();
		place = new string();
	}
};

class Call: public Exp{};

class BoolExp: public Exp{
	vector<pair<int,BranchLabelIndex>>* true_list;
	vector<pair<int,BranchLabelIndex>>* false_list;
public:
	void set_true_list(vector<pair<int,BranchLabelIndex>>& item){
		true_list = new vector<pair<int,BranchLabelIndex>>(item);
	}
	void set_false_list(vector<pair<int,BranchLabelIndex>>& item){
		false_list = new vector<pair<int,BranchLabelIndex>>(item);
	}
	vector<pair<int,BranchLabelIndex>>& get_true_list(){
		return *true_list;
	}
	vector<pair<int,BranchLabelIndex>>& get_false_list(){
		return *false_list;
	}
	void BoolExp_reset(){
		reset();
		true_list = new vector<pair<int,BranchLabelIndex>>();
		false_list = new vector<pair<int,BranchLabelIndex>>();
	}
};

class Enumerator: public Id{};

class FormalDecl : public Id, public Type{};

class ExpList{

private:
    vector<Exp>* exp_list;

public:
    void insert(const Exp& exp){
        exp_list->insert(exp_list->begin(),exp);
    }

    vector<string>& get_types_vector() {
        vector<string>* type_list = new vector<string>();
        for(auto & it : *exp_list) {
            type_list->push_back(it.get_type());
        }
        return *type_list;
    }
    void join_lists(ExpList& exp_obj) {
        *exp_list = *(exp_obj.exp_list);
    }
    void reset() {
        exp_list = new vector<Exp>();
    }
};

class FormalsList {

private:
    vector<FormalDecl>* formals_list;

public:
    void insert(const FormalDecl& formal){
        formals_list->insert(formals_list->begin(),formal);
    }

    vector<string>& get_params_type() {
        vector<string>* params_type  = new vector<string>();
        for(auto & it : *formals_list)
            params_type->push_back(it.get_type());
        return *params_type;
    }
	vector<string>& get_params_ids() {
        vector<string>* params_ids = new vector<string>();
        for(auto & it : *formals_list)
            params_ids->push_back(it.get_id());
        return *params_ids;
    }
    void join_lists(FormalsList& list) {
        *formals_list = *(list.formals_list);
    }
    void reset() {
        formals_list = new vector<FormalDecl>();
    }
};

class Formals: public FormalsList{};

class EnumeratorList{

private:
    vector<Enumerator>* enumerator_list;

public:
    void insert(Enumerator& enumerator) {
        enumerator_list ->push_back(enumerator);
    }

    vector<string>& get_enumVector(){
        vector<string>* id_list  = new vector<string>();
        for(auto & it : *enumerator_list)
            id_list->push_back(it.get_id());
        return *id_list;
    }
    void join_lists(EnumeratorList& list) {
        *enumerator_list = *(list.enumerator_list);
    }
    void reset() {
        enumerator_list = new vector<Enumerator>();
    }
};

class M{
	string* quad;
	
public:
	void set_quad(string& label){
		quad = new string(label);
	}
	string& get_quad(){
		return *quad;
	}
};

class N{
	vector<pair<int,BranchLabelIndex>>* next_list;
	
public:
	void set_next_list(vector<pair<int,BranchLabelIndex>>& list){
		next_list = new vector<pair<int,BranchLabelIndex>>(list);
	}
	vector<pair<int,BranchLabelIndex>>& get_next_list(){
		return *next_list;
	}
};

union STYPE
{
    Type type;
	Num num;
	Exp exp;
	BoolExp bool_exp;
    Call call;
    EnumType enum_type;
    Statement statement;
	Statements statements;
	RetType ret_type;
	FormalDecl formal_decl;
	Id id;
    Enumerator enumerator;
	ExpList exp_list;
	FormalsList formals_list;
	Formals formals;
    EnumeratorList enumerator_list;
} ;

#define YYSTYPE STYPE

#endif