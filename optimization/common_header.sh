# shellcheck shell=bash

function wrapDryRun {
	if (( "${dryRun-}" )); then
		echo "dryrun:" "$@"
	else
		"$@"
	fi
}

function manageShorthands {
	local str_short_args="${1:1}"
	shift
	for ((i="${#str_short_args}"-1;i>=0;--i)); do
		set -- "$@" "${str_short_args:$i:1}";
	done
}
