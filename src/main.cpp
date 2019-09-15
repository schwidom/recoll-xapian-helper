
#include <xapian.h>

#include <iostream>
#include <vector>
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

 static std::vector<struct option> long_options_vector 
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
  {"prefix",  required_argument, 0,  'p' },
  {"debug",    no_argument, 0,  'z' },
  {0,         0,                 0,  0 }
 };

 {
  int c;
  int digit_optind = 0;

  while (1) {

   int this_option_optind = optind ? optind : 1;
   int option_index = 0;

   // static struct option long_options[] =

   c = getopt_long(argc, argv, "havo:c1lwi2dp:z", long_options_vector.data(), &option_index);

   if (c == -1)
       break;

   auto found = std::find_if( long_options_vector.begin(), long_options_vector.end(), 
    [c]( struct option& o) -> bool { return o.val == c;});

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
       std::cout << "parameter needed: path of xapian database location (directory) " << c << std::endl;
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
 if( chkoption( "author") || chkoption( "help") || chkoption( "version"))
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
    std::cout << "-" << static_cast<char>(o.val) << ", --" << o.name << std::endl;   
   }
  }

  std::exit( 0);
 }

 if( "" == dblocation)
 {
  std::cout << "dblocation must be given" << std::endl;
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
