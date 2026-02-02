#!/usr/bin/env bash

set -eu -o pipefail

originalListSpace="SkimmedFiles_2017Background_Lists"
splittedListSpace="${originalListSpace}_splitted.tmp"
outputSpace="/eos/user/y/yuehshun/${originalListSpace}_output.tmp"
outputLogSpace="${originalListSpace}_outputlog.tmp"
NO_WRITE_OUTPUT_DIRS=0

templateDYJetsExcl="DYJetsToLL_M-50_HT-@LOWER@to@UPPER@_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8"
nameDYJetsIncl="DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8"

if [ ! -e "$(dirname "$outputSpace")" ]; then
	echo "$(dirname "$outputSpace") not found. Not writing output directories" >&2
	NO_WRITE_OUTPUT_DIRS=1
fi

declare -i nFilesPerJob=8

while IFS= read -r -d $'\0' file; do
	echo "working on filelist $file" >&2
	if [[ "$(basename -s ".txt" "$file")" == "$nameDYJetsIncl" ]]; then
		echo "skipping inclusive dataset $file" >&2;
		continue
	fi
	subdir="${file:$(( ${#originalListSpace} + 1)):-4}"
	splittedListDir="$splittedListSpace/$subdir"
	if [[ -e  "$splittedListDir" ]]; then
		echo "removing:$splittedListDir" >&2
		rm -rf "$splittedListDir"
	fi
	mkdir -vp "$splittedListDir"
	i=1;
	j=0;
	while read -r line; do
		if [ -n "$line" ]; then
			fileToCreate="$splittedListDir/splitted_$(basename "$splittedListDir")_$i.txt"
			if ! [ -e "$fileToCreate" ]; then
				echo "creating $fileToCreate" >&2
			fi
			echo "$line" >> "$fileToCreate"
			j=$(( j + 1 ))
		fi
		if (( j >= nFilesPerJob )); then
		j=0
		i=$(( i + 1 ))
		fi
	done < "$file"
	if ! (( NO_WRITE_OUTPUT_DIRS )); then
		outputDir="$outputSpace/$subdir"
		[[ -e "$outputDir" ]] || mkdir -vp "$outputDir";
	fi
	outputLogDir="$outputLogSpace/$subdir"
	[[ -e "$outputLogDir" ]] || mkdir -p "$outputLogDir"
	for suf in out err; do
		[[ -e "$outputLogDir/$suf" ]] || mkdir -v "$outputLogDir/$suf"
	done
done < <(find "$originalListSpace" -name "*.txt" -print0)

declare -a inclusiveBoundaries=( 0 70 100 200 400 600 800 1200 2500 Inf )
for (( iLower=0; iLower<"${#inclusiveBoundaries[@]}"-1; iLower++ )) {
	subdir="${templateDYJetsExcl/@LOWER@/${inclusiveBoundaries[$iLower]}}"
	subdir="${subdir/@UPPER@/${inclusiveBoundaries[$iLower+1]}}"
	splittedListDir="$splittedListSpace/$subdir"
	if [[ -d "$splittedListDir" ]]; then
		find "$splittedListDir" -mindepth 1 -maxdepth 1 -type f -name "*_inclusive_*.txt" -delete
	else
		mkdir -v "$splittedListDir"
	fi
	i=1;
	j=0;
	while read -r line; do
		if [ -n "$line" ]; then
			fileToCreate="$splittedListDir/splitted_${nameDYJetsIncl}_inclusive_$i.txt"
			if ! [ -e "$fileToCreate" ]; then
				echo "creating $fileToCreate" >&2
			fi
			echo "$line" >> "$fileToCreate"
			j=$(( j + 1 ))
		fi
		if (( j >= nFilesPerJob )); then
		j=0
		i=$(( i + 1 ))
		fi
	done < "$originalListSpace/$nameDYJetsIncl.txt"
	if ! (( NO_WRITE_OUTPUT_DIRS )); then
		outputDir="$outputSpace/$subdir"
		[[ -e "$outputDir" ]] || mkdir -vp "$outputDir";
	fi
	outputLogDir="$outputLogSpace/$subdir"
	[[ -e "$outputLogDir" ]] || mkdir -p "$outputLogDir"
	for suf in out err; do
		[[ -e "$outputLogDir/$suf" ]] || mkdir -v "$outputLogDir/$suf"
	done
}
