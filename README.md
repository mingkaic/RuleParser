# RuleParser

## Adhoc Metarule Grammar:

letter = regex("[A-Za-z_]");
decimal = regex("[0-9]");
space = regex("(?: )*");

START = "start";
STRING = regex("\"(?:[^\\\"\n]|\\[abtnvfr\"])*\"");
VAR = letter {letter | decimal};
ALTER = space "|" space;
ASSIGN = space "=" space;
CONCAT = regex("(?: )+");
REGEX = "regex";
LPAREN_NULL = "(?:";
LPAREN = "(";
RPAREN = ")";
LSB = "[";
RSB = "]";
LCB = "{";
RCB = "}";
RCBPLUS = "}+";
END = ";";

regfunc = REGEX space LPAREN space STRING space RPAREN;
opt = LSB space rvalue space RSB;
rep1 = LCB space rvalue space RCBPLUS;
rep0 = LCB space rvalue space RCB;
capture = LPAREN space rvalue space RPAREN;
null_grouping = LPAREN_NULL space rvalue space RPAREN;
endpoint = STRING | regfunc | opt | rep1 | rep0 | null_grouping | capture;
alterop = mid space ALTER;
mid = endpoint | (alterop space endpoint);
rvalue = VAR | mid | (rvalue CONCAT mid);

stmt = (START | VAR) ASSIGN rvalue END;

program = {stmt}+;

start = program;

## Precedence

NullGroup has highest precedence
Alteration operation has second highest precedence
Concatenation has lowest precedence

## Adhoc Symbol Meanings

Regex represents inner string as ECMAScript regex

Capture denotes base objects associable to an attribute/meaning/rule

Rep0 denotes inner elements appearing zero or more times

Rep1 denotes inner elements appearing one or more times

Optional denotes inner elements appearing one or zero times

## TODOS:

- change grouping to capture
- add nullgrouping
- upgrade first order to denote all products representable as regex
- only flatten products/sub-products that are captured AND first-order
- deal with products captured but NOT first-order