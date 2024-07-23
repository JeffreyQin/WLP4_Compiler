#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


using namespace std;


struct Node {

    string symbol;
    bool is_leaf;
    string pr;
    string lexeme;
    int type;

    vector<Node *> children;

    Node(string symbol, bool is_leaf = false, string pr = "", string lexeme = "", int type = 0) {
        this->symbol = symbol;
        this->is_leaf = is_leaf;
        this->pr = pr;
        this->lexeme = lexeme;
        this->type = type;
    }
};

std::istream& in = std::cin;

map<string, map<string, int>> var_offset;

map<string, bool> terminal_symbols = {
    {"ID", true},
    {"INT", true},
    {"BOF", true},
    {"EOF", true},
    {"WAIN", true},
    {"LPAREN", true},
    {"RPAREN", true},
    {"LBRACK", true},
    {"RBRACK", true},
    {"LBRACE", true},
    {"RBRACE", true},
    {"COMMA", true},
    {"SEMI", true},
    {"RETURN", true},
    {"STAR", true},
    {"NUM", true},
    {"NULL", true},
    {"BECOMES", true},
    {"PLUS", true},
    {"MINUS", true},
    {"SLASH", true},
    {"PCT", true},
    {"AMP", true},
    {"NEW", true},
    {"GETCHAR", true},
    {"PUTCHAR", true},
    {"IF", true}, 
    {"ELSE", true},
    {"WHILE", true},
    {"PRINTLN", true},
    {"DELETE", true},
    {"EQ", true},
    {"NE", true},
    {"LT", true},
    {"LE", true},
    {"GE", true},
    {"GT", true}
};

map<string, int> proc_arg_num;
map<string, int> proc_var_num;

Node* build_tree() {
    string s = "";

    if (!getline(in, s)) {

        return nullptr;

    } else {

        std::istringstream line(s);
        string symbol = "";

        line >> symbol;

        if (terminal_symbols[symbol]) {
            string lexeme = "";
            string type = "";

            line >> lexeme;

            if (line >> type) {
                line >> type;
            }
            
            Node* new_node;
            if (type == "") {   
                new_node = new Node(symbol, true, s, lexeme, 0);
            } else if (type == "int") {
                new_node = new Node(symbol, true, s, lexeme, 1);
            } else if (type == "int*") {
                new_node = new Node(symbol, true, s, lexeme, 2);
            }
            return new_node;
        } else {
            Node* new_node = new Node(symbol, false, s);
            while (line >> s) {
                if (s == ".EMPTY") {
                    break;
                } else if (s == ":") {
                    line >> s;
                    if (s == "int") {
                        new_node->type = 1;
                    } else if (s == "int*") {
                        new_node->type = 2;
                    }
                    break;
                }

                Node* child_node = build_tree();
                new_node->children.emplace_back(child_node);
            }
            return new_node;
        }
    }
}

void clear_memory(Node *root_node) {
    for (auto child: root_node->children) {
        clear_memory(child);
        delete child;
    }
    root_node->children.clear();
}


void special_code(string code) {
    cout << code << endl;
}
void generate_code(string inst, int param1 = 0, int param2 = 0, int param3 = 0, string label = "") {
    if (inst == "add") {
        cout << "add $" << param1 << ", $" << param2 << ", $" << param3 << endl;
    } else if (inst == "sub") {
        cout << "sub $" << param1 << ", $" << param2 << ", $" << param3 << endl;
    } else if (inst == "mult") {
        cout << "mult $" << param1 << ", $" << param2 << endl;
    } else if (inst == "multu") {
        cout << "multu $" << param1 << ", $" << param2 << endl;
    } else if (inst == "div") {
        cout << "div $" << param1 << ", $" << param2 << endl;
    } else if (inst == "divu") {
        cout << "divu $" << param1 << ", $" << param2 << endl;
    } else if (inst == "mfhi") {
        cout << "mfhi $" << param1 << endl;
    } else if (inst == "mflo") {
        cout << "mflo $" << param1 << endl;
    }

    else if (inst == "slt") {
        cout << "slt $" << param1 << ", $" << param2 << ", $" << param3 << endl;
    } else if (inst == "sltu") {
        cout << "sltu $" << param1 << ", $" << param2 << ", $" << param3 << endl;
    } else if (inst == "beq") {
        cout << "beq $" << param1 << ", $" << param2 << ", " << label << endl;
    } else if (inst == "bne") {
        cout << "bne $" << param1 << ", $" << param2 << ", " << label << endl;
    }

    else if (inst == "lis") {
        cout << "lis $" << param1 << endl;
    } else if (inst == ".word") {
        if (param2 == -1) {
            cout << ".word " << label << endl;
        } else {
            cout << ".word " << param1 << endl;
        }
    } else if (inst == "sw") {
        cout << "sw $" << param1 << ", " << param2 << "($" << param3 << ")" << endl;
    } else if (inst == "lw") {
        cout << "lw $" << param1 << ", " << param2 << "($" << param3 << ")" << endl;
    } else if (inst == "jr") {
        cout << "jr $" << param1 << endl;
    } else if (inst == "jalr") {
        cout << "jalr $" << param1 << endl;
    }

    else if (inst == "label") {
        cout << label << ":" << endl;
    }

}

void store_const(int reg, int val) {
    generate_code("lis", reg);
    generate_code(".word", val);
}

void push_to_stk(int reg) {
    generate_code("sw", reg, -4, 30);
    generate_code("sub", 30, 30, 4);
}

void pop_from_stk(int reg) {
    generate_code("add", 30, 30, 4);
    generate_code("lw", reg, -4, 30);
}


void init_dcl(Node* node) {

}

void init_main_var(Node* node) {

    store_const(4, 4);
    store_const(11, 1);

    generate_code("sw", 1, -4, 30);
    generate_code("sub", 30, 30, 4);
    generate_code("sw", 2, -4, 30);
    generate_code("sub", 30, 30, 4);

    generate_code("sub", 29, 30, 4);

    var_offset["wain"][node->children[3]->children[1]->lexeme] = 8;
    var_offset["wain"][node->children[5]->children[1]->lexeme] = 4;

    proc_arg_num["wain"] = 2;
}



void init_dcls(Node* node, string curr_proc, int num_dcl) {
    if (node->children.size() == 0) { // dcls -> .EMPTY
        proc_var_num[curr_proc] = num_dcl;
    } else {
        // var declaration
        if (std::stoi(node->children[3]->lexeme) == 0) {
            generate_code("sw", 0, -4, 30);
            generate_code("sub", 30, 30, 4);
        } else {
            store_const(3, std::stoi(node->children[3]->lexeme));
            generate_code("sw", 3, -4, 30);
            generate_code("sub", 30, 30, 4);
        }

        var_offset[curr_proc][node->children[1]->children[1]->lexeme] = num_dcl * -4;
        init_dcls(node->children[0], curr_proc, num_dcl + 1);
    }
}

void generate_expr(Node* node, string curr_proc) {
    if (node->symbol == "expr") {
        if (node->children.size() == 1) {
            generate_expr(node->children[0], curr_proc);
        } else {
            generate_expr(node->children[0], curr_proc);
            push_to_stk(3);
            generate_expr(node->children[2], curr_proc);
            pop_from_stk(5);
            if (node->children[1]->symbol == "PLUS") {
                generate_code("add", 3, 5, 3);
            } else if (node->children[1]->symbol == "MINUS") {
                generate_code("sub", 3, 5, 3);
            }
        }
    } else if (node->symbol == "factor") {
        if (node->children.size() == 1) {
            if (node->children[0]->symbol == "NUM") {
                store_const(3, std::stoi(node->children[0]->lexeme));
            } else if (node->children[0]->symbol == "ID") {
                generate_code("lw", 3, var_offset[curr_proc][node->children[0]->lexeme], 29);
            }
        } else if (node->children.size() == 3) {
            if (node->children[0]->symbol == "GETCHAR") {
                generate_code("lis", 5, 0, 0);
                generate_code(".word", -1, -1, -1, "0xffff0004");
                generate_code("lw", 3, 0, 5);
            } else {
                generate_expr(node->children[1], curr_proc);
            }
        }
    } else if (node->symbol == "term") {
        if (node->children.size() == 1) {
            generate_expr(node->children[0], curr_proc);
        } else {
            generate_expr(node->children[0], curr_proc);
            push_to_stk(3);
            generate_expr(node->children[2], curr_proc);
            pop_from_stk(5);
            if (node->children[1]->symbol == "STAR") {
                generate_code("multu", 5, 3);
                generate_code("mflo", 3);
            } else if (node->children[1]->symbol == "SLASH") {
                generate_code("divu", 5, 3);
                generate_code("mflo", 3);
            } else if (node->children[1]->symbol == "PCT") {
                generate_code("divu", 5, 3);
                generate_code("mfhi", 3);
            }
        }
    }
}


void generate_test(Node* node, string curr_proc, string true_label, string false_label) {

    // check
    if (node->symbol != "test") {
        cerr << "generate_test symbol condition check failed" << endl;
    }

    generate_expr(node->children[0], curr_proc);
    push_to_stk(3);
    generate_expr(node->children[2], curr_proc);
    pop_from_stk(5);

    // $5 should store expr1, $3 should store expr2

    if (node->children[1]->symbol == "EQ") {
        generate_code("beq", 5, 3, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "NE") {
        generate_code("bne", 5, 3, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "LT") {
        generate_code("slt", 3, 5, 3);
        generate_code("beq", 3, 11, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "GT") {
        generate_code("beq", 5, 3, 0, false_label);
        generate_code("slt", 3, 5, 3);
        generate_code("beq", 3, 11, 0, false_label);
        generate_code("beq", 0, 0, 0, true_label);
    } else if (node->children[1]->symbol == "LE") {
        generate_code("beq", 5, 3, 0, true_label);
        generate_code("slt", 3, 5, 3);
        generate_code("beq", 3, 11, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "GE") {
        generate_code("slt", 3, 5, 3);
        generate_code("beq", 3, 0, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    }
}

int if_ctr = 0;
int while_ctr = 0;

void generate_stmt(Node* node, string curr_proc) {

    if (node->symbol == "statements") {
        if (node->children.size() > 0) {
            generate_stmt(node->children[0], curr_proc);
            generate_stmt(node->children[1], curr_proc);
        }
    } else if (node->symbol == "statement") {
        if (node->children[1]->symbol == "BECOMES") { // assignment
            generate_expr(node->children[2], curr_proc);

            string var_id = node->children[0]->children[0]->lexeme;
    
            generate_code("sw", 3, var_offset[curr_proc][var_id], 29);
        } else if (node->children[0]->symbol == "PUTCHAR") {
            generate_expr(node->children[2], curr_proc);
            generate_code("lis", 5, 0, 0);
            generate_code(".word", -1, -1, -1, "0xffff000c");
            generate_code("sw", 3, 0, 5);
        } else if (node->children[0]->symbol == "PRINTLN") {
            push_to_stk(1);
            generate_expr(node->children[2], curr_proc);
            generate_code("add", 1, 3, 0);
            push_to_stk(31);

            generate_code("lis", 5, 0, 0);
            generate_code(".word", -1, -1, -1, "print");

            generate_code("jalr", 5, 0, 0);
            pop_from_stk(31);
            pop_from_stk(1);
        } else if (node->children[0]->symbol == "IF") {
            int curr_if_ctr = if_ctr;
            if_ctr ++;
            generate_test(node->children[2], curr_proc, "if" + std::to_string(curr_if_ctr), "else" + std::to_string(curr_if_ctr));
            generate_code("label", 0, 0, 0, "if" + std::to_string(curr_if_ctr));

            generate_stmt(node->children[5], curr_proc);

            generate_code("beq", 0, 0, 0, "endif" + std::to_string(curr_if_ctr));
            generate_code("label", 0, 0, 0, "else" + std::to_string(curr_if_ctr));

            generate_stmt(node->children[9], curr_proc);

            generate_code("label", 0, 0, 0, "endif" + std::to_string(curr_if_ctr));

        } else if (node->children[0]->symbol == "WHILE") {
            int curr_while_ctr = while_ctr;
            while_ctr ++;
            generate_test(node->children[2], curr_proc, "while" + std::to_string(curr_while_ctr), "endwhile" + std::to_string(curr_while_ctr));
            generate_code("label", 0, 0, 0, "while" + std::to_string(curr_while_ctr));

            generate_stmt(node->children[5], curr_proc);

            generate_test(node->children[2], curr_proc, "while" + std::to_string(curr_while_ctr), "endwhile" + std::to_string(curr_while_ctr));
            generate_code("label", 0, 0, 0, "endwhile" + std::to_string(curr_while_ctr));
        }
    }
}



void epilogue(Node* node, string curr_proc) {
    
    for (int i = 0; i < proc_arg_num[curr_proc]; i++) {
        generate_code("add", 30, 30, 4);
    }
    for (int i = 0; i < proc_var_num[curr_proc]; i++) {
        generate_code("add", 30, 30, 4);
    }

    generate_code("jr", 31);
}


void generate(Node* root_node) {

    
    root_node = root_node->children[1]->children[0];
    /*
    for (auto child: root_node->children) {
        traverse_tree(child);
    }
    */

    special_code(".import print");

    init_main_var(root_node);

    init_dcls(root_node->children[8], "wain", 0);

    generate_stmt(root_node->children[9], "wain");

    generate_expr(root_node->children[11], "wain"); // return statement

    epilogue(root_node, "wain");

}

int main(int argc, char *argv[]) {

    Node* tree = build_tree();

    generate(tree);

    clear_memory(tree);
    delete tree;
    return 0;
}
