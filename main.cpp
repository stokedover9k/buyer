#include "parser.h"

#include <iostream>
#include <iomanip>
#include <string.h>

#include "model.h"

#define P_WIDTH 12
#define TOKEN_MAX_SIZE 16
#define LINE_MAX_SIZE 256

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


std::istream& read_token(std::istream& is, char* buff, char delim = ' ') {
  while( is.peek() == ' ' || is.peek() == '\n' )  is.get();
  is.getline( buff, TOKEN_MAX_SIZE, delim );
  return is;
}



int main( int argc, char *argv[] )
{
  //--------------------
  // set up input stream
  //--------------------
  std::istream* in = ( argc > 2 && strcmp(argv[1], "-f") == 0 ) 
    ? new std::ifstream(argv[2]) : &std::cin;
  std::istream& infile = *in;

  if( !infile ) {
    std::cerr << "ERROR: Could not open input stream.\n"
	      << "Exiting (1)" << endl;
    std::exit(1);
  }

  //-----------------------------
  // parse input and create model
  //-----------------------------
  std::map<std::string, AgentID> agent_ids;
  std::map<std::string, BrandID> brand_ids;
  std::map<std::string, AgentID> weak_ids;
  TickerModel m;

  try
    {
      while( infile ) 
	{
	  while( infile.peek() == ' ' || infile.peek() == '\n' )   infile.get();
	  while( infile.peek() == '#' )   infile.ignore(LINE_MAX_SIZE, '\n');
	  if( !infile ) continue;

	  char buff[TOKEN_MAX_SIZE];
	  if( !read_token( infile, buff ) )
	    throw "Error occurred while reading the input stream.";

	  // add to model
	  if( 0 == strcmp( buff, "add" ) ) {

	    if( !read_token( infile, buff ) )
	      throw "Error while reading an addition to the model.";
	    
	    if( 0 == strcmp( buff, "brand" ) ) {
	      read_token( infile, buff, '\n' );
	      brand_ids[ std::string(buff) ] = m.add_brand(0);;
	    }

	    else if( 0 == strcmp( buff, "agent" ) ) {
	      read_token( infile, buff, '\n' );
	      agent_ids[ std::string(buff) ] = m.add_agent(Agent());
	    }

	    else if( 0 == strcmp( buff, "ads" ) ) {
	      read_token( infile, buff );
	      if( 0 != strcmp( buff, "agent" ) )
		throw "Unexpected token while reading ad-campaign.";
	      read_token( infile, buff );
	      AgentID aid = agent_ids[ std::string(buff) ];

	      read_token( infile, buff );
	      if( 0 != strcmp( buff, "brand" ) )
		throw "Unexpected token while reading ad-campaign.";
	      read_token( infile, buff );
	      BrandID bid = brand_ids[ std::string(buff) ];

	      read_token( infile, buff, '\n' );
	      if( !infile) throw "Unexpected token while reading ad-campaign.";
	      m.get_ad_seq(aid, bid).add( atof(buff) );
	    }

	    else if( 0 == strcmp( buff, "weak_tie" ) ) {
	      read_token( infile, buff, '\n' );
	      weak_ids[ std::string(buff) ] = m.add_weak();
	    }

	    else
	      throw "Invalid add requested.";
	    
	  } // end add to model

	  // social ties
	  else if( 0 == strcmp( buff, "social" ) ) {
	    read_token( infile, buff );
	    
	    if( 0 == strcmp(buff, "strong") ) {
	      read_token( infile, buff );
	      AgentID a1 = agent_ids[ std::string(buff) ];
	      read_token( infile, buff );
	      AgentID a2 = agent_ids[ std::string(buff) ];

	      read_token( infile, buff );
	      if( 0 == strcmp(buff, "A_") ) {
		read_token( infile, buff, '\n' );
		m.add_social(a1, a2, atof(buff), TickerModel::A_);
	      }
	      else if( 0 == strcmp(buff, "AA") ) {
		read_token( infile, buff, '\n' );
		m.add_social(a1, a2, atof(buff), TickerModel::AA);
	      }
	      else
		throw "Invalid parameter for strong social bonds.";
	    }

	    else if( 0 == strcmp(buff, "weak") ) {  // implement
	      read_token( infile, buff );
	      AgentID a = agent_ids[ std::string(buff) ];
	      read_token( infile, buff );
	      AgentID w = weak_ids[ std::string(buff) ];

	      read_token( infile, buff );
	      if( 0 == strcmp(buff, "B_") ) {
		read_token( infile, buff, '\n' );
		m.add_weak_social(a, w, atof(buff), TickerModel::B_);
	      }
	      else if( 0 == strcmp(buff, "BB") ) {
		read_token( infile, buff, '\n' );
		m.add_weak_social(a, w, atof(buff), TickerModel::BB);
	      }
	      else
		throw "Invalid parameter for weak social bonds.";

	      infile.ignore(LINE_MAX_SIZE, '\n');
	    }

	    else 
	      throw "Unknown type of social bond.";

	    infile.ignore(LINE_MAX_SIZE, '\n');
	  } // end social ties

	  // agent parameters
	  else if( 0 == strcmp( buff, "agent" ) ) {
	    read_token( infile, buff );
	    AgentID aid = agent_ids[ std::string(buff) ];
	    
	    char param_type[TOKEN_MAX_SIZE];
	    read_token( infile, param_type );

	    if( 0 == strcmp( param_type, "param" ) ) {
	      Agent::paramField p_field;
	      read_token( infile, buff );
	      
	      if     ( 0 == strcmp(buff, "BUDGET") ) p_field = Agent::BUDGET;
	      else if( 0 == strcmp(buff,     "R_") ) p_field = Agent::R_;
	      else if( 0 == strcmp(buff,     "RR") ) p_field = Agent::RR;
	      else if( 0 == strcmp(buff,     "L_") ) p_field = Agent::L_;
	      else if( 0 == strcmp(buff,     "LL") ) p_field = Agent::LL;
	      else if( 0 == strcmp(buff,     "F_") ) p_field = Agent::F_;
	      else if( 0 == strcmp(buff,     "FF") ) p_field = Agent::FF;
	      else if( 0 == strcmp(buff,     "T_") ) p_field = Agent::T_;
	      else if( 0 == strcmp(buff,     "TT") ) p_field = Agent::TT;
	      else if( 0 == strcmp(buff,     "G_") ) p_field = Agent::G_;
	      else if( 0 == strcmp(buff,     "GG") ) p_field = Agent::GG;
	      else if( 0 == strcmp(buff,     "DD") ) p_field = Agent::DD;
	      else
		throw "Invalid Agent param.";

	      read_token( infile, buff, '\n' );
	      m.get_agent(aid).set(p_field, atof(buff));
	    }
	    else {
	      read_token( infile, buff );
	      if( 0 != strcmp(buff, "brand") ) {
		cout << buff << " - " << param_type << endl;
		throw "Agent characteristic's brand not found.";
	      }

	      read_token( infile, buff );
	      Agent::States& state =
		m.get_agent(aid).state( brand_ids[ std::string(buff) ] );

	      read_token( infile, buff, '\n' );
	      float state_val = atof(buff);

	      if( 0 == strcmp( param_type, "trust" ) )
		state.trust = state_val;
	      else if( 0 == strcmp( param_type, "fit" ) )
		state.fit = state_val;
	      else if( 0 == strcmp( param_type, "fash" ) )
		state.fash = state_val;
	      else {
		cout << buff << endl;
		throw "Unexpected agent characteristic.";
	      }
	    }

	  } // end social parameters

	  else if( 0 == strcmp( buff, "weak_tie" ) ) {
	    read_token( infile, buff );
	    AgentID weak_id = weak_ids[ std::string(buff) ];
	    
	    read_token( infile, buff );
	    if( 0 == strcmp( buff, "brand" ) ) {
	      read_token( infile, buff );
	      BrandID bid = brand_ids[ std::string(buff) ];

	      read_token( infile, buff, '\n' );
	      m.add_weak_pref( weak_id, bid, (FashState)std::atoi(buff) );
	    }
	    else
	      throw "Brand expected for weak tie preference.";

	    //infile.ignore(LINE_MAX_SIZE, '\n');
	  }

	  // brand parameters
	  else if( 0 == strcmp( buff, "brand" ) ) {
	    infile.getline( buff, TOKEN_MAX_SIZE, ' ' );
	    BrandID bid = brand_ids[ std::string(buff) ];
	    infile.getline( buff, TOKEN_MAX_SIZE, ' ' );

	    if( 0 == strcmp( buff, "price" ) ) {
	      infile.getline( buff, TOKEN_MAX_SIZE, '\n' );
	      m.set_brand_price( bid, atof(buff) );
	    }
	    else
	      throw "Unexpected brand parameter.";
	  } // end brand parameters

	  else
	    throw "Unknown action at start of line.";
	  
	} // end parse loop

      //---------------------------------
      // initialize fashion distributions
      //---------------------------------
      for( std::map<std::string, BrandID>::const_iterator b_itr = brand_ids.begin(); b_itr != brand_ids.end(); b_itr++ ) 
	for( std::map<std::string, AgentID>::const_iterator a_itr = agent_ids.begin(); a_itr != agent_ids.end(); a_itr++ ) 
	  m.init_fashion_distrib( a_itr->second, b_itr->second );
    }
  catch ( const char* e )
    {
      std::cerr << "ERROR (while parsing and model creation): " << e
		<< "\nExitin (1)..." << endl;
      std::exit(1);
    }

  //--------------
  // run the model
  //--------------
  try
    {
      for( int t=0; t < N_TIME_INTERVALS; t++ )
	{
	  m.tick();

	  for( std::map<std::string, AgentID>::const_iterator a_itr = agent_ids.begin(); a_itr != agent_ids.end(); a_itr++ ) {
	    AgentID a = a_itr->second;

	    print_header(FASH, a);
	    for( std::map<std::string, BrandID>::const_iterator b_itr = brand_ids.begin(); b_itr != brand_ids.end(); b_itr++ ) 
	      print_prob( m.update_agent_fash(a, b_itr->second, Model::AVERAGE) );
	    cout << endl;

	    print_header(PURCH, a);
	    for( std::map<std::string, BrandID>::const_iterator b_itr = brand_ids.begin(); b_itr != brand_ids.end(); b_itr++ ) 
	      print_prob( m.v_purchase(a, b_itr->second, Model::AVERAGE) );
	    cout << endl;

	    for( ITERATE_FASH_STATES ) {
	      print_header(FASH_DISTRIB, a, *fash_itr);
	      for( std::map<std::string, BrandID>::const_iterator b_itr = brand_ids.begin(); b_itr != brand_ids.end(); b_itr++ ) 
		print_prob( m.fash_state_prob(a, b_itr->second, *fash_itr) );
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


int main1(int argc, char *argv[]) 
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
	load_file( argv[n_arg], agent_ids, brand_ids, m, n_arg==1, false);
      }
      
      cout << "# parsing file for social: " << argv[1] << endl;
      load_file( argv[1], agent_ids, brand_ids, m, false, true );
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

  BuyerParser parser( filename );

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
    return;
  }

  AgentID a;
  Agent agent;

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
