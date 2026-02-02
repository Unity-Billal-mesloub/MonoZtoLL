#!/usr/bin/env bash

originalListSpace="SkimmedFiles_2017Background_Lists"
splittedListSpace="${originalListSpace}_splitted.tmp"
listFile="${originalListSpace}_listfile.txt.tmp"

find "$splittedListSpace" -type f -name "*.txt" > "$listFile"
