```
<comment> ::= "(OwO)".*"(T_T)"

<identifier> ::= [a-Z]*

<intger> ::= [0-9]*

<float> ::= <integer> "." <integer>

<function-declaration> ::= <identifier> <identifier>* "no jutsu" \n (<statement>)* \n

<function-call> ::= <identifier> <expr>* "no jutsu"

<main-function> ::= "senpai no jutsu" \n (<statement> \n | <return-expr> \n)* \n

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

<selection-statement> ::= "nani" <expr> \n (<statement> \n)* \n
		| "nani" <expr> \n (<statement> \n)* "baka" \n (<statement> \n)* \n
		| "nani" <expr> \n (<statement> \n)* "baka" <selection-statement>

//hopefully can make this better
<while-statements> ::= "doki" <expr> \n (<statement> \n)* \n

<lambad-thread> ::= shadow clone jutsu <integer> \n (<statement> \n)* \n

<statement> ::= <while-statement> 
		| <selection-statement> 
		| <expr> \n
		| <return-expr> \n
		| <variable-declaration>
		| <lambda-thread>

<variable-declaration> ::= <identifier> "wa" <expr> "desu" \n

//must expand this!
<binary-operator> ::= "desu ga" 
		| "to" 
		| "||" 
		| "+" 
		| "-" 
		| "/" 
		| "*" 
		| "%"

```
