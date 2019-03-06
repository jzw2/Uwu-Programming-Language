```
<identifier> ::= [a-Z]*

<intger> ::= [0-9]*

<float> ::= <integer> "." <integer>

<function-declaration> ::= <identifier> <identifier>* "no jutsu" \n (<statement> \n)* \n\n

<function-call> ::= <identifier> "no jutsu" (\n)?

//sayonara is our return expression
<expr> ::= "yosh" | "iee" 
		| <identifier> 
		| <expr> <binary-operator> <expr> 
		| "sayonara" (<expr> "chan")?
		| <float>
		| <integer>

<selection-statement> ::= "nani" <boolean-expr> \n (<statement> \n)* \n
		| "nani" <boolean-expr> \n <stament>* "baka" \n (<statement> \n)* \n

//hopefully can make this better
<while-statements> ::= "suki" <boolean-expr> \n (<statement> \n)* \n

<statement> ::= <while-statement> 
		| <selection-statement> 
		| <expr>

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

<lambad-thread> ::= shadow clone jutsu [0-9]* \n (<statement> \n)* \n
```
