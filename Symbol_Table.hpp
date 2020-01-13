//whats up with the union thing they wrote about?
//TAKE INTO ACCOUNT BYTE->INT CONVERSION

#ifndef COMPI3_SYMBOL_TABLE_HPP
#define COMPI3_SYMBOL_TABLE_HPP

#include "hw3_output.hpp"
#include <iostream>
#include <list>
#include <vector>
#include <exception>

using namespace std;

enum Datatype {VAR, ENUM_VAR, FUNC, ENUM_DEF, ENUM_VAL};

typedef struct table_elem {
    Datatype datatype;
    string name;
    string type;
    vector<string> params_type;
    int offset;

    table_elem(Datatype dt, const string& name, const string& type, int offset) :
    datatype(dt), name(name), type(type), offset(offset) {}
    table_elem(Datatype dt, const string& name, const string& retType, const vector<string>& params_type) :
    datatype(dt), name(name), type(retType), params_type(params_type.begin(), params_type.end()), offset(0) {}
} table_elem;


class Symbol_Table {
    list < list<table_elem>* >* tables;
    list<int>* offsets;
public:

    //creates a Symbol Table:
    Symbol_Table();

    //inserts normal variables:
    void insert(const string& id, const string& type);

    //inserts a variable "id" of type "enum <enumName>", POSSIBLY REDUNDANT:
    void insertEnumVar(const string& id, const string& enumName);

    //same as this^, but also check that val is of the correct enum type:
    void insertEnumVar(const string& id, const string& enumName, const string& val);

    //inserts a function and its parameters:
    void insertFunc(const string& id, const string& retType, vector<string>& params_type);

    //inserts a func's parameters:
    void insertFuncParams(const vector<string>& params_type, const vector<string>& params_id);

    //inserts an enum type definition and its values:
    void insertEnumType(const string& enum_id, vector<string>& values);

    //returns id's type (relevant for all variables, including enum variables):
    string get_var_type(const string& id);

    //returns func's return type:
    string get_func_retType(const string& func_id);

    //returns func's param types:
    vector<string>& get_func_types(const string& id);

    //returns enumVal's enum type:
    string get_enumType_of_val(const string& val);

    //returns id's datatype:
    Datatype get_datatype(const string& id);

    //returns true if id exists with the right type, else throws error (2 possible errors):
    void using_id(const string& id);

    //returns true if id exists with the right type, else throws error (2 possible errors):
    void using_id(const string& id, const string& type);

    //returns true if func's id exists and all the parameters match, else throws error (2 possible errors):
    void using_func(const string& id, vector<string>& argTypes); //SHOULD BE VOID

    //returns true if the current scope is global scope:
    bool usingGlobal();

    //return true if id is an enum variable (ENUM_VAR) defined in the global scope, else false:
    bool isGloballyDefinedEnum(const string& id);

    //opens scope:
    void openScope();

    //prints everything in scope (as they requested), then closes scope:
    void closeScope();

    //return true if main exists and it returns void, else throws error:
    bool validMain();

    //return true if main exists and it returns void, else throws error:
    bool is_enumVal(const string& id);

    //returns id's offset:
    int get_offset(const string& id);




    //THE FOLLOWING FUNCTIONS ARE NOT MEANT FOR SHIREL:
    //inserts an enum value:
    void insertEnumVals(const string& enum_type, const vector<string>& enumVals);

    //return true if id exists in table else false:
    bool id_exists(const string& id);


    class ST_undef : public exception {
		string* id;
	public:
		ST_undef(){
			id = new string();
		}
		ST_undef(const string& id_v){
			id = new string(id_v);
		}
		string& get_raised_id(){
			return *id;
		}
	};
    class ST_def : public exception {
		string* id;
	public:
		ST_def(){
			id = new string();
		}
		ST_def(const string& id_v){
			id = new string(id_v);
		}
		string& get_raised_id(){
			return *id;
		}
	};
    struct ST_undefFunc : public exception {
		string* id;
	public:
		ST_undefFunc(){
			id = new string();
		}
		ST_undefFunc(const string& id_v){
			id = new string(id_v);
		}
		string& get_raised_id(){
			return *id;
		}
	};
    struct ST_undefEnum : public exception {
		string* id;
	public:
		ST_undefEnum(){
			id = new string();
		}
		ST_undefEnum(const string& id_v){
			id = new string(id_v);
		}
		string& get_raised_id(){
			return *id;
		}};
    struct ST_undefEnumValue : public exception {
		string* id;
	public:
		ST_undefEnumValue(){
			id = new string();
		}
		ST_undefEnumValue(const string& id_v){
			id = new string(id_v);
		}
		string& get_raised_id(){
			return *id;
		}
	};
    struct ST_mismatch : public exception {};
    struct ST_prototypeMismatch : public exception {
		string* id;
		vector<string>* argTypes;
	public:
		ST_prototypeMismatch(){
			id = new string();
			argTypes = new vector<string>();
		}
		ST_prototypeMismatch(const string& id_v, const vector<string>& vec_args){
			id = new string(id_v);
			argTypes = new vector<string>(vec_args);
		}
		string& get_raised_id(){
			return *id;
		}
		vector<string>& get_raised_args(){
			return *argTypes;
		}
	};
    struct ST_errorMainMissing : public exception {};
    struct ST_empty : public exception {};

};




#endif //COMPI3_SYMBOL_TABLE_HPP
