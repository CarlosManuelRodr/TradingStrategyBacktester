#!/usr/bin/env bash
g++ -c scriptarray.cpp -I../../angelscript/include -o scriptarray.o
ar rvs libscriptarray.a scriptarray.o
