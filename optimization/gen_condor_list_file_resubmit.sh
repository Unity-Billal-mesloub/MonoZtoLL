#!/usr/bin/env bash

set -eu -o pipefail

if [[ -z "${1-}" ]]; then
	echo "Expect argument clusterId" >&2
	exit 1
fi

clusterId="$1"

originalListSpace="SkimmedFiles_2017Background_Lists"
splittedListSpace="${originalListSpace}_splitted.tmp"
# listFile="${originalListSpace}_listfile.txt.tmp"

# find "$splittedListSpace" -type f -name "*.txt" > "$listFile"

outputLogSpace="${originalListSpace}_outputlog.tmp"

if [[ "${2-}" == "-" ]]; then
	listFileResubmit=/dev/stdout
else
	listFileResubmit="${2:-${originalListSpace}_listfile_resubmit_${clusterId}.txt.tmp}"
fi

if [[ -f "$listFileResubmit" ]]; then
	rm "$listFileResubmit"
fi

if [[ ! -e "$listFileResubmit" ]]; then
	touch "$listFileResubmit"
fi

while IFS= read -r -d $'\0' outputLogPath; do
	outputLogDirname="$(dirname "$outputLogPath")"
	# replace prefixing "$outputLogSpace" with "$splittedListSpace"
	# and remove the trailing "/out"
	inputFileDirname="${splittedListSpace}${outputLogDirname:${#outputLogSpace}:-4}"
	inputFileBasename="$(basename -s .out "$outputLogPath" | sed -E "s#^job_${clusterId}_[0-9]+_##").txt"
	echo "$inputFileDirname/$inputFileBasename" >> "$listFileResubmit"
done < <(find "$outputLogSpace" -type f -name "job_${clusterId}_*.out" ! -exec grep -q "^Done\$" {} \; -print0)
