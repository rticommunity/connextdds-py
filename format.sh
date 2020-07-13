#!/bin/bash
find connextdds/ -iname *.cpp -o -iname *.hpp | xargs clang-format -i -style=file
