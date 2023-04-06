#! /bin/bash

set -e

git checkout -B gh-pages

doxygen Doxyfile

cp -r docs/html/* docs/

git add -f docs/*

git commit -m "Push Docs" 

git push -f --set-upstream origin gh-pages
