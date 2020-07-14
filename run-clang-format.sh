#!/bin/bash

git ls-files -- '*.c' '*.h' | xargs clang-format -i -style=.clang-format
git diff --exit-code --color
