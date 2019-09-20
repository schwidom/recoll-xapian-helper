#!/bin/bash

source bin/list-preliminaries.sh.source

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data \
  --data-regex '^url=file://(/.*)' --regex-sub 1 --max-matches 1 |
 "$UNIQ" |
 "$GREP" -F "$PATH_INSIDE_INDEX" |
 { while read fil
  do
  if [[ "$fil" == "$PATH_INSIDE_INDEX"* ]]; then
   printf "%s\n" "$fil"
  fi
  done 
 }

