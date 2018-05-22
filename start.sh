#!/bin/bash

echo
echo "Pulling from git repo"
echo
git pull > /dev/null 2>&1

if [ $? -eq 0 ]
then
    echo "Git pull succeeded"
    ./scripts/start.sh
else
    echo "Git pull failed, check with 'git status'"
fi
