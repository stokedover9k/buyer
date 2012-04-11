#include "parser.h"

#include <iostream>

#include "model.h"

using std::cout;
using std::endl;
using std::flush;
using std::vector;


int main(int argc, char *argv[]) 
{
  AgentID a;
  Agent agent;
  Model m;
  vector<BrandID> brand_ids;


  try
    {
      const char* filename = ( argc > 1 ) ? argv[1] : "in.csv";
      BuyerParser parser(filename);

      vector<float>    prices = parser.getBrands(BuyerParser::PRICE);
      vector<float>     trust = parser.getBrands(BuyerParser::TRUST);
      vector<float>       fit = parser.getBrands(BuyerParser::FIT);
      vector<float>      fash = parser.getBrands(BuyerParser::FASH);
      vector<vector<float> > adv;
      parser.getAdCampaign(adv);

      agent.set(Agent::BUDGET, parser.getBudget());
      agent.set(Agent::R_, parser.getModelParam(BuyerParser::rho));
      agent.set(Agent::RR, parser.getModelParam(BuyerParser::RHO));
      agent.set(Agent::L_, parser.getModelParam(BuyerParser::lambda));
      agent.set(Agent::LL, parser.getModelParam(BuyerParser::LAMBDA));
      agent.set(Agent::F_, parser.getModelParam(BuyerParser::phi));
      agent.set(Agent::FF, parser.getModelParam(BuyerParser::PHI));
      agent.set(Agent::T_, parser.getModelParam(BuyerParser::tau));
      agent.set(Agent::TT, parser.getModelParam(BuyerParser::TAU));
      agent.set(Agent::G_, parser.getModelParam(BuyerParser::gamma));
      agent.set(Agent::GG, parser.getModelParam(BuyerParser::GAMMA));
      agent.set(Agent::DD, parser.getModelParam(BuyerParser::DELTA));

      a = m.add_agent(agent);

      for( int x = 0; x < prices.size(); x++ ) {
	BrandID brand = m.add_brand( prices[x] );

	m.get_agent(a).state( brand ).trust =           trust[x];
	m.get_agent(a).state( brand ).fit   =             fit[x];
	m.get_agent(a).state( brand ).fash  = (FashState)fash[x];

	brand_ids.push_back(brand);
      }
      
      for( int x = 0; x < adv.size(); x++ )
	m.add_ads( a, brand_ids[x], AdSeq(adv[x]) );

    }
  catch( const char* error )
    {
      cout << "ERROR loading model: " << error << "\nExiting (1)" << endl;
      std::exit(1);
    }

  //---------------------------------------------------------------------------

  try
    {

      cout << "count: " << Model::all_like_states.size() << endl;

      for( ITERATE_LIKE_STATES )
	{
	  cout << "Like state: " << *like_itr << endl;

	  for( int x = 0; x < brand_ids.size(); x++ ) {
	    /*
	      cout << "FIT:   " << m.m_fit  (a, brand_ids[x], LOVE) << endl
	      << "TRUST: " << m.m_trust(a, brand_ids[x], LOVE) << endl
	      << "FASH:  " << m.m_trust(a, brand_ids[x], LOVE) << endl;
	    //*/
	    cout << "brand: " << x << " "
		 << m.p_like(a, brand_ids[x], *like_itr )
		 << endl;
	  }
	}

      /*
      for( ITERATE_LIKE_STATES ) {
	cout << "like: " << *Model::like_itr << endl;

	for( int x = 0; x < brand_ids.size(); x++ ) {
	  /*
	    cout << "FIT:   " << m.m_fit  (a, brand_ids[x], LOVE) << endl
	    << "TRUST: " << m.m_trust(a, brand_ids[x], LOVE) << endl
	    << "FASH:  " << m.m_trust(a, brand_ids[x], LOVE) << endl;
	  //*
	  cout << "brand: " << x << " "
	       << m.p_like(a, brand_ids[x], *Model::like_itr )
	       << endl;
	}
      }
*/
    }
  catch( const char* error )
    {
      cout << "ERROR running model: " << error << "Exiting (2)" << endl;
      std::exit(2);
    }

  return 0;
}
