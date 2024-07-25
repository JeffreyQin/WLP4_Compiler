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

int special_label_id = 0;

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


int get_correct_offset(string proc, string id) {
    if (var_offset[proc][id] <= 0) {
        return var_offset[proc][id];
    } else {
        return 4 * (proc_arg_num[proc] - (var_offset[proc][id] / 4) + 1);
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



void init_params(Node* node, string curr_proc, int num_params) {

    if (node->symbol == "params") {
        if (node->children.size() == 0) {
            proc_arg_num[curr_proc] = 0;
        } else {
            init_params(node->children[0], curr_proc, num_params);
        }
    } else if (node->symbol == "paramlist") {
        if (node->children.size() == 1) {
            proc_arg_num[curr_proc] = num_params + 1;
            var_offset[curr_proc][node->children[0]->children[1]->lexeme] = 4 * num_params + 4;
        } else if (node->children.size() == 3) {
            var_offset[curr_proc][node->children[0]->children[1]->lexeme] = 4 * num_params + 4;
            init_params(node->children[2], curr_proc, num_params + 1);
        }
    }


}
void init_main_var(Node* node) {

    generate_code("sw", 1, -4, 30);
    generate_code("sub", 30, 30, 4);
    generate_code("sw", 2, -4, 30);
    generate_code("sub", 30, 30, 4);

    generate_code("sub", 29, 30, 4);

    var_offset["wain"][node->children[3]->children[1]->lexeme] = 4;
    var_offset["wain"][node->children[5]->children[1]->lexeme] = 8;

    proc_arg_num["wain"] = 2;
}



void init_dcls(Node* node, string curr_proc, int num_dcl) {
    if (node->children.size() == 0) { // dcls -> .EMPTY
        proc_var_num[curr_proc] = num_dcl;
    } else {
        // var declaration
        if (node->children[1]->children[0]->children.size() == 1) {
            if (std::stoi(node->children[3]->lexeme) == 0) {
                generate_code("sw", 0, -4, 30);
                generate_code("sub", 30, 30, 4);
            } else {
                store_const(3, std::stoi(node->children[3]->lexeme));
                generate_code("sw", 3, -4, 30);
                generate_code("sub", 30, 30, 4);
            }
        } else if (node->children[1]->children[0]->children.size() == 2) {
            if (node->children[3]->symbol == "NULL") {
                store_const(3, 1);
                generate_code("sw", 3, -4, 30);
                generate_code("sub", 30, 30, 4);
            }
        }

        var_offset[curr_proc][node->children[1]->children[1]->lexeme] = num_dcl * -4;
        init_dcls(node->children[0], curr_proc, num_dcl + 1);
    }
}


void generate_expr_for_arg(Node* node, string curr_proc) {
    if (node->symbol == "expr") {
        if (node->children.size() == 1) {
            generate_expr_for_arg(node->children[0], curr_proc);
        } else {
            generate_expr_for_arg(node->children[0], curr_proc);
            push_to_stk(3);
            generate_expr_for_arg(node->children[2], curr_proc);
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
                int offset = get_correct_offset(curr_proc, node->children[0]->lexeme);
                generate_code("lw", 3, offset, 29);
            }
        } else if (node->children.size() == 3) {
            if (node->children[0]->symbol == "GETCHAR") {
                generate_code("lis", 5, 0, 0);
                generate_code(".word", -1, -1, -1, "0xffff0004");
                generate_code("lw", 3, 0, 5);
            } else {
                generate_expr_for_arg(node->children[1], curr_proc);
            }
        } 
    } else if (node->symbol == "term") {
        if (node->children.size() == 1) {
            generate_expr_for_arg(node->children[0], curr_proc);
        } else {
            generate_expr_for_arg(node->children[0], curr_proc);
            push_to_stk(3);
            generate_expr_for_arg(node->children[2], curr_proc);
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


Node* remove_bracket(Node* node) {
    if (node->children[0]->symbol == "LPAREN") {
        return remove_bracket(node->children[1]);
    } else {
        return node;
    }
}


void generate_expr(Node* node, string curr_proc) {
    if (node->symbol == "expr") {
        if (node->children.size() == 1) {
            generate_expr(node->children[0], curr_proc);
        } else {
            if (node->children[0]->type == 2 && node->children[2]->type == 2) {
                generate_expr(node->children[0], curr_proc);
                push_to_stk(3);
                generate_expr(node->children[2], curr_proc);
                pop_from_stk(5);
                generate_code("sub", 3, 5, 3);
                generate_code("div", 3, 4);
                generate_code("mflo", 3);
            } else {
                generate_expr(node->children[0], curr_proc);

                if (node->children[2]->type == 2) {
                    generate_code("mult", 3, 4);
                    generate_code("mflo", 3);
                }
                
                push_to_stk(3);
                generate_expr(node->children[2], curr_proc);

                if (node->children[0]->type == 2) {
                    generate_code("mult", 3, 4);
                    generate_code("mflo", 3);
                }

                pop_from_stk(5);
                if (node->children[1]->symbol == "PLUS") {
                    generate_code("add", 3, 5, 3);
                } else if (node->children[1]->symbol == "MINUS") {
                    generate_code("sub", 3, 5, 3);
                }
            }
        }
    } else if (node->symbol == "factor") {
        if (node->children.size() == 1) {
            if (node->children[0]->symbol == "NUM") {
                store_const(3, std::stoi(node->children[0]->lexeme));
            } else if (node->children[0]->symbol == "ID") {
                int offset = get_correct_offset(curr_proc, node->children[0]->lexeme);
                generate_code("lw", 3, offset, 29);
            } else if (node->children[0]->symbol == "NULL") {
                store_const(3, 1);
            }
        } else if (node->children.size() == 2) {
            
            if (node->children[0]->symbol == "STAR") {
                generate_expr(node->children[1], curr_proc);
                generate_code("lw", 3, 0, 3);
            } else if (node->children[0]->symbol == "AMP") {
                // node->children[1]->symbol should be "lvalue"
                if (node->children[1]->children[0]->symbol == "STAR") {
                    generate_expr(node->children[1]->children[1], curr_proc);
                } else if (node->children[1]->children[0]->symbol == "ID") {
                    int offset = get_correct_offset(curr_proc, node->children[1]->children[0]->lexeme);
                    generate_code("lis", 3);
                    generate_code(".word", offset);
                    generate_code("add", 3, 29, 3);
                } else if (node->children[1]->children[0]->symbol == "LPAREN") {
                    Node* inside_node = remove_bracket(node->children[1]);
                    if (inside_node->children[0]->symbol == "STAR") {
                        generate_expr(inside_node->children[1], curr_proc);
                    } else if (inside_node->children[0]->symbol == "ID") {
                        int offset = get_correct_offset(curr_proc, inside_node->children[0]->lexeme);
                        generate_code("lis", 3);
                        generate_code(".word", offset);
                        generate_code("add", 3, 29, 3);
                    }
                }
            }
        } else if (node->children.size() == 3) {
            if (node->children[0]->symbol == "GETCHAR") {
                generate_code("lis", 5, 0, 0);
                generate_code(".word", -1, -1, -1, "0xffff0004");
                generate_code("lw", 3, 0, 5);


            } else if (node->children[0]->symbol == "ID") { // proc call without args
                
                string called_proc = node->children[0]->lexeme;

                push_to_stk(29);
                push_to_stk(31);
                
                generate_code("lis", 25);
                generate_code(".word", -1, -1, -1, "USER" + called_proc);
                generate_code("jalr", 25);

                pop_from_stk(31);
                pop_from_stk(29);

            } else {
                generate_expr(node->children[1], curr_proc);
            }
        } else if (node->children.size() == 4) { // proc call without argument
            
            string called_proc = node->children[0]->lexeme;

            push_to_stk(29);
            push_to_stk(31);

            // handle_proc_call_args
            generate_expr(node->children[2], curr_proc);
  
            generate_code("lis", 25);
            generate_code(".word", -1, -1, -1, "USER" + called_proc);
            generate_code("jalr", 25);

            for (int i = 0; i < proc_arg_num[called_proc]; i++) {
                generate_code("add", 30, 30, 4);
            }

            pop_from_stk(31);
            pop_from_stk(29);
        } else if (node->children.size() == 5) { // new
            generate_expr(node->children[3], curr_proc);
            push_to_stk(1);
            generate_code("add", 1, 3, 0);
            push_to_stk(31);

            generate_code("lis", 5);
            generate_code(".word", -1, -1, -1, "new");
            generate_code("jalr", 5);

            pop_from_stk(31);
            pop_from_stk(1);

            // change $3 to 1 if it is originally 0
            string special_label = "skip" + std::to_string(special_label_id);
            special_label_id += 1;
            generate_code("bne", 3, 0, 0, special_label);
            generate_code("add", 3, 0, 11);
            special_code(special_label + ":");
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
    } else if (node->symbol == "arglist") {
        if (node->children.size() == 1) {
            generate_expr(node->children[0], curr_proc);
            push_to_stk(3);
        } else if (node->children.size() == 3) {
            generate_expr(node->children[0], curr_proc);
            push_to_stk(3);
            generate_expr(node->children[2], curr_proc);
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
        if (node->children[0]->type == 2 && node->children[2]->type == 2) {
            generate_code("sltu", 3, 5, 3);
        } else {
            generate_code("slt", 3, 5, 3);
        }
        generate_code("beq", 3, 11, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "GT") {
        generate_code("beq", 5, 3, 0, false_label);
        if (node->children[0]->type == 2 && node->children[2]->type == 2) {
            generate_code("sltu", 3, 5, 3);
        } else {
            generate_code("slt", 3, 5, 3);
        }
        generate_code("beq", 3, 11, 0, false_label);
        generate_code("beq", 0, 0, 0, true_label);
    } else if (node->children[1]->symbol == "LE") {
        generate_code("beq", 5, 3, 0, true_label);
        if (node->children[0]->type == 2 && node->children[2]->type == 2) {
            generate_code("sltu", 3, 5, 3);
        } else {
            generate_code("slt", 3, 5, 3);
        }
        generate_code("beq", 3, 11, 0, true_label);
        generate_code("beq", 0, 0, 0, false_label);
    } else if (node->children[1]->symbol == "GE") {
        if (node->children[0]->type == 2 && node->children[2]->type == 2) {
            generate_code("sltu", 3, 5, 3);
        } else {
            generate_code("slt", 3, 5, 3);
        }
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

            if (node->children[0]->children[0]->symbol == "ID") {
                string var_id = node->children[0]->children[0]->lexeme;
                int offset = get_correct_offset(curr_proc, var_id);
                generate_code("sw", 3, offset, 29);
            } else if (node->children[0]->children[0]->symbol == "STAR") {
                push_to_stk(3);
                generate_expr(node->children[0]->children[1], curr_proc);
                pop_from_stk(5);
                generate_code("sw", 5, 0, 3);
            }
        } else if (node->children[0]->symbol == "DELETE") {

            string special_label = "skip" + std::to_string(special_label_id);
            special_label_id += 1;

            generate_expr(node->children[3], curr_proc);
            generate_code("beq", 3, 11, -1, special_label);

            push_to_stk(1);
            push_to_stk(31);

            generate_code("add", 1, 3, 0);

            generate_code("lis", 5);
            generate_code(".word", -1, -1, -1, "delete");
            generate_code("jalr", 5);

            pop_from_stk(31);
            pop_from_stk(1);

            special_code(special_label + ":");
            

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



int saved_reg_offset = 4;

void save_registers() {
    push_to_stk(5);
}

void restore_registers() {
    pop_from_stk(5);
}


void epilogue(string curr_proc) {

    if (curr_proc == "wain") {
        for (int i = 0; i < proc_arg_num[curr_proc]; i++) {
            generate_code("add", 30, 30, 4);
        }
    }

    for (int i = 0; i < proc_var_num[curr_proc]; i++) {
        generate_code("add", 30, 30, 4);
    }

    generate_code("jr", 31);
}

void alloc_init(Node* node) {
    if (node->children[3]->children[0]->children.size() == 2) { // wain arg 1 is pointer
        push_to_stk(2);
        generate_code("lw", 2, 4, 29);
        push_to_stk(31);

        generate_code("lis", 5);
        generate_code(".word", -1, -1, -1, "init");
        generate_code("jalr", 5);
        
        pop_from_stk(31);
        pop_from_stk(2);
    } else if (node->children[3]->children[0]->children.size() == 1) {
        push_to_stk(2);
        generate_code("add", 2, 0, 0);
        push_to_stk(31);

        generate_code("lis", 5);
        generate_code(".word", -1, -1, -1, "init");
        generate_code("jalr", 5);

        pop_from_stk(31);
        pop_from_stk(2);
    }
}

// generate procedure by procedure
void helper_generator(Node* root_node) {

    // root_node->symbol should be procedures
    if (root_node->children[0]->symbol == "procedure") {
        Node* next_proc_node = root_node->children[1];
        root_node = root_node->children[0];
        
        special_code("USER" + root_node->children[1]->lexeme + ":"); // procedure name

        init_params(root_node->children[3], root_node->children[1]->lexeme, 0);

        generate_code("sub", 29, 30, 4);

        init_dcls(root_node->children[6], root_node->children[1]->lexeme, 0);

        save_registers();

        generate_stmt(root_node->children[7], root_node->children[1]->lexeme);

        generate_expr(root_node->children[9], root_node->children[1]->lexeme); // return statement

        restore_registers();

        epilogue(root_node->children[1]->lexeme);


        //cout << root_node->children[1]->lexeme << proc_arg_num[root_node->children[1]->lexeme] << endl;
        special_code("; ___________________________________________________________________________________________________________");
        helper_generator(next_proc_node);


    } else if (root_node->children[0]->symbol == "main") {
        root_node = root_node->children[0];

        special_code("wain:");

        init_main_var(root_node);

        init_dcls(root_node->children[8], "wain", 0);

        alloc_init(root_node);

        generate_stmt(root_node->children[9], "wain");

        generate_expr(root_node->children[11], "wain"); // return statement


        epilogue("wain");
    }
}


void main_generator(Node* root_node) {

    special_code(".import print");
    special_code(".import init");
    special_code(".import new");
    special_code(".import delete");

    store_const(4, 4);
    store_const(11, 1);

    generate_code("beq", 0, 0, 0, "wain");

    special_code("; ________________________________________________________________________________");

    helper_generator(root_node->children[1]);

}

int main(int argc, char *argv[]) {

    Node* tree = build_tree();

    main_generator(tree);

    clear_memory(tree);
    delete tree;
    return 0;
}
