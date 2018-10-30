#include <cstdlib>
#include <cstring>
#include <iostream>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

void writetofile( std::string text, std::string name);
void xmlwritetofile( xmlChar *text, std::string name);
void htmlparser( htmlDocPtr doc, htmlNodePtr node);
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    std::cout << "not enough memory (realloc returned NULL)\n" <<std::endl;
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}