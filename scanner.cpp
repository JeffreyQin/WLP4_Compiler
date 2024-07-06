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

const char* dfa_string = R"delimiter(
.ALPHABET
a-z A-Z 0-9
< > = 
+ - * / % & !
( ) [ ] { } ; ,
.STATES
start
PLUS! MINUS! STAR! SLASH! PCT! AMP!
LPAREN! RPAREN! LBRACK! RBRACK! SEMI! LBRACE! RBRACE! COMMA!
p! pu! put! putc! putch! putcha! PUTCHAR!
pr! pri! prin! print! printl! PRINTLN!
w! wh! whi! whil! WHILE!
wa! wai! WAIN!
returnr! returnre! returnret! returnretu! returnretur! RETURN!
i! in! INT! IF!
elsee! elseel! elseels! ELSE!
nulln! nullnu! nullnul! NULL!
newn! newne! NEW!  
deleted! deletede! deletedel! deletedele! deletedelet! DELETE!
LT! LE!
BECOMES! EQ!
GT! GE!
EXC NE
id!
num!
comment!
.TRANSITIONS
start 0-9 num
num 0-9 num
num a-z A-Z id
start + PLUS
start - MINUS
start * STAR
start / SLASH
start % PCT
start & AMP
start ( LPAREN
start ) RPAREN
start [ LBRACK
start ] RBRACK 
start { LBRACE 
start } RBRACE 
start ; SEMI
start , COMMA
start < LT
start = BECOMES
start > GT
start ! EXC
SLASH / comment
EXC = NE
LT = LE
BECOMES = EQ
GT = GE
start a-c f-h j-m o q s-v x-z A-M O-Z id
start p p
p r pr
p u pu
p a-q s-t v-z A-Z 0-9 id
pr i pri
pr a-h j-z A-Z 0-9 id
pri n prin
pri a-m o-z A-Z 0-9 id
prin t print
prin a-s u-z A-Z 0-9 id
print l printl
print a-k m-z A-Z 0-9 id
printl n PRINTLN
printl a-m o-z A-Z 0-9 id
PRINTLN a-z A-Z 0-9 id
pu t put
pu a-s u-z A-Z 0-9 id
put c putc
put a-b d-z A-Z 0-9 id
putc h putch
putc a-g i-z A-Z 0-9 id
putch a putcha
putch b-z A-Z 0-9 id
putcha r PUTCHAR
putcha a-q s-z A-Z 0-9 id
PUTCHAR a-z A-Z 0-9 id
start w w
w h wh
w a wa
w b-g i-z A-Z 0-9 id
wh i whi
wh a-h j-z A-Z 0-9 id
whi l whil
whi a-k m-z A-Z 0-9 id
whil e WHILE
whil a-d f-z A-Z 0-9 id
WHILE a-z A-Z 0-9 id
wa i wai
wa a-h j-z A-Z 0-9 id
wai n WAIN
wai a-m o-z A-Z 0-9 id
WAIN a-z A-Z 0-9 id
start i i 
i f IF
i n in
i a-e g-m o-z A-Z 0-9 id
IF a-z A-Z 0-9 id
in t INT
in a-s u-z A-Z 0-9 id
INT a-z A-Z 0-9 id
start e elsee
elsee l elseel
elsee a-k n-z A-Z 0-9 id
elseel s elseels
elseel a-r t-z A-Z 0-9 id
elseels e ELSE
elseels a-d f-z A-Z 0-9 id
ELSE a-z A-Z 0-9 id
start n newn
newn e newne
newn a-d f-z A-Z 0-9 id
newne w NEW
newne a-v x-z A-Z 0-9 id
NEW a-z A-Z 0-9 id
start d deleted
deleted e deletede
deleted a-d f-z A-Z 0-9 id
deletede l deletedel
deletede a-k m-z A-Z 0-9 id
deletedel e deletedele
deletedel a-d f-z A-Z 0-9 id
deletedele t deletedelet
deletedele a-s u-z A-Z 0-9 id
deletedelet e DELETE
deletedelet a-d f-z A-Z 0-9 id
DELETE a-z A-Z 0-9 id
start r returnr
returnr e returnre
returnr a-d f-z A-Z 0-9 id
returnre t returnret
returnre a-s u-z A-Z 0-9 id
returnret u returnretu
returnret a-t v-z A-Z 0-9 id
returnretu r returnretur
returnretu a-q s-z A-Z 0-9 id
returnretur n RETURN
returnretur a-m o-z A-Z 0-9 id
RETURN a-z A-Z 0-9 id
start N nulln
nulln U nullnu
nulln a-z A-T V-Z 0-9 id
nullnu L nullnul
nullnu a-z A-K M-Z 0-9 id
nullnul L NULL
nullnul a-z A-K M-Z 0-9 id
NULL a-z A-Z 0-9 id
id a-z A-Z 0-9 id
.INPUT
)delimiter";

std::string s;
istringstream in(dfa_string);
std::istream& file_in = std::cin;

vector<char> language;
string initial_state;
map<string, bool> states;
map<string, map<char, string>> transitions;

  map<string, bool> direct_states = {
    {"PLUS", true},
    {"MINUS", true},
    {"STAR", true},
    {"SLASH", true},
    {"PCT", true},
    {"LPAREN", true},
    {"RPAREN", true},
    {"LBRACK", true},
    {"RBRACK", true},
    {"SEMI", true},
    {"LBRACE", true},
    {"RBRACE", true},
    {"WHILE", true},
    {"WAIN", true},
    {"RETURN", true},
    {"IF", true},
    {"INT", true},
    {"ELSE", true},
    {"NULL", true},
    {"NEW", true},
    {"DELETE", true},
    {"LT", true},
    {"LE", true},
    {"BECOMES", true},
    {"EQ", true},
    {"GT", true},
    {"GE", true},
    {"COMMA", true},
    {"PUTCHAR", true},
    {"PRINTLN", true},
    {"AMP", true},
    {"NE", true}
  };

  map<string, bool> invalid_states {
    {"EXC", true}
  };

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

int handleNum(string num_str) {
  if (num_str.length() > 10) {
    cerr << "ERROR: integer out of range" << endl;
    return 1;
  } else if (num_str.length () == 10) {
    if (stoi(num_str.substr(0,9)) > 214748364) {
      cerr << "ERROR: integer out of range" << endl;
      return 1;
    } else if (stoi(num_str.substr(0,9)) == 214748364 && num_str[9] > '7') {
      cerr << "ERROR: integer out of range" << endl;
      return 1;
    }
  }

  cout << "NUM " << num_str << endl;
  return 0;
}

int printToken(string token, string state) {

  if (direct_states[state]) {
    cout << state << " " << token << endl;
  } else if (states[state]) {

    if (state == "num") {
      if (handleNum(token) == 1) {
        return 1;
      }
    } else {
      cout << "ID " << token << endl;
    }
  } else if (invalid_states[state]) {
    cerr << "ERROR: invalid token" << endl;
    return 1;
  }
  return 0;
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-SLASH comments:
//// (Four-SLASH comment)
int main() {
  std::getline(in, s);
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        language.emplace_back(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
            language.emplace_back(c);
        }
      } 
    }
  }

  std::getline(in, s);
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
      if (initial) {
        initial_state = s;
        initial = false;
      }
      states[s] = accepting;
    }
  }

  std::getline(in, s);
  while(std::getline(in, s)) {
    if (s == INPUT) { 
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
      }
    }
  }

  while (getline(file_in, s)) {
    if (s == "") {
      continue;
    }
    string current_state = initial_state;
    string token = "";
    bool decided = false;

    int ptr = 0;

  
    while (ptr < s.length() && s[ptr] == ' ') {
      ptr += 1;
    }

    if (ptr == s.length()) {
      continue;
    }
    while (ptr < s.length()) {
        char c = s[ptr];

        if (current_state == "comment") {
          break;
        } else if (transitions[current_state].find(c) != transitions[current_state].end()) {
            token += c;
            ptr += 1;
            current_state = transitions[current_state][c];
        } else {
            if (c == ' ' || states[current_state]) {
                
                if (printToken(token, current_state) == 1) {
                  return 1;
                }

                current_state = initial_state;
                token = "";
                if (c == ' ') {
                  if (ptr == s.length() - 1) {
                    decided = true;
                  }
                  ptr += 1;
                }
            } else if (find(language.begin(), language.end(), c) == language.end()) {
                cerr << "ERROR: " << c << " is not in the language" << endl;
                decided = true;
                return 1;
            } else {
                cerr << "ERROR: invalid token" << endl;
                decided = true;
                return 1;
            }
        }
    }

    if (!decided) {
      if (current_state != "comment") {
        if (states[current_state]) {

            if (printToken(token, current_state) == 1) {
              return 1;
            }
        } else {
            cerr << "ERROR: invalid token" << endl;
            return 1;
        }
      }
    }
  }
  return 0;
}


