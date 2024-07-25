#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

using namespace std;

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {

  const char* dfa_string = R"delimiter(
.ALPHABET
a-z A-Z 0-9
. : $ - , ( )
.STATES
start
zero! hexint hexint2!
comma! lparen! rparen!
reg d1! d2!
period dir!
id! label!
dec1 dec2!
.TRANSITIONS
start . period
period a-z A-Z dir
dir a-z A-Z dir
start a-z A-Z id
id 0-9 a-z A-Z id
id : label
start 0 zero 
zero x hexint
hexint 0-9 a-f A-F hexint2
hexint2 0-9 a-f A-F hexint2
start $ reg
reg 0-9 d1
d1 0-9 d2
start - dec1
start 1-9 dec2
dec1 1-9 dec2
dec2 0-9 dec2
start , comma
start ( lparen
start ) rparen
.INPUT
)delimiter";

  // std::istream& in = std::cin;
  std::string s;

  istringstream in(dfa_string);
  std::istream& file_in = std::cin;

  vector<char> language;
  string initial_state;
  map<string, bool> states;
  map<string, map<char, string>> transitions;
  

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        language.emplace_back(s[0]);
        //// Variable 's[0]' is an alphabet symbol
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
            language.emplace_back(c);
          //// Variable 'c' is an alphabet symbol
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      if (initial) {
        //// The state is initial
        initial_state = s;
        initial = false;
      }
      states[s] = accepting;
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      std::vector<std::string> lineVec;
      while(line >> s) {
        lineVec.push_back(s);
      }
      fromState = lineVec.front();
      toState = lineVec.back();
      for(int i = 1; i < lineVec.size()-1; ++i) {
        std::string s = lineVec[i];
        if (isChar(s)) {
          symbols += s;
        } else if (isRange(s)) {
          for(char c = s[0]; c <= s[2]; ++c) {
            symbols += c;
          }
        }
      }

      for ( char c : symbols ) {
        transitions[fromState][c] = toState;
        //// There is a transition from 'fromState' to 'toState' on 'c'
      }
    }
  }
  
  while (getline(file_in, s)) {
    string current_state = initial_state;
    string token = "";
    bool decided = false;

    int ptr = 0;
    while (ptr < s.length()) {
        char c = s[ptr];


        if (transitions[current_state].find(c) != transitions[current_state].end()) {
            token += c;
            ptr += 1;
            current_state = transitions[current_state][c];
        } else {
            if (states[current_state]) {
                if (current_state == "dir") {
                  cout << "DIRECTIVE " << token << endl;
                } else if (current_state == "label") {
                  cout << "LABEL " << token << endl;
                } else if (current_state == "id") {
                  cout << "ID " << token << endl;
                } else if (current_state == "hexint2") {
                  cout << "HEXINT " << token << endl;
                } else if (current_state == "d1" || current_state == "d2") {
                  cout << "REG " << token << endl;
                } else if (current_state == "dec2" || current_state == "zero") {
                  cout << "DEC " << token << endl;
                } else if (current_state == "comma") {
                  cout << "COMMA " << token << endl;
                } else if (current_state == "lparen") {
                  cout << "LPAREN " << token << endl;
                } else if (current_state == "rparen") {
                  cout << "RPAREN " << token << endl;
                }
                current_state = initial_state;
                token = "";
                if (c == ' ') {
                  ptr += 1;
                }
            } else if (find(language.begin(), language.end(), c) == language.end()) {
                cerr << "ERROR: " << c << " is not in the language" << endl;
                decided = true;
                break;
            } else {
                cerr << "ERROR: invalid token" << endl;
                decided = true;
                break;
            }
        }
    }
    if (!decided) {
      if (states[current_state]) {
          if (current_state == "dir") {
              cout << "DIRECTIVE " << token << endl;
          } else if (current_state == "label") {
              cout << "LABEL " << token << endl;
          } else if (current_state == "id") {
              cout << "ID " << token << endl;
          } else if (current_state == "hexint2") {
              cout << "HEXINT " << token << endl;
          } else if (current_state == "d1" || current_state == "d2") {
              cout << "REG " << token << endl;
          } else if (current_state == "dec2" || current_state == "zero") {
              cout << "DEC " << token << endl;
          } else if (current_state == "comma") {
              cout << "COMMA " << token << endl;
          } else if (current_state == "lparen") {
              cout << "LPAREN " << token << endl;
          } else if (current_state == "rparen") {
              cout << "RPAREN " << token << endl;
          }
      } else {
          cerr << "ERROR: invalid token" << endl;
      }
    }
  }
}


