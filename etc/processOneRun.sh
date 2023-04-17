#!/bin/zsh

# check for 1 argument
if [[ $# -ne 1 ]]; then
    echo "Usage: ./processOneRun.sh <runID>" >&2
    exit 2
fi

runID=$1

# run Fangorn, PruneTree, Reconstruct for one built ROOT file
root -l -q "Fangorn.C($runID)"
root -l -q "PruneTree.C($runID)"
root -l -q "Reconstruct.C($runID)"

echo "\n Finished processing run $runID"

echo "" >&2