# Compiler-FrontEnd
The front end of a simple compiler
## Lexical Analyzer
- Input
    ```C++
    int main()
    {
    printf("HelloWorld");
    return 0;
    }
    ```   
- Output
    ```
    1: <int,17>
    2: <main,81>
    3: <(,44>
    4: <),45>
    5: <{,59>
    6: <printf,81>
    7: <(,44>
    8: <",78>
    9: <HelloWorld,81>
    10: <",78>
    11: <),45>
    12: <;,53>
    13: <return,20>
    14: <0,80>
    15: <;,53>
    16: <},63>
    ```
## LL Parser
- input
    ```C++
    {
    ID = NUM ;
    }
    ```
- output
    ```
    program
        compoundstmt
            {
            stmts
                stmt
                    assgstmt
                        ID
                        =
                        arithexpr
                            multexpr
                                simpleexpr
                                    NUM
                                multexprprime
                                    E
                            arithexprprime
                                E
                        ;
                stmts
                    E
            }
    ```
## LR Parser
- input
    ```C++
    {
    ID = NUM ;
    }
    ```
- output
    ```
    program =>
    compoundstmt =>
    { stmts } =>
    { stmt stmts } =>
    { stmt } =>
    { assgstmt } =>
    { ID = arithexpr ; } =>
    { ID = multexpr arithexprprime ; } =>
    { ID = multexpr ; } =>
    { ID = simpleexpr multexprprime ; } =>
    { ID = simpleexpr ; } =>
    { ID = NUM ; }
    ```
## Translation Scheme
- input
    ```C++
    int a = 1 ; int b = 2 ; real c = 3.0 ;
    {
    a = a + 1 ;
    b = b * a ;
    if ( a < b ) then c = c / 2 ; else c = c / 4 ;
    }
    ```
- output
    ```C++
    a: 2
    b: 4
    c: 1.5
    ```
## Note
- implement with C++
