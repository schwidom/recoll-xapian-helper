#!/bin/bash

set -e

source bin/list-preliminaries.sh.source

bin/list-non-existing-files-from-index.sh "$@" |
 { while read fil
  do
   printf "deleting from index: %s\n" "$fil"
   "$RECOLLINDEX" -e "$fil"
  done 
 }

