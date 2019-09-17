
#include "../config.h"

#include <xapian.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm> // find_if
#include <regex> 
#include <tuple>

#include <cstdlib> // exit
#include <cctype> // isprint, isascii

#include <getopt.h> // getopt_long
#include <stdio.h> // printf

#define OS(debug,x) (debug ? "debug: " #x ": " : "") << ( x)

int main( int argc, char** argv)
{

 std::set<std::string> option_set;

 auto chkoption = [&option_set](std::string name) -> bool {
  return option_set.find(name) != option_set.end();
 };
 
 std::string prefix = "";
 std::string dblocation = "";

 enum class NonChars : int { NONE = 0
  , HELP_BUGS = 0x10000
  , MARKDOWN = 0x10001
  , HELP_REGEX = 0x10002
  , MAX_MATCHES = 0x10003
  , HELP_FUTURE_PLANS = 0x10004
 };

 std::vector<struct option> const long_options_vector 
 {
  {"help",     no_argument, 0, 'h' },
  {"author",     no_argument, 0,  'a' },
  {"version",     no_argument, 0,  'v' },
  {"dblocation",  required_argument, 0,  'o' },
  {"document-count",  no_argument, 0, 'c'},
  {"description1",  no_argument, 0,  '1' },
  {"doclength",  no_argument, 0,  'l' },
  {"wdf",  no_argument, 0,  'w' },
  {"docid",  no_argument, 0,  'i' },
  {"description2",  no_argument, 0,  '2' },
  {"data",  no_argument, 0,  'd'},
  {"help-data",  no_argument, 0,  '@'},
  {"data-regex",  required_argument, 0,  'r'},
  {"max-matches",  required_argument, 0,  static_cast<int>(NonChars::MAX_MATCHES)},
  {"regex-sub",  required_argument, 0,  's'},
  {"help-regex",  no_argument, 0,  static_cast<int>(NonChars::HELP_REGEX)},
  {"prefix",  required_argument, 0,  'p' },
  {"debug",    no_argument, 0,  'z' },
  {"help-bugs",  no_argument, 0,  static_cast<int>(NonChars::HELP_BUGS)},
  {"markdown",  no_argument, 0,  static_cast<int>(NonChars::MARKDOWN)},
  {"help-future-plans",  no_argument, 0,  static_cast<int>(NonChars::HELP_FUTURE_PLANS)},
  {0,         0,                 0,  0 }
 };

 static std::map< std::string, std::string> const help_map {
  { "help", "prints help"},
  { "dblocation", "mandatory parameter, path of xapian database location (directory) "},
  { "data", "provides the interesting data see --help-data"},
  { "prefix", "prefix determines the prefix for non document output lines, default \"\""},
  { "markdown", "markdown output for help pages"},
  { "data-regex", "in case of --data, repeatable, needs regex parameter, filters data output, see --help-regex"},
  { "regex-sub", "in case of --data-regex, repeatable, needs count parameter, chooses the sub expression, see --help-regex"},
 };

 auto get_help = []( std::string longoptionname) -> std::string 
 {
  auto found = help_map.find( longoptionname);
  if( found != help_map.end()){ return " : " + found->second;}
  else return "";
 };

 /** if that function is called then the parameter optstring getopt_long misses a character and/or a colon */
 auto exit_if_programmer_forgot_optstring_entry = []() -> void
 {
  if( ! optarg)
  {
   std::cerr << "no parameter given, exiting" << std::endl;
   std::exit( 1);
  }
 };

 std::vector<std::tuple<std::string,std::set<int>>> regex_vector2; // TODO : uint , rename to opt_regex_vector

 int opt_max_matches= 0; // 0 = all

 {
  int c;

  while (1) {

   int option_index = 0;

   // static struct option long_options[] =

   c = getopt_long(argc, argv, "havo:c1lwi2dr:s:p:@z", long_options_vector.data(), &option_index);

   if (c == -1)
       break;

   auto found = std::find_if( long_options_vector.begin(), long_options_vector.end(), 
    [c]( struct option const & o) -> bool { return o.val == c;});

   auto exit_if_programmer_forgot_longoption = [&long_options_vector, &found, &c]() -> void
   {
    if( found == long_options_vector.end())
    {
     std::cout << "longoption not found, c: " << c << std::endl;
     std::exit( 1);
    }
   };

   switch (c) {
    case 0:
        printf("option not expected %s", long_options_vector.at( option_index).name);
        if (optarg)
            printf(" with arg %s", optarg);
        printf("\n");
        std::exit( 1);
        break;

    case 'h':
    case 'a':
    case 'v':
    case 'c':
    case '1':
    case 'l':
    case 'w':
    case 'i':
    case '2':
    case 'd':
    case '@':
    case 'z':
    case static_cast<int>(NonChars::HELP_BUGS):
    case static_cast<int>(NonChars::MARKDOWN):
    case static_cast<int>(NonChars::HELP_REGEX):
    case static_cast<int>(NonChars::HELP_FUTURE_PLANS):

     exit_if_programmer_forgot_longoption();
     option_set.emplace(found -> name); 

     break;

    case 'o':

     exit_if_programmer_forgot_longoption();
     exit_if_programmer_forgot_optstring_entry();

     dblocation = optarg;

     break;

    case 'p':

     exit_if_programmer_forgot_longoption();
     exit_if_programmer_forgot_optstring_entry();

     prefix = optarg;

     break;

    case 'r':

     exit_if_programmer_forgot_longoption();
     exit_if_programmer_forgot_optstring_entry();

     regex_vector2.emplace_back( std::make_tuple( optarg, std::set<int>{}));

     break;

    case static_cast<int>(NonChars::MAX_MATCHES):

     exit_if_programmer_forgot_longoption();
     exit_if_programmer_forgot_optstring_entry();

     { // TODO : generalize
      int number;
      std::istringstream iss( optarg);
      iss >> number;
      if (iss.fail()) {
       std::cerr << "--" << found -> name << "could not convert to number: " << std::endl;
       std::cerr << optarg << std::endl;
      }

      opt_max_matches = number;

     }

     break;

    case 's':

     exit_if_programmer_forgot_longoption();
     exit_if_programmer_forgot_optstring_entry();

     if( regex_vector2.empty()){
      std::cerr << "--" << found -> name << "no --data-regex given, exiting" << std::endl;
      std::exit( 1);
     }

     { // TODO : generalize
      int number;
      std::istringstream iss( optarg);
      iss >> number;
      if (iss.fail()) {
       std::cerr << "--" << found -> name << "could not convert to number: " << std::endl;
       std::cerr << optarg << std::endl;
      }

     std::get<1>( regex_vector2.back()).insert(number);

     }

     break;

    default:
      printf("?? getopt returned character code 0%o ??\n", c);
      std::exit( 1);
   }
  }

  if (optind < argc) {
   std::cout << "trailing chars on the commandline" << std::endl;
   std::exit( 1);
  }
 }

 // TODO : "author" => AUTHOR

 if( false 
  || chkoption( "author")
  || chkoption( "help")
  || chkoption( "version")
  || chkoption( "help-data")
  || chkoption( "help-bugs")
  || chkoption( "help-regex")
  || chkoption( "help-future-plans")
 )
 {
  bool markdown = chkoption( "markdown");

  std::string markdown_header = ( markdown ? "# " : "");

  auto block_begin = [markdown](){
   if( markdown) {
    std::cout << "<div><code><pre>" << std::endl;
   }
  };

  auto block_end = [markdown](){
   if( markdown) {
    std::cout << "</pre></code></div>" << std::endl;
   }
  };

  std::cout << markdown_header << "The Recoll Xapian Helper" << std::endl;
  std::cout << std::endl;

  if( chkoption( "author")) {
   std::cout << "Author: Frank Schwidom <schwidom@gmx.net>" << std::endl;
   std::cout << std::endl;
  }

  if( chkoption( "version")) {
   std::cout << "Version: " << VERSION << std::endl;
   std::cout << std::endl;
  }

  if( chkoption( "help"))
  {
   std::cout << markdown_header << "Help: (--help)" << std::endl;

   block_begin();
   for( auto& o : long_options_vector) { 
    if( nullptr == o.name) { break;}
    std::cout << ( isprint( o.val)  && isascii( o.val) 
     ? std::string("-") + static_cast<char>(o.val) + " , " 
     : "     " )
     << " --" << o.name 
     << get_help( o.name) << std::endl;   
   }
   block_end();
   std::cout << std::endl;
  }

  if( chkoption( "help-data"))
  {
   std::cout << markdown_header << "The Data: (--help-data)" << std::endl;

   block_begin();
   std::cout << R"(
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

)" << std::endl;
   block_end();
   std::cout << std::endl;
  }

  if( chkoption( "help-future-plans"))
  {
   std::cout << markdown_header << "Future Plans : (--help-future-plans)" << std::endl;

   block_begin();
   std::cout << R"(

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

)" << std::endl;
   block_end();
   std::cout << std::endl;
  }

  if( chkoption( "help-regex"))
  {
   std::cout << markdown_header << "The Regex: (--help-regex)" << std::endl;

   block_begin();
   std::cout << R"(
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

)" << std::endl;
   block_end();
   std::cout << std::endl;
  }

  if( chkoption( "help-bugs"))
  {
   std::cout << markdown_header << "The Bugs: (--help-bugs)" << std::endl;

   block_begin();
   std::cout << R"(
 It is currently not clear how to match the url of a recoll document
 in the xapian database if the name of the original file contains a newline.
)" << std::endl;
   block_end();
   std::cout << std::endl;
  }

  std::exit( 0);
 }

 if( "" == dblocation)
 {
  std::cout << "--dblocation is missing " << get_help( "dblocation") << ", (--dblocation ~/.recoll/xapiandb) , see --help" << std::endl;
  std::exit( 1);
 }

 // terminate called after throwing an instance of 'Xapian::DatabaseOpeningError'
 Xapian::Database db( dblocation);


 // db.get_doccount returns doccount = typedef unsigned doccount => int;

 bool opt_document_count = chkoption("document-count");
 bool opt_description1 = chkoption("description1");
 bool opt_doclength = chkoption("doclength");
 bool opt_wdf = chkoption("wdf");
 bool opt_docid = chkoption("docid");
 bool opt_description2 = chkoption("description2");
 bool opt_data = chkoption("data");
 bool opt_debug = chkoption("debug");

 if( opt_document_count){
  std::cout << prefix << "doccount: " << OS( opt_debug, db.get_doccount() ) << std::endl; 
 }

 if( false 
  | opt_description1
  | opt_doclength
  | opt_wdf
  | opt_docid
  | opt_description2
  | opt_data
 )
 {

 std::vector<std::tuple<std::regex,std::set<int>>> regex_vector3; // TODO : uint

  for( auto const & regex_tuple : regex_vector2)
  { 
   auto const & regex_string = std::get<0>(regex_tuple);
   auto const & sub_number_set = std::get<1>(regex_tuple);

   if( opt_debug) { std::cout << "debug: regex_string: " << regex_string << std::endl;}

   std::regex re( regex_string, std::regex_constants::ECMAScript | std::regex_constants::optimize); // ecmascript is default

   // regex_vector3.emplace_back( std::make_tuple( regex_string, sub_number_set));
   // regex_vector3.emplace_back( std::piecewise_construct, std::forward_as_tuple( regex_string)
    // , std::forward_as_tuple( sub_number_set));

   regex_vector3.emplace_back( std::make_tuple( re, sub_number_set)); // TODO : piecewise_construct
  }

  for( auto postingIterator = db.postlist_begin( ""); postingIterator != db.postlist_end(""); ++postingIterator)
  {

   if( opt_description1) {
    std::cout << prefix << "description1: " << OS( opt_debug, postingIterator.get_description()) << std::endl;
   }

   if( opt_doclength) {
    std::cout << prefix << "doclength: " << OS( opt_debug, postingIterator.get_doclength()) << std::endl;
   }

   if( opt_wdf) {
    std::cout << prefix << "wdf: " << OS( opt_debug, postingIterator.get_wdf())  << std::endl;
   }

   if( opt_docid)
   {
    std::cout << prefix << "docid: " << OS( opt_debug, *postingIterator)  << std::endl;
   }

   const auto document = db.get_document( *postingIterator);
  
   if( opt_description2) {
    std::cout << prefix << "description2: " << OS( opt_debug, document.get_description() ) << std::endl;
   }

   if( opt_data)
   {

    /**

    std::cout << OS( opt_debug, document.get_value( 0)) << std::endl; 
    std::cout << OS( opt_debug, document.get_value( 1)) << std::endl; 
    std::cout << OS( opt_debug, document.get_value( 2)) << std::endl; 
    std::cout << OS( opt_debug, document.get_value( 3)) << std::endl; 
    std::cout << OS( opt_debug, document.get_value( 4)) << std::endl; 
    std::cout << OS( opt_debug, document.termlist_count()) << std::endl; 
    std::cout << OS( opt_debug, document.values_count()) << std::endl; 

    for( auto termlist_value = document.termlist_begin()
     ; termlist_value != document.termlist_end(); ++termlist_value)
    {
     std::cout << OS( opt_debug, *termlist_value) << std::endl;
    }

    for( auto values_value = document.values_begin()
     ; values_value != document.values_end(); ++values_value)
    {
     std::cout << OS( opt_debug, *values_value) << std::endl;
    }

    */

    auto got_data = std::string( document.get_data()); // working on a copy!

    for( auto const & regex_tuple : regex_vector3)
    {
     auto const & regex_re = std::get<0>(regex_tuple);
     auto const & sub_number_set = std::get<1>(regex_tuple);

     bool matched = false;
     int matchcount = 0;

     for( auto regex_iterator = std::sregex_iterator(got_data.begin(), got_data.end(), regex_re)
      ; regex_iterator != std::sregex_iterator() 
      ; ++regex_iterator)
     {
      std::smatch match_results = *regex_iterator;
      int subs = 0;
      for( auto const & match_result : match_results) {

       if( sub_number_set.empty())
       {
        matched = true;
        std::cout << OS( opt_debug, match_result.str()) << std::endl;
       }else
       {
        if( sub_number_set.find(subs) != sub_number_set.end())
        {
         matched = true;
         std::cout << OS( opt_debug, match_result.str()) << std::endl;
        }
       }

       ++subs;
      }

      ++matchcount;

      // be careful changing things here, it can break the runtime behaviour (fast => slow)
      if( opt_max_matches != 0 && opt_max_matches == matchcount) { break;}
     }

     if( !matched)
     {
      if( opt_debug) {
       std::cout << "debug: regex did not match" << std::endl;
      }
     }
    }

    if( regex_vector2.empty())
    {
     if( opt_debug) { std::cout << prefix << "data: document.get_data()" << std::endl; } 
     else { std::cout << prefix << " data:" << std::endl; }
     std::cout << got_data << std::endl; // contains url
    }
   }
  }
 }

 return 0;
}

