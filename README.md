$ ./recoll-xapian-helper --markdown --author --version --help --help-data

# The Recoll Xapian Helper

Author: Frank Schwidom <schwidom@gmx.net>

Version: 0.3

# Help: (--help)
<div><code><pre>
-h ,  --help : prints help
-a ,  --author
-v ,  --version
-o ,  --dblocation : mandatory parameter, path of xapian database location (directory) 
-c ,  --document-count
-1 ,  --description1
-l ,  --doclength
-w ,  --wdf
-i ,  --docid
-2 ,  --description2
-d ,  --data : provides the interesting data see --help-data
-@ ,  --help-data
-r ,  --data-regex : in case of --data, repeatable, needs regex parameter, filters data output, see --help-regex
      --max-matches
-s ,  --regex-sub : in case of --data-regex, repeatable, needs count parameter, chooses the sub expression, see --help-regex
      --help-regex
-p ,  --prefix : prefix determines the prefix for non document output lines, default ""
-z ,  --debug
      --help-bugs
      --markdown : markdown output for help pages
</pre></code></div>

# The Data: (--help-data)
<div><code><pre>

 In case of the recoll database the data option outputs a lot of 
 metadata about a specific document. The most interesting part in the
 data is the url. Because the recoll database I have to deal with 
 (version 1.17) is not able to detect deleted files and this way 
 the database increases continuously on every indexing run, I searched
 a way to solve this problem. First by analysing recoll and xapian
 tools but no one could give me a list of the indexed files. 

 The recollindex tool is able to delete files from the index but
 you have to know the file location. 

 The recoll-xapian-helper helps you to get all urls
 by:

  ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data |
   grep '^url=file'

 provides files in a form like (if indexed)

  url=file:///etc/passwd

 and you can create a list of files by 

  ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data |
   grep '^url=file' | sed 's+^url=file://\(.*\)+\1+1'

 or 
 
  ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data \
    --data-regex '^url=file://(/.*)' --regex-sub 1 --max-matches 1


 which shows the result like so:

  /etc/passwd

 At this point you can throw the output in a file, select a specific
 directory whose entries you want to have deleted and reindexed by (for example):

 cat list.txt | recollindex -e # for deleting files
 find . <specific directory> | recollindex -i -f

 It is suggested to perform a run through the tool 'uniq' because one file
 can be indexed more than once if it is an archive.
 
 see bin/drop-non-existent-files-from-index.sh

 Happy indexing.


</pre></code></div>
