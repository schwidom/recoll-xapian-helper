
#include <xapian.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm> // find_if

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
  {"prefix",  required_argument, 0,  'p' },
  {"debug",    no_argument, 0,  'z' },
  {"help-bugs",  no_argument, 0,  static_cast<int>(NonChars::HELP_BUGS)},
  {0,         0,                 0,  0 }
 };

 static std::map< std::string, std::string> const help_map {
  { "help", "prints help"},
  { "dblocation", "mandatory parameter, path of xapian database location (directory) "},
  { "data", "provides the interesting data see --help-data"},
  { "prefix", "prefix determines the prefix for non document output lines, default \"\""},
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

 {
  int c;

  while (1) {

   int option_index = 0;

   // static struct option long_options[] =

   c = getopt_long(argc, argv, "havo:c1lwi2dp:@z", long_options_vector.data(), &option_index);

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
 )
 {
  std::cout << "The Recoll Xapian Helper" << std::endl;
  std::cout << std::endl;

  if( chkoption( "author")) {
   std::cout << "Author: Frank Schwidom <schwidom@gmx.net>" << std::endl;
   std::cout << std::endl;
  }

  if( chkoption( "version")) {
   std::cout << "Version: 0.1" << std::endl;
   std::cout << std::endl;
  }

  if( chkoption( "help"))
  {
   for( auto& o : long_options_vector) { 
    if( nullptr == o.name) { break;}
    std::cout << ( isprint( o.val)  && isascii( o.val) 
     ? std::string("-") + static_cast<char>(o.val) + " , " 
     : "     " )
     << " --" << o.name 
     << get_help( o.name) << std::endl;   
   }
   std::cout << std::endl;
  }

  if( chkoption( "help-data"))
  {
   std::cout << R"(The Data: (--help-data)

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

 like:

  /etc/passwd

 At this point you can throw the output in a file, select a specific
 directory whose entries you want to have deleted and reindexed by:

 cat list.txt | recollindex -e # for deleting files
 find . <specific directory> | recollindex -i -f


 Happy indexing.

)" << std::endl;
   std::cout << std::endl;
  }

  if( chkoption( "help-bugs"))
  {
   std::cout << R"(The Bugs: (--help-bugs)

 It is currently not clear how to match the url of a recoll document
 in the xapian database if the name of the original file contains a newline.
)" << std::endl;
   std::cout << std::endl;
  }

  std::exit( 0);
 }

 if( "" == dblocation)
 {
  std::cout << "dblocation is missing " << get_help( "dblocation") << ", see --help" << std::endl;
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
    if( opt_debug) { std::cout << prefix << " data: document.get_data()" << std::endl; } 
    else { std::cout << prefix << " data:" << std::endl; }
    std::cout << document.get_data() << std::endl; // contains url
   }
  }
 }

 return 0;
}

