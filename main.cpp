#include "stuff.h"
#include <curl/curl.h>
#include <fstream>

void flushchar( char *text, char toflush)
{
    for ( int i = 0; i < strlen( text); i++)
    {
        if( text[i] == toflush)
        {
            text[i] = ' ';
        }
    }
}

char* extract( xmlChar *xpath,  htmlNodePtr node, htmlDocPtr doc, xmlChar *prop)
{
    xmlXPathContextPtr context = xmlXPathNewContext( doc);
    xmlXPathObjectPtr object;

    object = xmlXPathNodeEval( node, xpath, context);
    if( xmlXPathNodeSetIsEmpty( object->nodesetval) )
    {
        std::cout << "XPath Returned NULL..." << std::endl;
        return NULL;
    }

    xmlChar *content;
    if ( prop != NULL)
    {
        content = xmlGetProp( object->nodesetval->nodeTab[0], prop );
    } else {
        content = xmlNodeGetContent( object->nodesetval->nodeTab[0] );
    }
    
    return (char *) content;
}

int main( int argc, char **argv)
{
    std::ofstream myfile;
    CURL *curl;
    CURLcode code;
    struct MemoryStruct output;
    xmlXPathObjectPtr result;
    xmlXPathContextPtr context; 
    
    output.memory = NULL;
    output.size = 0;

    curl_global_init( CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    curl_easy_setopt( curl, CURLOPT_URL, "https://www.newegg.com/global/uk/Video-Cards-Video-Devices/Category/ID-38");
    //curl_easy_setopt( curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &output);

    code = curl_easy_perform( curl);
    

    curl_easy_cleanup( curl);
    curl_global_cleanup();


    htmlDocPtr doc = htmlReadDoc( (xmlChar *) output.memory, NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    xmlNode *node;
    if (doc == NULL)
    {
        std::cout << "Failed to Parse File!" << std::endl;
        return -1;
    }
    node = xmlDocGetRootElement( doc);
    if (node == NULL)
    {
        std::cout << "Failed to get root element!" << std::endl;
        return -1;
    }

    xmlChar *xpath = (xmlChar *) "//*[contains(@class, \"item-container\")]";
    //std::cout << output.memory << std::endl;
    
    context = xmlXPathNewContext( doc);
    result = xmlXPathEvalExpression( xpath, context);

    if( xmlXPathNodeSetIsEmpty( result->nodesetval) )
    {
        xmlXPathFreeObject( result);
        std::cout << "Not Found. Check XPath." << std::endl;
        return 1;
    }
    
    myfile.open( "products.csv", std::ofstream::trunc | std::ofstream::out );
    myfile << "Brand,Description,Price,Rating\r";
    
    for ( int i = 0; i < result->nodesetval->nodeNr; i++)
    {
        
        char *text = extract( (xmlChar *) ".//div/div/a/img", result->nodesetval->nodeTab[i], doc, (xmlChar *) "title");
        if ( text == NULL)
        {
            std::cout << "Not Found.. " << std::endl;
            return -1;
        }
        flushchar( text, ',');
        myfile << text;
        myfile << ",";

        text = extract( (xmlChar *) ".//*[contains(@class, \"item-title\")]", result->nodesetval->nodeTab[i], doc, NULL);
        if ( text == NULL)
        {
            std::cout << "Not Found.. " << std::endl;
            return -1;
        }
        flushchar( text, ',');
        myfile << text;
        myfile << ",";

        text = extract( (xmlChar *) ".//*[contains(@class, \"price-current\")]/strong", result->nodesetval->nodeTab[i], doc, NULL);
        if ( text == NULL)
        {
            std::cout << "Not Found.. " << std::endl;
            return -1;
        }
        myfile << "£";
        myfile << text;
        //myfile << ".";

        text = extract( (xmlChar *) ".//*[contains(@class, \"price-current\")]/sup", result->nodesetval->nodeTab[i], doc, NULL);
        if ( text == NULL)
        {
            std::cout << "Not Found.. " << std::endl;
            return -1;
        }
        myfile << text;
        myfile << ",";

        text = extract( (xmlChar *) ".//*[contains(@class, \"item-rating-num\")]", result->nodesetval->nodeTab[i], doc, NULL);
        if ( text == NULL)
        {
            std::cout << "Not Found.. " << std::endl;
            return -1;
        }
        flushchar( text, '(');
        flushchar( text, ')');
        myfile << text;
        myfile << "\r";

    }
    myfile.close();

    return 0;
}



