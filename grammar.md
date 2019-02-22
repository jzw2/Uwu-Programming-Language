
```
<function> ::= <identifier> jutsu \n  <statement>* modori <expr>?

<selection-statement> ::= if <boolean-expr> \n <statement> * modori 
                  | if <boolean-expr> \n <stament>* else <statement> 


<while-statements> ::= shuriken <boolean-expr> \n <statement>* modori

<statement> ::= <while-statement> | <selection-statement>

<variable> ::= [a-z]*

<variable-declaration> ::= nin <variable> \n

<expr> ::= <variable> | <function-call> | <expr> <bin-operator> <expr> 

<boolean-expr> ::= yosh | false | <boolean-expr> <boolean-operator> <boolean-expr> | <boolean-function> | <expr>

<lambad-thread> ::= TODO
```
