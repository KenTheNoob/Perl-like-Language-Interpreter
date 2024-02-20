# Perl-like-Language-Interpreter
Programming Language Concepts class project consisting of a lexical analyzer and interpreter.
Reads a text document with Perl like code as an input file and splits it into tokens using the lexical anayzer, then checks the syntax using the parser.
Either runs the code in the file or outputs the error messages.

Compile with g++ -o prog3 prog3.cpp lex.cpp parserInt.cpp
Run with ./prog3.exe test1.txt
