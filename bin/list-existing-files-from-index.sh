#!/bin/bash

source bin/list-preliminaries.sh.source

bin/list-files-from-index.sh "$@" |
 { while read fil
  do
  if test -e "$fil"; then
   printf "%s\n" "$fil"
  fi
  done 
 }

