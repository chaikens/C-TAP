#include "pugixml.hpp"
#include <string.h>
#include <iostream>
#include <error.h>
using namespace std;
using namespace pugi;



/* notes 

(1) We could skip comments, document node, and whitespace in plain text elts
by setting flags during parsing, see

https://pugixml.org/docs/manual.html#loading.options

(2) Missing items (name, value, etc) are documented in
https://pugixml.org/docs/manual.html#access.nodedata

(3) Parse plain text nodes (for our setting values) as kinds of numbers:
https://pugixml.org/docs/manual.html#access.text
*/

void ind( int l )
{
  for (int i = 0; i < l; i++) cout << (i+1);
}


void traverse(xml_node n, int l)
{
  ind(l); cout << "Node type=" << '"' << n.type() << '"' << endl;
  ind(l); cout << "Node name=" << '"' << n.name() << '"' << endl;
  ind(l); cout << "Node valu=" << '"' << n.value() << '"' << endl;
  ind(l); cout << "Begin attribute loop" << endl;
  for ( xml_attribute at = n.first_attribute(); at; at = at.next_attribute() )
    {
      ind(l);
      cout << "Attr name=" << at.name() << " val=" << '"' << at.value() << '"' << endl;
    }
  ind(l); cout << "End attr loop." << endl;
  ind(l); cout << "Begin children loop (will recurse)." << endl;
  for ( xml_node nc = n.first_child(); nc; nc = nc.next_sibling())
    {
      traverse( nc, l+1);
    }
  ind(l); cout << "End children loop (recursion done)." << endl;
}



int main(int argc, char *argv[])
{
  xml_document doc;
  if(!argv[1]) { error(1, 0, "Must give something.xml argument."); }
  cout << "node_element=" << node_element << endl;
  xml_parse_result pres = doc.load_file(argv[1]);
  cout << "Load status=" << pres.description() << endl;
  xml_node top = doc.first_child();
  //traverse( top, 0);
  traverse( doc, 0);
  return 0;
}
  
