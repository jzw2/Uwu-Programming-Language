```
<comment> ::= "(OwO)".*"(T_T)"

<identifier> ::= [a-Z]*

<intger> ::= [0-9]*

<delimiter> ::= ("~") 

<float> ::= <integer> "." <integer>

<function-declaration> ::= <identifier> <identifier>* "no jutsu" <delimiter> (<statement>)* "!!"

<function-call> ::= <identifier> ":" <expr>+ "no jutsu"

<main-function> ::= "senpai no jutsu" (<delimiter>|"\n") (<statement>)* ~!

<expr> ::= "yosh" | "iee"
		| "namae" //used for getting the id of the thread
		| "(" <expr> ")"
		| <identifier>
		| <function-call>
		| <expr> <binary-operator> <expr> 
		| <float>
		| <integer>

//sayonara is our return expression
<return-expr> ::= "sayonara" (<expr> "chan")?

<selection-statement> ::= "nani" <expr> (<delimiter>|"\n") (<statement>)* "~?"
		| "nani" <expr> (<delimiter>|"\n") (<statement>)* "baka" (<delimiter>|"\n") (<statement>)* "~?"
		| "nani" <expr> (<delimiter>|"\n") (<statement>)* "baka" <selection-statement>

//hopefully can make this better
<while-statements> ::= "doki" <expr> (<delimiter>|"\n") (<statement>)* "~~"

<lambad-thread> ::= shadow clone jutsu <integer> <delimiter> (<statement>)* "!!"

<statement> ::= <while-statement> 
		| <selection-statement> 
		| <expr> <delimiter>
		| <return-expr> <delimiter>
		| <variable-declaration>
		| <lambda-thread>

<variable-declaration> ::= <identifier> "wa" <expr> "desu" <delimiter>

//must expand this!
<binary-operator> ::= "desu ga" 
		| "to" 
		| "||" 
		| "+" 
		| "-" 
		| "/" 
		| "*" 
		| "%"
		| ">"
		| "<"
		| "<="
		| ">="
		| ">>"
		| "<<"

```
