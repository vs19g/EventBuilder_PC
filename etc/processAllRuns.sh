#!/bin/zsh

# check for 2 arguments
if [[ $# -ne 2 ]]; then
    echo "Usage: ./processAllRuns.sh <firstRun> <lastRun>" >&2
    exit 2
fi

firstRun=$1
lastRun=$2

# run processOneRun.sh for a range of built ROOT files
# non-existent files are automatically skipped
for run in {$firstRun..$lastRun}
do
	./processOneRun.sh $run
done

echo "\n All done! Completed runs $firstRun-$lastRun"

echo "" >&2