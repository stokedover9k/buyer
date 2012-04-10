#include "agent.h"

float Agent::get( Agent::paramField p )
{ return param(p); }

State_t Agent::get( BrandID b, Agent::stateField s )
{ return state(b, s); }

void Agent::set( Agent::paramField p, float v )
{ param(p) = v; }

void Agent::set( BrandID b, Agent::stateField s, State_t v )
{ state(b, s) = v; }

//-----------------------------------------------------------------------------

float& Agent::param( Agent::paramField p ) 
{
  if( p < 0 || p >= N_PARAM_FIELDS ) 
    throw "Agent::param(Agent::paramField): unexpected param requested.";
  return _params[p];
}

State_t Agent::state( BrandID b, Agent::stateField state_name ) const {
  if( state_name < 0 || state_name >= Agent::N_STATE_FIELDS )
    throw "Agent::state(BrandID, Agent::stateField): unexpected field requested";
  std::map<BrandID, States>::const_iterator s = _states.find(b);
  if( s == _states.end() )
    throw "Agent::state(BrandID, Agent::stateField): brand ID not found";
  return s->second.s[state_name];
}

State_t& Agent::state( BrandID b, Agent::stateField state_name ) {
  if( state_name < 0 || state_name >= Agent::N_STATE_FIELDS )
    throw "Agent::state(BrandID, Agent::stateField): unexpected field requested";
  return _states[b].s[state_name];
}
