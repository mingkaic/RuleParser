#include "include/adhocs.hpp"
#include <ctime>

int main(int argc, char* argv[]) {
	srand(time(NULL));
	std::string rulefile = "metarules.txt";
	std::string codefile = "rules.txt";
	if (argc > 1) {
		rulefile = argv[1];
	}
	if (argc > 2) {
		codefile = argv[2];
	}
	adhoc::adhoc_parser parser;

	std::ifstream rules(rulefile);
	if (false == rules.good()) {
		throw std::runtime_error("input code file not found");
	}
	parser.parse(rules);
	
	adhoc::program* top = dynamic_cast<adhoc::program*>(parser.top());
	top->rule_gen();

	std::ifstream code(codefile);
	if (false == code.good()) {
		throw std::runtime_error("input code file not found");
	}
	
	// top->parser->parse(code);

	delete top->parse;
	delete top;
	return 0;
}
