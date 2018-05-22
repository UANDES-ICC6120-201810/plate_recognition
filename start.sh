#!/bin/bash

echo
echo "Pulling from git repo"
echo
git pull

if [ $? -eq 0 ]
then
    ./scripts/start.sh
else
    echo
    echo "Git pull failed, check with 'git status'"
fi
