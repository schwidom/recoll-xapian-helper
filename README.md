$ ./recoll-xapian-helper --markdown --author --version --help --help-data --help-future-plans

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
      --help-future-plans
</pre></code></div>

# The Data: (--help-data)
<div><code><pre>

 In case of the recoll database the '--data' option outputs a lot of 
 metadata about a specific document. The most interesting part of the
 data is the url. Because the recoll database I have to deal with 
 (version 1.17) is not able to detect deleted files and this way 
 the database increases continuously on every indexing run, I searched
 a way to solve this problem. First by analyzing recoll and xapian
 tools but no one could give me a list of the indexed files which 
 I could have been used to remove deleted files from the index
 via 'recollindex -e'.

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

# Future Plans : (--help-future-plans)
<div><code><pre>

 Currently for an enduser to have a valuable benefit from the program 
 their has to program some bash magic itself and has at least the relief of
 the existing bin/drop-non-existent-files-from-index.sh tool which contains
 an example usage of the recoll-xapian-helper but is not very well 
 documented yet. 

 When I am in the mood I will maybe add some options which allow to 
 search and unindex files without the need of external tools.

 But to go over bash scripting is of course the most flexible approach.


 Because the recoll database can grow very large with tons of documents,
 my current approach to query the database for files can be last very
 long. I figured out that by questioning via

  ./recoll-xapian-helper ... --data-regex '^url=file://(/.*)' \
   --regex-sub 1 --max-matches 1

 the first 10000 documents took 0.1 seconds. So on a Database with 5 million
 document entries it will take at least 50 seconds.

 So it could be handy to have an optional duration estimation of the search
 process.


</pre></code></div>
