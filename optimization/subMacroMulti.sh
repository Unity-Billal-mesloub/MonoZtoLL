#!/usr/bin/env bash

set -eu -o pipefail

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

# # shellcheck disable=SC2046
c++ -g -O3 $(root-config --glibs --cflags) -o "${macroName}.o" "${sendSpace}/${macroName}.C" "${sendSpace}/untuplizer.h"
chmod u+x "./${macroName}.o"

# # shellcheck disable=SC2046
# xrdcp -v --parallel 4 --retry 5 $(cat "$sendSpace/$inputListFile") "$infileDir/"

"./${macroName}.o" "$outputFile" $(cat "$sendSpace/$inputListFile")

duration="$SECONDS"
echo -e "RUN TIME: $((duration / 60)) minutes and $((duration % 60)) seconds"
echo "Done"
