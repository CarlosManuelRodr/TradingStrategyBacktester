#!/usr/bin/env bash
(cd Libraries/AngelScript/angelscript/projects/gnuc && make clean)
(cd Libraries/AngelScript/add_on/scriptarray && ./Clear.sh)
(cd Libraries/AngelScript/add_on/scriptbuilder && ./Clear.sh)
(cd Libraries/AngelScript/add_on/scriptstdstring && ./Clear.sh)
(cd Build && make clean)
rm -r Build