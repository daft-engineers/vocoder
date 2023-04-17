#! /bin/bash

# daft engineers vocoder project
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set -e

git checkout -B gh-pages

doxygen Doxyfile

cp -r docs/html/* docs/

git add -f docs/*

git commit -m "Push Docs" 

git remote set-url origin git@github.com:daft-engineers/vocoder.git

git push -f --set-upstream origin gh-pages
