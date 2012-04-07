#include "parser.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;
using std::vector;


int main(int argc, char *argv[]) 
{
  try
    {
      const char* filename = ( argc > 1 ) ? argv[1] : "in.csv";
      BuyerParser parser(filename);

      cout << "Budget: " << parser.getBudget() << endl;

      cout << "Prices..." << endl;
      vector<float> prices = parser.getBrands(BuyerParser::PRICE);
      for( vector<float>::iterator i = prices.begin(); i != prices.end(); i++ )
	cout << "  " << *i << endl;
      
      cout << "Trust..." << endl;
      vector<float> trust = parser.getBrands(BuyerParser::TRUST);
      for( vector<float>::iterator i = trust.begin(); i != trust.end(); i++ )
	cout << "  " << *i << endl;

      cout << "Fit..." << endl;
      vector<float> fit = parser.getBrands(BuyerParser::FIT);
      for( vector<float>::iterator i = fit.begin(); i != fit.end(); i++ )
	cout << "  " << *i << endl;
    }
  catch( const char* error )
    {
      cout << "ERROR: " << error << "\nExiting (1)" << endl;
      std::exit(1);
    }
  return 0;
}
