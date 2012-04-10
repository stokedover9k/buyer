#include "parser.h"

#include <iostream>

#include "model.h"

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

      vector<float> prices  = parser.getBrands(BuyerParser::PRICE);
      vector<float> trust   = parser.getBrands(BuyerParser::TRUST);
      vector<float> fit     = parser.getBrands(BuyerParser::FIT);
      vector<float> fash    = parser.getBrands(BuyerParser::FASH);
      vector<vector<float> > adv;
      parser.getAdCampaign(adv);

      Agent agent;
      ModelData m;
      vector<BrandID> brand_ids;

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

      AgentID a = m.add_agent(agent);

      for( int x = 0; x < prices.size(); x++ ) {
	BrandID brand = m.add_brand( prices[x] );

	m.get_agent(a).set( brand, Agent::TRUST, trust[x] );
	m.get_agent(a).set( brand, Agent::FIT,     fit[x] );
	m.get_agent(a).set( brand, Agent::FASH,   fash[x] );

	brand_ids.push_back(brand);
      }
      
      for( int x = 0; x < adv.size(); x++ )
	m.add_ads( a, brand_ids[x], AdSeq(adv[x]) );

    }
  catch( const char* error )
    {
      cout << "ERROR: " << error << "\nExiting (1)" << endl;
      std::exit(1);
    }
  return 0;
}
