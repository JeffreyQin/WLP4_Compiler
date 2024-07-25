#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;


/** Convert a string representation of a number to an unsigned integer.
 *
 * If the string is "0", then 0 is returned.  If the string starts with "0x", the string is
 * interpreted as an unsigned hexidecimal number.  If the string starts with a "0", the string is
 * interpreted as an unsigned octal number.  Otherwise, the string is interpreted as a signed
 * decimal number.
 *
 * The function name is read as "string to uint64".
 *
 * @param s The string to parse
 * @return The uint64_t representation of the string
 */
uint32_t stouint64(const std::string & s)
{
    if(s == "0")
    {
        return 0;
    }
    if(s.starts_with("0x"))
    {
        return std::stol(s.substr(2), nullptr, 16);
    }

    if(s.starts_with("0"))
    {
        return std::stol(s.substr(1), nullptr, 8);
    }

    return std::stod(s);
}

enum TokenType {
    // Not a real token type we output: a unique value for initializing a TokenType when the
    // actual value is unknown
    NONE,

    DIRECTIVE,
    LABEL,
    ID,
    HEXINT,
    REG,
    DEC,
    COMMA,
    LPAREN,
    RPAREN,
};


#define TOKEN_TYPE_PRINTER(t) case t: return #t
const char * tokenTypeString(TokenType t)
{
    switch(t)
    {
        TOKEN_TYPE_PRINTER(NONE);
        TOKEN_TYPE_PRINTER(LABEL);
        TOKEN_TYPE_PRINTER(DIRECTIVE);
        TOKEN_TYPE_PRINTER(ID);
        TOKEN_TYPE_PRINTER(HEXINT);
        TOKEN_TYPE_PRINTER(REG);
        TOKEN_TYPE_PRINTER(DEC);
        TOKEN_TYPE_PRINTER(COMMA);
        TOKEN_TYPE_PRINTER(LPAREN);
        TOKEN_TYPE_PRINTER(RPAREN);
    }

    // We will never get here
    return "";
}
#undef TOKEN_TYPE_PRINTER

#define TOKEN_TYPE_READER(s, t) if(s == #t) return t
TokenType stringToTokenType(const std::string & s)
{
    TOKEN_TYPE_READER(s, NONE);
    TOKEN_TYPE_READER(s, LABEL);
    TOKEN_TYPE_READER(s, DIRECTIVE);
    TOKEN_TYPE_READER(s, ID);
    TOKEN_TYPE_READER(s, HEXINT);
    TOKEN_TYPE_READER(s, REG);
    TOKEN_TYPE_READER(s, DEC);
    TOKEN_TYPE_READER(s, COMMA);
    TOKEN_TYPE_READER(s, LPAREN);
    TOKEN_TYPE_READER(s, RPAREN);
    return NONE;
}
#undef TOKEN_TYPE_READER

class Token
{
public:
    const TokenType type;
    const std::string lexeme;

    Token(TokenType type, std::string lexeme);

    string getType();
    string getLexeme();
};

Token::Token(TokenType t, std::string l)
  : type(t), lexeme(l)
{
    // Nothing
}

string Token::getType() {
    return tokenTypeString(type);
}

string Token::getLexeme() {
    return lexeme;
}

std::ostream & operator<<(std::ostream & out, const Token token)
{
    out << tokenTypeString(token.type) << " " << token.lexeme;
    return out;
}

/** Prints an error to stderr with an "ERROR: " prefix, and newline suffix.
 *
 * @param message The error to print
 */
void formatError(const std::string & message)
{
    std::cerr << "ERROR: " << message << std::endl;
}


void printLabelMap(vector<string>& labels, unordered_map<string, uint32_t> & labelMap) {
    for (const auto& label: labels) {
        cerr << label << " " << labelMap[label] << endl;
    }
}

// extract number from register ($xx)
string registerNum(string reg) {
    uint32_t reg_num = stouint64(reg.substr(1));
    return to_string(reg_num);
}

int getUnsigned(int signed_num) {
    if (signed_num >= 0) {
        return signed_num;
    } else {
        return static_cast<uint16_t>(signed_num);
    }
}

string processCommand(vector<vector<Token>> & commands, unordered_map<std::string, uint32_t> labelMap, unordered_map<std::string, bool> hasLabel) {

    for (int idx = 0; idx < commands.size(); idx++) {

        vector<Token> command = commands[idx];

        Token firstToken = command[0];

        string processed_command = "";

        if (firstToken.getType() == "ID") {

            processed_command += firstToken.getLexeme() + " ";

            if (firstToken.getLexeme() == "add") {
                
                if (command.size() != 6
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                    || command[5].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());


            } else if (firstToken.getLexeme() == "sub") {

                if (command.size() != 6
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                    || command[5].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());

            } else if (firstToken.getLexeme() == "mult") {

                if (command.size() != 4
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " 0";

            } else if (firstToken.getLexeme() == "multu") {

                if (command.size() != 4
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " 0";

            } else if (firstToken.getLexeme() == "div") {
                
                if (command.size() != 4
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " 0";


            } else if (firstToken.getLexeme() == "divu") {
                
                if (command.size() != 4
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " 0";

            } else if (firstToken.getLexeme() == "mfhi") {

                if (command.size() != 2 || command[1].getType() != "REG") {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " 0 0";

            } else if (firstToken.getLexeme() == "mflo") {
                
                if (command.size() != 2 || command[1].getType() != "REG") {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " 0 0";

            } else if (firstToken.getLexeme() == "lis") {

                if (command.size() != 2 || command[1].getType() != "REG") {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " 0 0";

            } else if (firstToken.getLexeme() == "slt") {

                if (command.size() != 6
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                    || command[5].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());

            } else if (firstToken.getLexeme() == "sltu") {

                if (command.size() != 6
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                    || command[5].getType() != "REG"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());

            } else if (firstToken.getLexeme() == "jr") {

                
                if (command.size() != 2 || command[1].getType() != "REG") {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " 0 0";

            } else if (firstToken.getLexeme() == "jalr") {

                
                if (command.size() != 2 || command[1].getType() != "REG") {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " 0 0";

            } else if (firstToken.getLexeme() == "beq") {

                if (command.size() != 6 
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                ) {
                    return "";
                }
                
                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";


                if (command[5].getType() == "DEC" || command[5].getType() == "HEXINT") {
    
                    processed_command += to_string(getUnsigned(stouint64(command[5].getLexeme())));

                } else if (command[5].getType() == "ID") {
                    
                    if (!hasLabel[command[5].getLexeme()]) {
                        // label not found;
                        return "";
                    }

                    int current_address = 4 * idx + 4;
                    int label_address = labelMap[command[5].getLexeme()];

                    int address_diff = (label_address - current_address) / 4;


                    processed_command += to_string(getUnsigned(address_diff));
                }

            } else if (firstToken.getLexeme() == "bne") {

                if (command.size() != 6 
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[3].getType() != "REG"
                    || command[4].getType() != "COMMA"
                ) {
                    return "";
                }
                
                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";
                processed_command += registerNum(command[3].getLexeme());
                processed_command += " ";

                if (command[5].getType() == "DEC" || command[5].getType() == "HEXINT") {
    
                    processed_command += to_string(getUnsigned(stouint64(command[5].getLexeme())));

                } else if (command[5].getType() == "ID") {

                    if (!hasLabel[command[5].getLexeme()]) {
                        // label not found;
                        return "";
                    }

                    int current_address = 4 * idx + 4;
                    int label_address = labelMap[command[5].getLexeme()];
                    int address_diff = (label_address - current_address) / 4;

                    processed_command += to_string(getUnsigned(address_diff));
                }

            } else if (firstToken.getLexeme() == "lw") {
                
                if (command.size() != 7
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[4].getType() != "LPAREN"
                    || command[5].getType() != "REG"
                    || command[6].getType() != "RPAREN"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";

                if (command[3].getType() == "DEC" || command[3].getType() == "HEXINT") {
                    processed_command += to_string(getUnsigned(stouint64(command[3].getLexeme())));
                } else {
                    return "";
                }

                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());

            } else if (firstToken.getLexeme() == "sw") {
                
                if (command.size() != 7
                    || command[1].getType() != "REG"
                    || command[2].getType() != "COMMA"
                    || command[4].getType() != "LPAREN"
                    || command[5].getType() != "REG"
                    || command[6].getType() != "RPAREN"
                ) {
                    return "";
                }

                processed_command += registerNum(command[1].getLexeme());
                processed_command += " ";

                if (command[3].getType() == "DEC" || command[3].getType() == "HEXINT") {
                    processed_command += to_string(getUnsigned(stouint64(command[3].getLexeme())));
                } else {
                    return "";
                }
                
                processed_command += " ";
                processed_command += registerNum(command[5].getLexeme());
            }

        } else if (firstToken.getType() == "DIRECTIVE") {

            processed_command += firstToken.getLexeme().substr(1);
            processed_command += " ";

            if (command.size() != 2) {
                return "";
            }
            if (command[1].getType() == "DEC" || command[1].getType() == "HEXINT") {
                processed_command += to_string(getUnsigned(stouint64(command[1].getLexeme())));
            } else if (command[1].getType() == "ID") {
                if (!hasLabel[command[1].getLexeme()]) {
                    // label not found;
                    return "";
                }
                processed_command += to_string(labelMap[command[1].getLexeme()]);
            } else {
                return "";
            }
            processed_command += " 0 0";

        } else {
            // invalid first mips term
            return "";
        }

        cout << processed_command << endl;
    }
    return "complete";
}



/** For a given instruction, prints the instruction in the ascii representation of a call to
 *  compileLine().
 *
 * @param instruction The name of the instruction
 * @param one The value of the first parameter
 * @param two The value of the second parameter, 0 if the instruction has < 2 parameter
 * @param three The value of the third parameter, 0 if the instruction has < 3 parameter
 */
bool compileLine(const std::string & instruction,
                 uint32_t one,
                 uint32_t two,
                 uint32_t three)
{
    cout << instruction << one << two << three << endl;
    uint32_t values[3] = {one, two, three};

    std::cout << "compileLine(\"" << instruction << "\", ";
    for(size_t i = 0; i < 3; i += 1)
    {
        std::cout << (values[i] != 0 ? "0x" : "")
                  << std::hex << values[i] << (i < 2 ? ", " : "");
    }
    std::cout << ");" << std::endl;

    return true;
}


/** Entrypoint for the assembler.  The first parameter (optional) is a mips assembly file to
 *  read.  If no parameter is specified, read assembly from stdin.  Prints machine code to stdout.
 *  If invalid assembly is found, prints an error to stderr, stops reading assembly, and return a
 *  non-0 value.
 *
 * If the file is not found, print an error and returns a non-0 value.
 *
 * @return 0 on success, non-0 on error
 */
int main(int argc, char * argv[])
{
    if(argc > 2)
    {
        std::cerr << "Usage:" << std::endl
                  << "\tasm [$FILE]" << std::endl
                  << std::endl
                  << "If $FILE is unspecified or if $FILE is `-`, read the assembly from standard "
                  << "in. Otherwise, read the assembly from $FILE." << std::endl;
        return 1;
    }

    std::ifstream fp;
    std::istream &in =
        (argc > 1 && std::string(argv[1]) != "-")
      ? [&]() -> std::istream& {
            fp.open(argv[1]);
            return fp;
        }()
      : std::cin;

    if(!fp && argc > 1)
    {
        formatError((std::stringstream() << "file '" << argv[1] << "' not found!").str());
        return 1;
    }

    std::vector<Token> tokens;
    std::vector<vector<Token>> commands;
    std:: vector<string> labels;
    std::unordered_map<std::string, uint32_t> labelMap;
    std::unordered_map<std::string, bool> hasLabel;
    
    int current_address = 0;

    while (!in.eof()) {
        std::string line;
        std::getline( in, line );

        // if token line is empty
        if(line == "")
        {
            continue;
        }

        std::string tokenType;
        std::string lexeme;

        std::stringstream lineParser(line);
        lineParser >> tokenType;

        if(tokenType != "NEWLINE")
        {
            lineParser >> lexeme;
        }
        

        // MERGE TOKENS FROM THE SAME INSTRUCTION
        // CREATE LABEL TABLE

        Token newest_token {stringToTokenType(tokenType), lexeme};

        if (commands.size() == 0 && tokenType == "LABEL") {
            string label = lexeme.substr(0,lexeme.length() - 1);
            labels.emplace_back(label);
            labelMap[label] = 0;
            hasLabel[label] = true;

            vector<Token> newest_command {};
            commands.emplace_back(newest_command);
        } else {
            if (tokenType == "NEWLINE") {
                if (commands.size() == 0 || commands.back().size() != 0) {
                    vector<Token> newest_command {};
                    commands.emplace_back(newest_command);
                }
            } else if (tokenType == "LABEL") {
                string label = lexeme.substr(0,lexeme.length() - 1);
                if (hasLabel[label]) {
                    formatError("duplicate label definition detected");
                    return 1;
                } else {
                    labels.emplace_back(label);
                    labelMap[label] = current_address;
                    hasLabel[label] = true;
                }
                if (commands.back().size() != 0) {
                    vector<Token> newest_command {};
                    commands.emplace_back(newest_command);
                }
            } else {
                if (commands.size() == 0) {
                    vector<Token> newest_command {};
                    commands.emplace_back(newest_command);
                    current_address += 4;
                } else if (commands.back().size() == 0) {
                    current_address += 4;
                }
                commands.back().emplace_back(newest_token);
            }
        }

    }

    // remove last empty command
    if (commands.back().size() == 0) {
        commands.pop_back();
    }

    // print all labels to stderr
    printLabelMap(labels, labelMap);

    // print command one by one
    string result = processCommand(commands, labelMap, hasLabel);

    if (result == "") {
        formatError("invalid mips command detected");
        return 1;
    }

    return 0;
}
