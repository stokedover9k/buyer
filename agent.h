#ifndef __AGENT_H__
#define __AGENT_H__

#include "model-defs.h"

#include <map>

//-----------------------------------------------------------------------------

class Agent {

 public:

  enum stateField { TRUST, FIT, FASH, N_STATE_FIELDS };

  enum paramField  // notation: R_ => lower case rho, RR => upper case RHO
  { R_ = 0, RR, L_, LL, F_, FF, T_, TT, G_, GG, DD, A_, AA, BUDGET, 
    N_PARAM_FIELDS };

  float   get(                Agent::paramField param_name );
  State_t get( BrandID brand, Agent::stateField state_name );

  void    set(                Agent::paramField param_name, float value );
  void    set( BrandID brand, Agent::stateField state_name, State_t val );

 protected:
  float&   param(          Agent::paramField param_name );
  State_t  state( BrandID, Agent::stateField state_name ) const;
  State_t& state( BrandID, Agent::stateField state_name );
  
 private:
  struct States {
    State_t s[N_STATE_FIELDS];
  };

  float _params[N_PARAM_FIELDS];
  std::map<BrandID, States> _states;
  
};

#endif
