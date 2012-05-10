#ifndef __AGENT_H__
#define __AGENT_H__

#include "model-defs.h"

#include <map>

//-----------------------------------------------------------------------------

class Agent {

 public:
  struct States {
    float like;
    float fit;
    float trust;
    float fash;
  };

  enum paramField  // notation: R_ => lower case rho, RR => upper case RHO
  { R_ = 0, RR, L_, LL, F_, FF, T_, TT, G_, GG, DD, BUDGET, 
    N_PARAM_FIELDS };

  float get( Agent::paramField param_name ) const ;
  void  set( Agent::paramField param_name, float value );

  States const& state( BrandID ) const;
  States      & state( BrandID );

 protected:
  float & param( Agent::paramField param_name );
  float   param( Agent::paramField param_name ) const;
  
 private:

  float _params[N_PARAM_FIELDS];
  std::map<BrandID, States> _states;
  
};

#endif
