#!/bin/bash

set -e
set -u

RECOLLINDEX="$(which recollindex)"
GREP="$(which grep)"
SED="$(which sed)"

set -x
test -f recoll-xapian-helper
test -x recoll-xapian-helper
test -f "$RECOLLINDEX"
test -x "$RECOLLINDEX"
set +x

PATH="$(realpath "$1")"
echo "use path $PATH"

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data |
 "$GREP" '^url=file' | "$GREP" -F "$PATH" |
 "$SED" 's+^url=file://\(.*\)+\1+1' |
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

