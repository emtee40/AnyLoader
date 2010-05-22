#!/bin/sh
git ls-tree --full-tree -r HEAD | awk '{print $4}' | grep -v daemon/HandBrakeCLI | xargs cat | wc -l
