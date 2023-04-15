#! /bin/bash

set -e

git checkout -B gh-pages

doxygen Doxyfile

cp -r docs/html/* docs/

git add -f docs/*

git commit -m "Push Docs" 

git remote set-url origin git@github.com:daft-engineers/vocoder.git

git push -f --set-upstream origin gh-pages
