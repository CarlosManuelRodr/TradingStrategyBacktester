#!/usr/bin/env bash
(cd Libraries/AngelScript/angelscript/projects/gnuc && make)
(cd Libraries/AngelScript/add_on/scriptarray && ./Build.sh)
(cd Libraries/AngelScript/add_on/scriptbuilder && ./Build.sh)
(cd Libraries/AngelScript/add_on/scriptstdstring && ./Build.sh)
mkdir Build
(cd Build && cmake ../Backtester && make)