module UWU-SYNTAX
       imports DOMAINS-SYNTAX

       syntax AExp ::= 
               Int
              | Id
              > AExp "/" AExp  [left, strict]
              | AExp "*" AExp [left, strict]
              > AExp "+" AExp [left, strict]
              | AExp "-" AExp [left, strict]

              
              

endmodule

module UWU
       imports UWU-SYNTAX
       import DOMAINS

        syntax KResult ::= Int

       rule I1 / I2 => I1 /Int I2
       rule I1 * I2 => I1 *Int I2
       rule I1 + I2 => I1 +Int I2
       rule I1 - I2 => I1 -Int I2
endmodule