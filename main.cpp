#include "parser.h"

#include <iostream>
#include <iomanip>

#include "model.h"

#define P_WIDTH 12

using std::cout;
using std::endl;
using std::flush;
using std::vector;



enum PrintHeader { FASH, PURCH, FASH_DISTRIB };

void print_header( PrintHeader, AgentID, FashState=HATE );
void print_prob( float );

void load_file(const char* filename,
	       vector<AgentID>&, vector<BrandID>&, TickerModel&, 
	       bool add_brands=false, bool load_social=false);



int main(int argc, char *argv[]) 
{
  if( argc < 2 ) {
    cout << "ERROR: no input file(s) specified.\n"
	 << "Usage: ./a.out master_file_1 [file_2 ...]\n" 
	 << "Exiting (1)" << endl;
    std::exit(1);
  }

  vector<AgentID> agent_ids;
  vector<BrandID> brand_ids;
  TickerModel m;

  try 
    {
      AgentID a;
      Agent agent;
      vector<vector<std::pair<float,float> > > interactions;

      for( int n_arg = 1; n_arg < argc; n_arg++ ) {
	cout << "# parsing file: " << argv[n_arg] << endl;
	load_file( argv[n_arg], agent_ids, brand_ids, m, n_arg==1, n_arg==argc-1);
      }
    }
  catch( const char* error )
    {
      cout << "ERROR loading model: " << error << "\nExiting (2)" << endl;
      std::exit(2);
    }

  //---------------------------------------------------------------------------

  try
    {
      for( int t=0; t < N_TIME_INTERVALS; t++ )
	{
	  m.tick();

	  for( int i=0; i < agent_ids.size(); i++ ) {
	    AgentID a = agent_ids[i];

	    print_header(FASH, a);
	    for( int x = 0; x < brand_ids.size(); x++ )
	      print_prob( m.update_agent_fash(a, brand_ids[x], Model::AVERAGE) );
	    cout << endl;

	    print_header(PURCH, a);
	    for( int x = 0; x < brand_ids.size(); x++ )
	      print_prob( m.v_purchase(a, brand_ids[x], Model::AVERAGE) );
	    cout << endl;

	    for( ITERATE_FASH_STATES ) {
	      print_header(FASH_DISTRIB, a, *fash_itr);
	      for( int x = 0; x < brand_ids.size(); x++ )
		print_prob( m.fash_state_prob(a, brand_ids[x], *fash_itr) );
	      cout << endl;
	    }
	  }
	}
    }
  catch( const char* error )
    {
      cout << "ERROR running model: " << error << "\nExiting (3)" << endl;
      std::exit(3);
    }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////


void load_file(const char* filename,
	       vector<AgentID>& agent_ids, vector<BrandID>& brand_ids, 
	       TickerModel& m, 
	       bool add_brands, bool load_social) {
  
  AgentID a;
  Agent agent;

  BuyerParser parser( filename );

  vector<float>    prices = parser.getBrands( BuyerParser::PRICE );
  vector<float>     trust = parser.getBrands( BuyerParser::TRUST );
  vector<float>       fit = parser.getBrands( BuyerParser::FIT   );
  vector<float>      fash = parser.getBrands( BuyerParser::FASH  );
  vector<vector<float> > adv;
  parser.getAdCampaign(adv);

  agent.set(Agent::BUDGET, parser.getBudget());
  agent.set(Agent::R_, parser.getModelParam( BuyerParser::rho    ));
  agent.set(Agent::RR, parser.getModelParam( BuyerParser::RHO    ));
  agent.set(Agent::L_, parser.getModelParam( BuyerParser::lambda ));
  agent.set(Agent::LL, parser.getModelParam( BuyerParser::LAMBDA ));
  agent.set(Agent::F_, parser.getModelParam( BuyerParser::phi    ));
  agent.set(Agent::FF, parser.getModelParam( BuyerParser::PHI    ));
  agent.set(Agent::T_, parser.getModelParam( BuyerParser::tau    ));
  agent.set(Agent::TT, parser.getModelParam( BuyerParser::TAU    ));
  agent.set(Agent::G_, parser.getModelParam( BuyerParser::gamma  ));
  agent.set(Agent::GG, parser.getModelParam( BuyerParser::GAMMA  ));
  agent.set(Agent::DD, parser.getModelParam( BuyerParser::DELTA  ));

  a = m.add_agent(agent);
  agent_ids.push_back(a);

  for( int x = 0; x < prices.size(); x++ ) {
    BrandID brand;
    if( add_brands ) {
      brand = m.add_brand( prices[x] );
      brand_ids.push_back(brand);
    } else {
      brand = brand_ids[x];
    }

    m.get_agent(a).state( brand ).trust =           trust[x];
    m.get_agent(a).state( brand ).fit   =             fit[x];
    m.get_agent(a).state( brand ).fash  = (FashState)fash[x];

    m.init_fashion_distrib(a, brand);
  }

  for( int x = 0; x < adv.size(); x++ )
    m.add_ads( a, brand_ids[x], AdSeq(adv[x]) );

  if( load_social ) {
    vector<vector<std::pair<float,float> > > interactions; 
    parser.getInteractions( interactions, agent_ids.size() );
    for( int i=0; i<agent_ids.size(); i++ )
      for( int j=0; j<agent_ids.size(); j++ ) {
	m.add_social(agent_ids[i], agent_ids[j], 
		     interactions[i][j].first, TickerModel::A_);
	m.add_social(agent_ids[i], agent_ids[j], 
		     interactions[i][j].second, TickerModel::AA);
      }
  }
}

void print_header( PrintHeader h, AgentID a, FashState f ) {
  const char* header;
  char detail[10] = "\0";

  switch( h )
    {
    case PURCH:        header = "PURCH";                                 break;
    case FASH:         header = "FASH";                                  break;
    case FASH_DISTRIB: header = "F_DIST";  sprintf(detail, ",f=%d", f);  break;
    }
  
  char output[15];
  sprintf( output, "%-6s (a=%d%s):", header, a, detail );
  printf( "%-20s", output );
}

void print_prob( float p ) {
  cout << std::setw(12)
       << std::setiosflags(std::ios::fixed) 
       << std::setiosflags(std::ios::showpoint) 
       << std::setprecision(8)
       << p;
}
