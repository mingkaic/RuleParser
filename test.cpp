#include "static_parser.hpp"
#include <ctime>

static string names[] = {
	"VAR",
	"STRING",
	"ALTER",
	"CONCAT",
	"ASSIGN",
	"REGEX",
	"LPAREN",
	"RPAREN",
	"LSB",
	"RSB",
	"LCB",
	"RCB",
	"RCB+",
	"END",
	"LSIDE",
	"RSIDE",
	"TERM$",
	"PROGRAM",
	"STMT",
	"RVALUE",
	"MID",
	"BASE",
	"GROUP",
	"OPTION",
	"REP0",
	"REP1",
	"ALTEROP",
};

int main(int argc, char* argv[]) {
	srand(time(NULL));
	string rulefile = "metarules.txt";
	string codefile = "rules.txt";
	if (argc > 1) {
		rulefile = argv[1];
	}
	if (argc > 2) {
		codefile = argv[2];
	}
	adhoc_parser parser;
	parser.parse(rulefile);
	program* top = parser.top();
	top->rule_gen();

	ifstream dyn_in(codefile);
	if (false == dyn_in.good()) {
		throw runtime_error("input code file not found");
	}
	//top->parse->parse(dyn_in);

	delete top->parse;
	delete top;
	return 0;
}