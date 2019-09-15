#!/bin/bash

mkdir -p tmp/
touch tmp/cleanup-build-files_progress.txt

rm -rfv \
\
Makefile \
aclocal.m4 \
autom4te.cache/ \
config.h \
config.h.in~ \
config.log \
config.status \
recoll-xapian-helper \
src/.deps/ \
src/.dirstamp \
src/main.o \
stamp-h1 \
tmp/cleanup-build-files_progress.txt

