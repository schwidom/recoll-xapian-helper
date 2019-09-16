#!/bin/bash

mkdir -p tmp

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --document-count

echo "wgdult2lh6" >"tmp/wgdult2lh6.txt"

recollindex -i -f tmp/wgdult2lh6.txt

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --document-count

rm tmp/wgdult2lh6.txt

bin/drop-non-existent-files-from-index.sh tmp

./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --document-count

