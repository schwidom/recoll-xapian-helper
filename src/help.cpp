
#include <string>

#include "help.h"

namespace help {
 
 std::string help_data()
 {
   return R"(
 The option '--data' outputs a lot of metadata about a specific 
 document from a recoll database. The most interesting part of that
 data is the url. Because the recoll database I have to deal with 
 (version 1.17) is not able to detect deleted files and this way 
 the database increases continuously on every indexing run, I searched
 a way to solve this problem. First by analyzing the tools of recoll 
 and xapian but none of that could give me a list of the indexed files 
 which I could have been used to remove deleted files from the index
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

)";
  
 };

 std::string help_future_plans() {

   return R"(

 Currently, an end user has to program some bash magic himself to gain
 valuable value from the program, and at least has the convenience of 
 using the existing tool bin/drop-non-existent-files-from-index.sh.

 This tool contains an example use of the recoll-xapian-helper but
 is not very vell documented yet.

 When I am in the mood I will maybe add some options which allow to 
 search and unindex files without the need of external tools.

 But to go over bash scripting is of course the most flexible approach.


 Because the recoll database can grow very large with tons of documents,
 my current approach to query the database for files can last very
 long. I figured out that by questioning via

  ./recoll-xapian-helper ... --data-regex '^url=file://(/.*)' \
   --regex-sub 1 --max-matches 1

 the first 10000 documents took 0.1 seconds. So on a Database with 5 million
 document entries it will take at least 50 seconds.

 So it could be handy to have an optional duration estimation of the search
 process.

)";

 }

 std::string help_regex() {

  return R"(

 The options --data-regex and --regex-sub provide to filter the data element which 
 can be seen when only the --data option is passed. 

 The data contains usually data like:

 url=file:///
 mtype=application/x-fsdirectory
 fmtime=01568306963
 origcharset=
 fbytes=4096
 pcbytes=4096
 dbytes=0
 sig=40961568306963

 What we want to have is the url.

 If we are not sure whether the url begins every time at the top we should let the regex begin
 with (^|\n): e.g. "'(^|\n)(.*)'"
 
 example:

 $ ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data --data-regex '(^|\n)(.*)' --regex-sub 2 | head -n10

 outputs:
 
 url=file:///
 mtype=application/x-fsdirectory
 fmtime=01568306963
 origcharset=
 fbytes=4096
 pcbytes=4096
 dbytes=0
 sig=40961568306963

 --regex-sub 2 chooses the second submatch from the second open beace th its closing counterpart.

 If we want to only have the urls we choose '(^|\n)(url=.*)' 

 example:

 $ ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data --data-regex '(^|\n)(url.*)' --regex-sub 2 | head -n 5

 url=file:///
 url=file:///etc
 url=file:///etc/wpa_supplicant
 url=file:///etc/wpa_supplicant/action_wpa.sh
 url=file:///etc/wpa_supplicant/functions.sh

 now we only want to have the filenames, so we set the second submatch according to:

 ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data --data-regex '(^|\n)url=file://(/.*)' --regex-sub 2 | head -n 5

 /
 /etc
 /etc/wpa_supplicant
 /etc/wpa_supplicant/action_wpa.sh
 /etc/wpa_supplicant/functions.sh
 
 10000 Elements are chosen in 5 sec

 As of now this is a very expensive operation because after the first match of url=file 
 the program tries to find further matches. With --max-matches 1 we are fine.

 ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data --data-regex '(^|\n)url=file://(/.*)' --regex-sub 2 --max-matches 1 | head -n 5

 10000 Elements are chosen in 1 sec

 but much faster is relying on that the url every time is on the first place in the data:

 ./recoll-xapian-helper --dblocation ~/.recoll/xapiandb --data --data-regex '^url=file://(/.*)' --regex-sub 1 --max-matches 1 | head -n 5

 10000 Elements are chosen in 0.1 sec

 The tool bin/drop-non-existent-files-from-index.sh makes use of that.

)";

 }

 std::string help_bugs()
 {
  return R"(

 It is currently not clear how to match the url of a recoll document
 in the xapian database if the name of the original file contains a newline.
)";

 }

} // namespace help

