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

      cout << "Fash (initial)..." << endl;
      vector<float> fash = parser.getBrands(BuyerParser::FASH);
      for( vector<float>::iterator i = fash.begin(); i != fash.end(); i++ )
	cout << "  " << *i << endl;

      cout << "Advertisements..." << endl;
      vector<vector<float> > adv;
      parser.getAdCampaign(adv);
      int count = 0;
      for( vector<vector<float> >::iterator i = adv.begin(); i != adv.end(); i++ ) {
	for( vector<float>::iterator j = (*i).begin(); j != (*i).end(); j++ ) {
	  cout << *j << " ";
	}
	cout << endl;
      }

      cout << parser.getModelParam(BuyerParser::rho) << endl;
      cout << parser.getModelParam(BuyerParser::RHO) << endl;
      cout << parser.getModelParam(BuyerParser::lambda) << endl;
      cout << parser.getModelParam(BuyerParser::LAMBDA) << endl;
      cout << parser.getModelParam(BuyerParser::phi) << endl;
      cout << parser.getModelParam(BuyerParser::PHI) << endl;
      cout << parser.getModelParam(BuyerParser::tau) << endl;
      cout << parser.getModelParam(BuyerParser::TAU) << endl;
      cout << parser.getModelParam(BuyerParser::gamma) << endl;
      cout << parser.getModelParam(BuyerParser::GAMMA) << endl;
      cout << parser.getModelParam(BuyerParser::DELTA) << endl;
    }
  catch( const char* error )
    {
      cout << "ERROR: " << error << "\nExiting (1)" << endl;
      std::exit(1);
    }
  return 0;
}
