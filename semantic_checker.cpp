
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
            symbol == "RBRACK" 
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




bool label_type(Node* node, string curr_procedure) {

    if (node->symbol == "dcl") {
        node->children[1]->type = node->children[0]->children.size();
    } else if (node->symbol == "NUM") {
        node->type = 1;
    } else if (node->symbol == "NULL") {
        node->type = 2;
    } else if (node->symbol == "ID") {
        node->type = dcls[curr_procedure][node->lexeme];
    } else if (node->symbol == "expr") {
        if (node->children.size() == 3) {
            if (node->children[0]->type == 2 && node->children[2]->type == 2) {
                if (node->children[1]->symbol == "PLUS") {
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
                return false;
            }
            node->type = 2;
        } else if (node->children[0]->symbol == "AMP") {
            if (node->children[1]->type != 1) {
                return false;
            }
            node->type = 2;
        } else if (node->children[0]->symbol == "STAR") {
            if (node->children[1]->type != 2) {
                return false;
            }
            node->type = 1;
        } else {
            int middle_idx = node->children.size() / 2;
            node->type = node->children[middle_idx]->type;
        }
    } else if (node->symbol == "term") {
        if (node->children.size() == 3) {
            if (node->children[0]->type == 0) {
                return false;
            } else if (node->children[2]->type == 0) {
                return false;
            }
        }
        node->type = node->children[0]->type;
    } else if (node->symbol == "lvalue") {
        if (node->children.size() == 1) {
            node->type = node->children[0]->type;
        } else if (node->children.size() == 2) {
            if (node->children[1]->type != 2) {
                return false;
            }
            node->type = 1;
        } else if (node->children.size() == 3) {
            node->type = node->children[1]->type;
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
        return false;
    } 
    if (param2type != 1) {
        return false;
    }

    dcls["wain"][param1name] = param1type;
    dcls["wain"][param2name] = param2type;

    return true;
}

bool check_return_type(Node* node, string curr_procedure) {
    if (node->symbol != "main") {
        return true;
    } else {
        if (node->children[11]->type != 1) {
            return false;
        }
    }
    return true;
}


bool handle_dcl(Node* node, string curr_procedure) {
    if (node->symbol != "dcls") {
        return true;
    } else {
        if (node->children.size() == 0) { // .EMPTY
            return true;
        }

        string param_name = node->children[1]->children[1]->lexeme;
        int param_type = node->children[1]->children[1]->type;

        if (dcls[curr_procedure][param_name] != 0) {
            return false;
        } else {
            dcls[curr_procedure][param_name] = param_type;
        }

        if (param_type != node->children[3]->type) { // type assignment mismatch
            return false;
        }

    }
    return true;
}

bool traverse_tree(Node* root_node) {

    if (root_node->symbol == "procedures") {
        if (root_node->children[0]->symbol == "main") { // main
            if (!init_wain(root_node->children[0])) {
                return false;
            }
        } else {
            
        }
    }
    
 
    for (auto child: root_node->children) {
        if (!traverse_tree(child)) {
            return false;
        }
    }

    //cout << root_node->symbol << " " << root_node->pr << endl;
    if (!label_type(root_node, "wain")) {
        return false;
    }


    if (!check_return_type(root_node, "wain")) {
        return false;
    }

    if (!handle_dcl(root_node, "wain")) {
        return false;
    }

    return true;
}


void print_tree(Node* root_node) {
    if (root_node->type == -1) {
        cout << root_node->pr << endl;
    } else if (root_node->type == 1) {
        cout << root_node->pr << " : int" << endl;
    } else if (root_node->type == 2) {
        cout << root_node->pr << " : int*" << endl;
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

    has_procedure["wain"] = true;

    /*
    for (int i = 0; i < 14; i++) {
        cout << untyped_tree->children[1]->children[0]->children[i]->symbol << endl;
    }
    */


    if (traverse_tree(untyped_tree)) {
        print_tree(untyped_tree);
    } else {
        cerr << "ERROR" << endl;

        clearMemory(untyped_tree);
        delete untyped_tree;
        
        return 1;
    }

    clearMemory(untyped_tree);
    delete untyped_tree;
    return 0;
    
}
