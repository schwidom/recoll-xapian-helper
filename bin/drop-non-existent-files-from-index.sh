#!/bin/bash

set -e
set -u

RECOLLINDEX="$(which recollindex)"
GREP="$(which grep)"
SED="$(which sed)"
UNIQ="$(which uniq)"

set -x
test -f recoll-xapian-helper
test -x recoll-xapian-helper
test -f "$RECOLLINDEX"
test -x "$RECOLLINDEX"
set +x

PATH="$(realpath "$1")"
echo "use path $PATH"

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data \
  --data-regex '^url=file://(/.*)' --regex-sub 1 --max-matches 1 |
 "$UNIQ" |
 "$GREP" -F "$PATH" |
 { while read fil
  do
  if [[ $fil == "$PATH"* ]]; then
  if ! test -e $fil; then
   printf "deleting from index: %s\n" "$fil"
   "$RECOLLINDEX" -e "$fil"
  fi
  fi
  done 
 }

