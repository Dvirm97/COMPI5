#include "Symbol_Table.hpp"

/*
std::string& toUpper(string& str){
	for(int i = 0; i < str.size(); i ++){
		str[i] = std::toupper(str[i]);
	}
	return str;
}
vector<string>& toUpper(vector<string>& vec){
	for(int i = 0; i < vec.size() ; i++){
		vec[i] = toUpper(vec[i]);
	}
	return vec;
}
*/



Symbol_Table::Symbol_Table() {
    tables = new list < list<table_elem>* >;
    offsets = new list<int>;
    //tables->push_back(new list<table_elem>);
    //offsets->push_back(0);
}

bool Symbol_Table::id_exists(const string& id) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id) {
                return true;
            }
        }
    }
    return false;
}

void Symbol_Table::insert(const string& id, const string& type) {
    if (id_exists(id)) {
        throw ST_def(id);
    }

    //look at type aswell?

    //add symbol
    list<table_elem>* cur_scope = tables->front();
    table_elem new_symbol = table_elem(VAR, id, type, offsets->front());
    cur_scope->push_front(new_symbol);
    //increment offset
    int new_offset = offsets->front() + 1;
    offsets->pop_front();
    offsets->push_front(new_offset);
}

void Symbol_Table::insertEnumVar(const string& id, const string& enumType) {
    if (id_exists(id)) {
        throw ST_def(id);
    }

    try {
        if (get_datatype(enumType) != ENUM_DEF) {
            throw ST_undefEnum(enumType);
        }
    }catch(ST_undef& e) {
        throw ST_undefEnum(enumType);
    }
    //add symbol
    list<table_elem>* cur_scope = tables->front();
    table_elem new_symbol = table_elem(ENUM_VAR, id, enumType, offsets->front());
    cur_scope->push_front(new_symbol);
    //increment offset
    int new_offset = offsets->front() + 1;
    offsets->pop_front();
    offsets->push_front(new_offset);
}

void Symbol_Table::insertEnumVar(const string& id, const string& enumType, const string& val) {
    if (id_exists(id)) {
        throw ST_def(id);
    }
    try {
        if (get_datatype(enumType) != ENUM_DEF) {
            throw ST_undefEnum(enumType);
        }
    }catch(ST_undef& e) {
        throw ST_undefEnum(enumType);
    }
		string enumType_of_val = get_enumType_of_val(val);
        if (enumType_of_val != enumType) {
            throw ST_undefEnumValue(val);
        }
    insertEnumVar(id, enumType);
}
void Symbol_Table::insertFunc(const string& id, const string& retType, vector<string>& params_type) {
    if (id_exists(id)) {
        throw ST_def(id);
    }
    //add symbol
    list<table_elem>* cur_scope = tables->front();
    table_elem new_symbol = table_elem(FUNC, id, retType, params_type);
    cur_scope->push_front(new_symbol);
}

void Symbol_Table::insertFuncParams(const vector<string>& params_type, const vector<string>& params_id) {
    list<table_elem>* cur_scope = tables->front();
    vector<string>::const_iterator id_it, type_it;
    int param_offset;
    for (id_it = params_id.begin(), type_it = params_type.begin(), param_offset = -1;
         id_it!=params_id.end() && type_it!=params_type.end();
         id_it++, type_it++, param_offset--)
    {
        if (id_exists(*id_it)) {
            throw ST_def(*id_it);
        }
        if (*type_it == "BOOL" || *type_it == "BYTE" || *type_it == "INT" || *type_it == "STRING")
            cur_scope->push_front(table_elem(VAR, *id_it, *type_it, param_offset));
        else {
            try {
                if (get_datatype(*type_it) != ENUM_DEF) {
                    throw ST_undefEnum(*type_it);
                }
                if (!isGloballyDefinedEnum(*type_it)) {
                    throw ST_mismatch();
                }
            }catch(ST_undef& e) {
                throw ST_undefEnum(*type_it);
            }
            cur_scope->push_front(table_elem(ENUM_VAR, *id_it, *type_it, param_offset));
        }
    }
}

void Symbol_Table::insertEnumType(const string& enum_id, vector<string>& values) {
    if (id_exists(enum_id)) {
        throw ST_def(enum_id);
    }
    list<table_elem>* cur_scope = tables->front();
    cur_scope->push_front(table_elem(ENUM_DEF, enum_id, "enum", values));
    insertEnumVals(enum_id, values);
}

void Symbol_Table::insertEnumVals(const string& enum_type, const vector<string>& enumVals) {
    vector<string>::const_iterator it;
    list<table_elem>* cur_scope = tables->front();
    for (it = enumVals.begin(); it!=enumVals.end(); it++) {
        if (id_exists(*it)) {
            throw ST_def(*it);
        }
        cur_scope->push_front(table_elem(ENUM_VAL, *it, enum_type, 0));
    }
}

void Symbol_Table::using_id(const string& id) {
    if (!id_exists(id))
        throw ST_undef(id);
}

void Symbol_Table::using_id(const string& id, const string& type) { //should it be a void func? since the error is raised here anyway...
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id) {
                if ((*elem_it).type == type)
                    return;
                else {
                    throw ST_mismatch();
                }
            }
        }
    }
    throw ST_undef(id);
}

void Symbol_Table::using_func(const string& id, vector<string>& argTypes) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    bool match = true;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id) {
                if ((*elem_it).datatype != FUNC) {
                    throw ST_undefFunc(id);
                }
                vector<string>::iterator type_it, using_type_it;
                if (argTypes.size() != (*elem_it).params_type.size()) {
                    for(vector<string>::iterator param_it = (*elem_it).params_type.begin(); param_it != (*elem_it).params_type.end(); param_it++) {
                        if (*param_it != "BOOL" && *param_it != "BYTE" && *param_it != "INT" && *param_it != "STRING") {
                            *param_it = "enum " + (*param_it);
                        }
                    }
                    throw ST_prototypeMismatch(id, (*elem_it).params_type);
                }
                for (using_type_it = argTypes.begin(), type_it = (*elem_it).params_type.begin();
                     using_type_it!=argTypes.end() && type_it!=argTypes.end();
                     using_type_it++, type_it++)
                {
                    if(*type_it != *using_type_it) {
                        if(*type_it != "INT" || *using_type_it != "BYTE") {
                            for(vector<string>::iterator param_it = (*elem_it).params_type.begin(); param_it != (*elem_it).params_type.end(); param_it++) {
                                if (*param_it != "BOOL" && *param_it != "BYTE" && *param_it != "INT" && *param_it != "STRING") {
                                    *param_it = "enum " + (*param_it);
                                }
                            }
                            throw ST_prototypeMismatch(id, (*elem_it).params_type);
                        }
                    }
                }
                return;
            }
        }
    }
    throw ST_undefFunc(id);
}

string  Symbol_Table::get_var_type(const string& id) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id)
                return (*elem_it).type;
        }
    }
    throw ST_undef(id);
}

string  Symbol_Table::get_func_retType(const string& id) {
    return get_var_type(id);
}

vector<string>&  Symbol_Table::get_func_types(const string& id) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id)
                return (*elem_it).params_type;
        }
    }
    throw ST_undefFunc(id);
}

string Symbol_Table::get_enumType_of_val(const string& val) {
    try{
		string var = get_var_type(val);
		return var;
	}catch (ST_undef& e){
		throw ST_undefEnumValue(val);
	}
}

Datatype Symbol_Table::get_datatype(const string& id) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id)
                return (*elem_it).datatype;
        }
    }
    throw ST_undef(id);
}

bool Symbol_Table::usingGlobal() {
    list < list<table_elem>* > :: iterator scope_it = tables->begin();
    return (++scope_it == tables->end());
}

bool Symbol_Table::isGloballyDefinedEnum(const string& id) {
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope = tables->back();
    for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
        if ((*elem_it).name == id) {
            return ((*elem_it).datatype == ENUM_DEF);
        }
    }
    return false;
}

void  Symbol_Table::openScope() {
    tables->push_front(new list<table_elem>);
    if (!offsets->empty())
        offsets->push_front(offsets->front());
    else
        offsets->push_front(0);
}

void  Symbol_Table::closeScope() {
    /*
    output::endScope();
    list<table_elem>* cur_scope = tables->front();
    list<table_elem>::reverse_iterator elem_it;
    for (elem_it = cur_scope->rbegin(); elem_it!=cur_scope->rend(); elem_it++) {
        switch ((*elem_it).datatype) {
            case(VAR) : output::printID((*elem_it).name, (*elem_it).offset, (*elem_it).type);
                break;
            case(ENUM_VAR) : output::printID((*elem_it).name, (*elem_it).offset, "enum "+ (*elem_it).type);
                break;
            case(FUNC): {
                for(vector<string>::iterator param_it = (*elem_it).params_type.begin(); param_it != (*elem_it).params_type.end(); param_it++) {
                    if (*param_it != "BOOL" && *param_it != "BYTE" && *param_it != "INT" && *param_it != "STRING") {
                        *param_it = "enum "+(*param_it);
                    }
                }
                string func_type = output::makeFunctionType((*elem_it).type, (*elem_it).params_type);
                output::printID((*elem_it).name, 0, func_type);
            }
            default : ;
        }
    }
    for (elem_it = cur_scope->rbegin(); elem_it!=cur_scope->rend(); elem_it++) {
        if ((*elem_it).datatype == ENUM_DEF) {
            output::printEnumType((*elem_it).name, (*elem_it).params_type);
        }
    }
    */
    if (tables->empty() || offsets->empty())
        throw ST_empty();
    tables->pop_front();
    offsets->pop_front();
}

bool Symbol_Table::validMain() {
    list<table_elem>::iterator elem_it;
    list<table_elem> *scope = tables->back();
    for (elem_it = scope->begin(); elem_it != scope->end(); elem_it++) {
        if ((*elem_it).datatype == FUNC && (*elem_it).name == "main" && (*elem_it).type == "VOID" && ((*elem_it).params_type).empty()) {
            return true;
        }
    }
    throw ST_errorMainMissing();
}

bool Symbol_Table::is_enumVal(const string &id) {
    try {
        if (get_datatype(id) == ENUM_VAL)
            return true;
        else
            throw ST_undefEnumValue(id);
    }catch (ST_undef& e) {
        throw ST_undefEnumValue(e.get_raised_id());
    }
}

int Symbol_Table::get_offset(const string& id) {
    list < list<table_elem>* > :: iterator scope_it;
    list <table_elem> :: iterator elem_it;
    list<table_elem>* cur_scope;
    //checking for existence
    for (scope_it = tables->begin(); scope_it!=tables->end(); scope_it++) {
        cur_scope = *scope_it;
        for (elem_it = cur_scope->begin(); elem_it!=cur_scope->end(); elem_it++) {
            if ((*elem_it).name == id)
                return (*elem_it).offset;
        }
    }
    throw ST_undef(id);
}
