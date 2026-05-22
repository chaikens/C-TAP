#include "pugixml.hpp"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  char *filename=argv[1];
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if ( ! result )
    {
      cerr << "File " << filename << " failed to load by XML parser" << endl;
    }
  pugi::xml_node settings = doc.child("CamSett");
  //CamSett named node, just below document
  //each child has the form <setting NAME="VALUE"/>
  //for our 20 or so settings not including camera name.
  //Some VALUEs are int, some are float.
  for (pugi::xml_node onesetting : settings.children() )
    { for (pugi::xml_attribute att : onesetting.attributes() )
	{  cout << att.name() << "  " << att.value() << endl; }
    }
}
