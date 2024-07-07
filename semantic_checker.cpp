
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Node {

    string symbol;
    vector<Node *> children;

    bool is_leaf;
    string pr;
    string lexeme;
    int type;

    Node(string symbol, bool is_leaf = false, string pr = "", string lexeme = "", int type = -1) {
        this->symbol = symbol;
        this->is_leaf = is_leaf;
        this->pr = pr;
        this->lexeme = lexeme;
        this->type = type;
    }  
};

std::istream& in = std::cin;

map<string, bool> has_procedure;
map<string, vector<string>> signature;
map<string, map<string, int>> dcls; // 0 for uninitialized, 1 for int, 2 for int*

map<string, vector<string>> param_list;
map<string, map<string, int>> params;



Node* build_tree() {

    string s = "";
    if (!getline(in, s)) {

        return nullptr;
        
    } else {

        std::istringstream line(s);
        string symbol;
        line >> symbol;

        if (symbol == "ID" || 
            symbol == "INT" || 
            symbol == "BOF" ||
            symbol == "EOF" ||
            symbol == "WAIN" ||
            symbol == "LPAREN" ||
            symbol == "RPAREN" ||
            symbol == "COMMA" ||
            symbol == "LBRACE" ||
            symbol == "RBRACE" ||
            symbol == "SEMI" ||
            symbol == "RETURN" ||
            symbol == "STAR" ||
            symbol == "NUM" ||
            symbol == "NULL" ||
            symbol == "BECOMES" ||
            symbol == "PLUS" ||
            symbol == "MINUS" ||
            symbol == "SLASH" ||
            symbol == "PCT" ||
            symbol == "AMP" ||
            symbol == "NEW" ||
            symbol == "LBRACK" ||
            symbol == "RBRACK" ||
            symbol == "GETCHAR" ||
            symbol == "PUTCHAR" ||
            symbol == "IF" ||
            symbol == "ELSE" ||
            symbol == "WHILE" ||
            symbol == "PRINTLN" ||
            symbol == "PUTCHAR" ||
            symbol == "DELETE" ||
            symbol == "EQ" ||
            symbol == "NE" ||
            symbol == "LT" ||
            symbol == "LE" ||
            symbol == "GE" ||
            symbol == "GT"
        ) {
            string lexeme;
            line >> lexeme;
            Node* new_node = new Node(symbol, true, s, lexeme, -1);
            return new_node;
        } else {
            Node* new_node = new Node(symbol, false, s, "", -1);
            while (line >> s) {
                if (s == ".EMPTY") {
                    break;
                }
                Node* child_node = build_tree();
                new_node->children.emplace_back(child_node);
            }
            return new_node;
        }
    }
}


void print_error(string err) {
    cerr << "ERROR : " + err << endl;
}


bool check_args(Node* node, int num_args, string called_proc) {
    
    if (node->children.size() == 1) { // expr
        if (num_args != param_list[called_proc].size()) {
            if (num_args > param_list[called_proc].size()) {
                print_error("too many args when calling " + called_proc);
            } else {
                print_error("lacking args when calling " + called_proc);
            }
            return false;
        }
    }

    int arg_type = node->children[0]->type;
    int param_type = params[called_proc][param_list[called_proc][num_args - 1]];
    if (arg_type != param_type) {
        print_error("param " + to_string(num_args - 1) + " of " + called_proc + " type mismatch");
        return false;
    }

    if (node->children.size() == 3) {
        if (!(check_args(node->children[2], num_args + 1, called_proc))) {
            return false;
        }
    }
    return true;
}

bool handle_proc_call(Node* node, string curr_proc) {

    if (node->children[0]->symbol == "ID") {

        string proc_name = node->children[0]->lexeme;

        if (dcls[curr_proc][proc_name] != 0) {
            print_error("procedure call invalid: " + proc_name + " exists as a variable in " + curr_proc);
            return false;
        }
        if (!has_procedure[proc_name]) {
            print_error("procedure call invalid: " + proc_name + " not declared");
            return false;
        }

        if (node->children.size() == 3 && param_list[proc_name].size() > 0) {
            print_error("lacking argument when calling " + proc_name + " in " + curr_proc);
            return false;
        } else if (node->children.size() > 3) {
            if (!check_args(node->children[2], 1, proc_name)) {
                return false;
            }
        }
    } else { // get char
        if (dcls[curr_proc][node->children[0]->lexeme] != 0) {
            print_error("procedure call invalid: getchar exists as a variable in " + curr_proc);
            return false;
        }
    }
    return true;
}

bool label_type(Node* node, string curr_proc) {

    if (node->symbol == "dcl") {
        node->children[1]->type = node->children[0]->children.size();
    } else if (node->symbol == "NUM") {
        node->type = 1;
    } else if (node->symbol == "NULL") {
        node->type = 2;
    } else if (node->symbol == "ID") {
        node->type = dcls[curr_proc][node->lexeme];
    } else if (node->symbol == "expr") {
        if (node->children.size() == 3) {
            if (node->children[0]->type == 2 && node->children[2]->type == 2) {
                if (node->children[1]->symbol == "PLUS") {
                    print_error("cannot add two pointers");
                    return false;
                } else {
                    node->type = 1;
                }
            } else if (node->children[0]->type == 2 || node->children[2]->type == 2) {
                node->type = 2;
            } else {
                node->type = 1;
            }
        } else {
            node->type = node->children[0]->type;
        }
    } else if (node->symbol == "factor") {
        if (node->children[0]->symbol == "NEW") {
            if (node->children[3]->type != 1) {
                print_error("got pointer as argument in NEW initialization");
                return false;
            } else {
                node->type = 2;
            }
        } else if (node->children[0]->symbol == "AMP") {
            if (node->children[1]->type != 1) {
                print_error("got pointer before &");
                return false;
            } else {
                node->type = 2;
            }
        } else if (node->children[0]->symbol == "STAR") {
            if (node->children[1]->type != 2) {
                print_error("got int before *");
                return false;
            } else {
                node->type = 1;
            } 
        } else if (node->children[0]->symbol == "GETCHAR") {
            node->type = 1;
        } else {
            if (node->children.size() >= 3 && 
                node->children[1]->symbol == "LPAREN" && 
                node->children[node->children.size() - 1]->symbol == "RPAREN") {

                    if (!handle_proc_call(node, curr_proc)) {
                        return false;
                    }
                    node->type = 1;
                        
            } else {
                int middle_idx = node->children.size() / 2;
                node->type = node->children[middle_idx]->type;
            }
        }
    } else if (node->symbol == "term") {
        if (node->children.size() == 3) {
            if (node->children[0]->type != 1 || node->children[2]->type != 1) {
                print_error("got pointer in mult, div, or pct");
                return false;
            }
        }
        node->type = node->children[0]->type;
    } else if (node->symbol == "lvalue") {
        if (node->children.size() == 1) {
            node->type = node->children[0]->type;
        } else if (node->children.size() == 2) {
            if (node->children[1]->type != 2) {
                print_error("got int factor in pr: lvalue star factor");
                return false;
            } else {
                node->type = 1;
            }
        } else if (node->children.size() == 3) {
            node->type = node->children[1]->type;
        }
    }
    return true;
}


bool init_param(Node* node, string curr_proc) {
    // node->symbol should be paramlist
    string param_name = node->children[0]->children[1]->lexeme;
    int param_type = node->children[0]->children[0]->children.size();

    if (params[curr_proc][param_name] != 0) {
        print_error("duplicate param in " + curr_proc);
        return false;
    }
    param_list[curr_proc].emplace_back(param_name);
    params[curr_proc][param_name] = param_type;
    dcls[curr_proc][param_name] = param_type;

    if (node->children.size() == 3) {
        if (!init_param(node->children[2], curr_proc)) {
            return false;
        }
    }
    return true;
}

bool init_proc(Node* node) {
    string proc_name = node->children[1]->lexeme;
    
    if (has_procedure[proc_name]) {
        print_error("duplicate procedure declaration");
        return false;
    }
    has_procedure[proc_name] = true;

    if (node->children[3]->children.size() > 0) {
        if (!init_param(node->children[3]->children[0], proc_name)) {
            return false;
        }
    }
    return true;
}

bool init_wain(Node* node) {

    string param1name = node->children[3]->children[1]->lexeme;
    string param2name = node->children[5]->children[1]->lexeme;
    int param1type = node->children[3]->children[0]->children.size();
    int param2type = node->children[5]->children[0]->children.size();

    if (param1name == param2name) {
        print_error("duplicate param name in wain");
        return false;
    } 
    if (param2type != 1) {
        print_error("param 2 is not of type int");
        return false;
    }

    dcls["wain"][param1name] = param1type;
    dcls["wain"][param2name] = param2type;
    params["wain"][param1name] = param1type;
    params["wain"][param2name] = param2type;

    return true;
}

bool check_return_type(Node* node, string curr_proc) {

    if (node->symbol != "main" && node->symbol != "procedure") {
        return true;
    } else if (node->symbol == "main") {
        if (node->children[11]->type != 1) {
            print_error("return type of " + curr_proc + " is not int");
            return false;
        }
    } else if (node->symbol == "procedure") {
        if (node->children[9]->type != 1) {
            print_error("return type of " + curr_proc + " is not int");
            return false;
        }
    }
    return true;
}

bool handle_dcl(Node* node, string curr_proc) {
    if (node->symbol != "dcls") {
        return true;
    } else {
        if (node->children.size() == 0) { // .EMPTY
            return true;
        }

        string param_name = node->children[1]->children[1]->lexeme;
        int param_type = node->children[1]->children[1]->type;

        if (dcls[curr_proc][param_name] != 0) {
            print_error("duplicate var declaration of " + param_name + " in " + curr_proc);
            return false;
        } else {
            dcls[curr_proc][param_name] = param_type;
        }

        if (param_type == 1 && node->children[3]->symbol != "NUM") {
            print_error("type assignment mismatch at declaration of " + param_name + " in " + curr_proc);
            return false;
        }

        if (param_type == 2 && node->children[3]->symbol != "NULL") {
            print_error("type assignment mismatch at declaration of " + param_name + " in " + curr_proc);
            return false;
        }
        
        
        if (param_type != node->children[3]->type) { // type assignment mismatch
            print_error("type assignment mismatch at declaration of " + param_name + " in " + curr_proc);
            return false;
        }
        

    }
    return true;
}


bool handle_stmt(Node* node, string curr_proc) {

    if (node->symbol == "test") {
        if (node->children.size() != 3) {
            print_error("test doesn't have exactly 3 children");
            return false;
        }
        if (node->children[0]->type == 0 || node->children[2]->type == 0) {
            print_error("operands of test uninitialized");
            return false;
        }
        if (node->children[0]->type != node->children[2]->type) {
            print_error("operands of test are not of the same type");
            return false;
        }
    } else if (node->symbol == "statement") {
        if (node->children[1]->symbol == "BECOMES") {

            if (node->children[0]->type == 0 || node->children[2]->type == 0) {
                print_error("operand of value assignment uninitialized");
                return false;
            }
            if (node->children[0]->type != node->children[2]->type) {
                print_error("operand of value assignment are of different types");
                return false;
            }

        } else if (node->children[0]->symbol == "PRINTLN") {
            if (node->children[2]->type != 1) {
                print_error("println arg not an int");
                return false;
            }
        } else if (node->children[0]->symbol == "PUTCHAR") {
            if (node->children[2]->type != 1) {
                print_error("putchar arg not an int");
                return false;
            }
        } else if (node->children[0]->symbol == "DELETE") {
            if (node->children[3]->type != 2) {
                print_error("delete arg not a pointer");
                return false;
            }
        }
    }
    return true;
}

void post_edit(Node* node, string curr_proc) {
    if (node->symbol == "procedure") {
        node->children[1]->type = 0;
    }
}

bool traverse_tree(Node* root_node, string curr_proc) {
    if (root_node->symbol == "procedures") {
        if (root_node->children[0]->symbol == "main") { // main
            if (!init_wain(root_node->children[0])) {
                return false;
            }
            curr_proc = "wain";
        } else {
            if (!init_proc(root_node->children[0])) {
                return false;
            }
            curr_proc = root_node->children[0]->children[1]->lexeme;
        }
    }
    
 
    for (auto child: root_node->children) {
        if (!traverse_tree(child, curr_proc)) {
            return false;
        }
    }

    if (!label_type(root_node, curr_proc)) {
        return false;
    }


    if (!check_return_type(root_node, curr_proc)) {
        return false;
    }

    if (!handle_dcl(root_node, curr_proc)) {
        return false;
    }

    if (!handle_stmt(root_node, curr_proc)) {
        return false;
    }

    post_edit(root_node, curr_proc);

    return true;
}


void print_tree(Node* root_node) {
    if (root_node->type == 1) {
        cout << root_node->pr << " : int" << endl;
    } else if (root_node->type == 2) {
        cout << root_node->pr << " : int*" << endl;
    } else {
        cout << root_node->pr << endl;
    }
    for (auto child: root_node->children) {
        print_tree(child);
    }
}

void clearMemory(Node *root) {
    for (auto child: root->children) {
        clearMemory(child);
        delete child;
    }
    root->children.clear();
}


int main(int argc, char *argv[]) {

    Node* untyped_tree = build_tree();

    
    if (traverse_tree(untyped_tree, "")) {
        print_tree(untyped_tree);
    } else {
        clearMemory(untyped_tree);
        delete untyped_tree;
        
        return 1;
    }
    

    clearMemory(untyped_tree);
    delete untyped_tree;
    return 0;
}
