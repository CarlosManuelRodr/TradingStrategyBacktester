#!/usr/bin/env bash
g++ -c scriptbuilder.cpp -I../../angelscript/include -o scriptbuilder.o
ar rvs libscriptbuilder.a scriptbuilder.o
