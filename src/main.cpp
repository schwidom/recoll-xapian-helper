
#include <xapian.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm> // find_if

#include <cstdlib> // exit

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

 static std::vector<struct option> const long_options_vector 
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
  {0,         0,                 0,  0 }
 };

 static std::map< std::string, std::string> const help_map {
  { "help", "prints help"},
  { "dblocation", "mandatory parameter, path of xapian database location (directory) "},
  { "data", "provides the interesting data see --help-data"},
 };

 auto get_help = []( std::string longoptionname) -> std::string 
 {
  auto found = help_map.find( longoptionname);
  if( found != help_map.end()){ return " : " + found->second;}
  else return "";
 };

 {
  int c;
  int digit_optind = 0;

  while (1) {

   int this_option_optind = optind ? optind : 1;
   int option_index = 0;

   // static struct option long_options[] =

   c = getopt_long(argc, argv, "havo:c1lwi2dp:@z", long_options_vector.data(), &option_index);

   if (c == -1)
       break;

   auto found = std::find_if( long_options_vector.begin(), long_options_vector.end(), 
    [c]( struct option const & o) -> bool { return o.val == c;});

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

     if( found == long_options_vector.end()) 
     { std::cout << "not found " << c << std::endl; }
     else
     {
      option_set.emplace(found -> name); 
     }

     break;

    case 'o':

     if( found == long_options_vector.end())
     {
      std::cout << "not found " << c << std::endl;
      std::exit( 1);
     }
     else
     {
      if( ! optarg)
      {
       std::cout << get_help( "dblocation") << c << std::endl;
       std::exit( 1);
      }
      dblocation = optarg;
     }

     break;

    case 'p':

     if( found == long_options_vector.end())
     {
      std::cout << "not found " << c << std::endl;
      std::exit( 1);
     }
     else
     {
      if( ! optarg)
      {
       std::cout << "parameter needed: prefix " << c << std::endl;
       std::exit( 1);
      }
      prefix = optarg;
     }

     break;

    default:
      printf("?? getopt returned character code 0%o ??\n", c);
      std::exit( 1);
   }
  }
 }

 // TODO : "author" => AUTHOR
 if( chkoption( "author") || chkoption( "help") || chkoption( "version") || chkoption( "help-data"))
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
    std::cout << "-" << static_cast<char>(o.val) 
     << ", --" << o.name 
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

  std::exit( 0);
 }

 if( "" == dblocation)
 {
  std::cout << "dblocation is missing " << get_help( "dblocation") << std::endl;
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
 bool opt_prefix = chkoption("prefix");
 bool opt_debug = chkoption("debug");

 bool debug{opt_debug}; // TODO remove

 if( opt_document_count){
  std::cout << prefix << "doccount: " << OS( debug, db.get_doccount() ) << std::endl; 
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
    std::cout << prefix << "description1: " << OS( debug, postingIterator.get_description()) << std::endl;
   }

   if( opt_doclength) {
    std::cout << prefix << "doclength: " << OS( debug, postingIterator.get_doclength()) << std::endl;
   }

   if( opt_wdf) {
    std::cout << prefix << "wdf: " << OS( debug, postingIterator.get_wdf())  << std::endl;
   }

   if( opt_docid)
   {
    std::cout << prefix << "docid: " << OS( debug, *postingIterator)  << std::endl;
   }

   const auto document = db.get_document( *postingIterator);
  
   if( opt_description2) {
    std::cout << prefix << "description2: " << OS( debug, document.get_description() ) << std::endl;
   }

   if( opt_data)
   {
    if( debug) { std::cout << prefix << " data: document.get_data()" << std::endl; } 
    else { std::cout << prefix << " data:" << std::endl; }
    std::cout << document.get_data() << std::endl; // contains url
   }
  }
 }

 return 0;
}

