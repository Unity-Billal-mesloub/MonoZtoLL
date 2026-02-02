#!/usr/bin/env bash

SECONDS=0

sendSpace="$1"
macroName="$2"
inputListFile="$3"

export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

originalListSpaceName="SkimmedFiles_2017Background_Lists"

splittedListSpace="${originalListSpaceName}_splitted.tmp"
outputSpace="/eos/user/y/yuehshun/${originalListSpaceName}_output.tmp"
outputFile="$(dirname "$outputSpace${inputListFile:${#splittedListSpace}}")/$(basename -s .txt "$inputListFile").root"

export X509_USER_PROXY=/afs/cern.ch/user/y/yuehshun/private/x509up
echo "X509_USER_PROXY=$X509_USER_PROXY"

# # # shellcheck disable=SC2046
# c++ -g -O3 $(root-config --glibs --cflags) -o "${macroName}.o" "${sendSpace}/${macroName}.C" "${sendSpace}/untuplizer.h"
# chmod u+x "./${macroName}.o"

# # # shellcheck disable=SC2046
# # xrdcp -v --parallel 4 --retry 5 $(cat "$sendSpace/$inputListFile") "$infileDir/"

# "./${macroName}.o" "$outputFile" $(cat "$sendSpace/$inputListFile")

while IFS= read -r -d $'\n' inputFileUrl; do
	echo "Trying $inputFileUrl" >&2
	tempOutlog="$(mktemp -t check_connection_XXXXXX)"
	tempErrlog="$(mktemp -t check_connection_XXXXXX)"
	# Reference https://stackoverflow.com/questions/692000/how-do-i-write-standard-error-to-a-file-while-using-tee-with-a-pipe
  # but print all to stderr
	root -l -q "$inputFileUrl" > >(tee -a "$tempOutlog" >&2) 2> >(tee -a "$tempErrlog" >&2)
	if grep -q "nullptr" "$tempOutlog"; then
		echo "$inputFileUrl"
	fi
	rm "$tempOutlog"
	rm "$tempErrlog"
done < "$sendSpace/$inputListFile"

duration="$SECONDS"
echo -e "RUN TIME: $((duration / 60)) minutes and $((duration % 60)) seconds"
# echo "Done"
