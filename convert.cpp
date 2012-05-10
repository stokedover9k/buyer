#include "parser.h"

#include <iostream>
#include <iomanip>
#include <string.h>

#define P_WIDTH 12

using std::cout;
using std::endl;
using std::flush;
using std::vector;

#define INPUT_ARG_IND 5


int main( int argc, char *argv[] ) {
  int brand_count = 0;
  int time_count = 0;
  int agent_count = 0;

  try 
    {
      if( argc <= INPUT_ARG_IND ) 
	throw "Improper usage. Use: ./parse -b brand_count -t max_time FILE1 FILE2 ...";

      if( strcmp(argv[1], "-b") != 0 )           throw "No brand count provided.";
      if( (brand_count = atoi(argv[2])) <= 0 )   throw "Invalid brand count.";
      
      if( strcmp(argv[3], "-t") != 0 )           throw "No max time provided.";
      if( (time_count = atoi(argv[4])) <= 0 )    throw "Invalid max time value.";
      
      //-----------
      // add brands
      //-----------
      BuyerParser parser_1( argv[INPUT_ARG_IND] );
      const std::vector<float>& prices = parser_1.getBrands( BuyerParser::PRICE );
      if( prices.size() != brand_count )
	throw "Brand count doesn't match the number of extracted brands.";
      std::vector<std::string> brand_names( brand_count );
      
      for( int b=0; b<brand_count; b++ ) {
	brand_names[b] = parser_1.getBrandName(b);
	cout << "add brand " << brand_names[b] << endl;
	cout << "    brand " << brand_names[b] << " price " << prices[b] << endl;
      }

      //-----------
      // add agents
      //-----------
      for( int argn = INPUT_ARG_IND; argn < argc; argn++ ) {
	agent_count++;

	BuyerParser parser = BuyerParser(argv[argn]);

	cout << "add agent " << agent_count << endl;

	cout << "    agent " << agent_count << " param BUDGET " << parser.getBudget()                          << endl;
	cout << "    agent " << agent_count << " param R_ "     << parser.getModelParam( BuyerParser::rho    ) << endl;
	cout << "    agent " << agent_count << " param RR "     << parser.getModelParam( BuyerParser::RHO    ) << endl;
	cout << "    agent " << agent_count << " param L_ "     << parser.getModelParam( BuyerParser::lambda ) << endl;
	cout << "    agent " << agent_count << " param LL "     << parser.getModelParam( BuyerParser::LAMBDA ) << endl;
	cout << "    agent " << agent_count << " param F_ "     << parser.getModelParam( BuyerParser::phi    ) << endl;
	cout << "    agent " << agent_count << " param FF "     << parser.getModelParam( BuyerParser::PHI    ) << endl;
	cout << "    agent " << agent_count << " param T_ "     << parser.getModelParam( BuyerParser::tau    ) << endl;
	cout << "    agent " << agent_count << " param TT "     << parser.getModelParam( BuyerParser::TAU    ) << endl;
	cout << "    agent " << agent_count << " param G_ "     << parser.getModelParam( BuyerParser::gamma  ) << endl;
	cout << "    agent " << agent_count << " param GG "     << parser.getModelParam( BuyerParser::GAMMA  ) << endl;
	cout << "    agent " << agent_count << " param DD "     << parser.getModelParam( BuyerParser::DELTA  ) << endl;

	//---------------------------
	// get agent's initial states
	//---------------------------
	{
	  const vector<float>& trust = parser.getBrands( BuyerParser::TRUST );
	  const vector<float>&   fit = parser.getBrands( BuyerParser::FIT   );
	  const vector<float>&  fash = parser.getBrands( BuyerParser::FASH  );

	  for( int b=0; b<brand_count; b++ )  cout << "    agent " << agent_count << " trust brand " << brand_names[b] << " " << trust[b] << endl;
	  for( int b=0; b<brand_count; b++ )  cout << "    agent " << agent_count << " fit   brand " << brand_names[b] << " " <<   fit[b] << endl;
	  for( int b=0; b<brand_count; b++ )  cout << "    agent " << agent_count << " fash  brand " << brand_names[b] << " " <<  fash[b] << endl;
	}

	//------------------------------
	// get ad campaign for the agent
	//------------------------------
	{
	  vector<vector<float> > ads;
	  parser.getAdCampaign(ads);
	  
	  for( int b=0; b<brand_count; b++ ) {
	    for( int t=0; t<time_count; t++ ) {
	      cout << "add ads agent " << agent_count << " brand " << brand_names[b] << " " << ads[b][t] << endl;
	    }
	  }
	}
	
      }  // end agent iteration loop

      //----------------
      // add social ties
      //----------------
      {
	vector<vector<std::pair<float,float> > > social;
	parser_1.getInteractions( social, agent_count );
	for( int i=0; i<agent_count; i++ ) 
	  {
	    for( int j=0; j<agent_count; j++ ) 
	      {
		cout << "social strong " << i+1 << " " << j+1 << " A_ " << social[i][j].first << endl;
		cout << "social strong " << i+1 << " " << j+1 << " AA " << social[i][j].second << endl;
	      }
	  }
      }
    }
  catch (const char* e)
    {
      cout << "ERROR: " << e << endl;
      exit(1);
    }

  return 0;
}
