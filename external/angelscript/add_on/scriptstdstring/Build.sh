#!/usr/bin/env bash
g++ -c scriptstdstring.cpp -I../../angelscript/include -o scriptstdstring.o
g++ -c scriptstdstring_utils.cpp -I../../angelscript/include -o scriptstdstring_utils.o
ar rvs libscriptstdstring.a scriptstdstring.o scriptstdstring_utils.o
