#include "agent.h"

float Agent::get( Agent::paramField p ) const
{ return param(p); }
void Agent::set( Agent::paramField p, float v )
{ param(p) = v; }

Agent::States const& Agent::state( BrandID b ) const {
  std::map<BrandID, States>::const_iterator i = _states.find(b);
  if( i == _states.end() )
    throw "Agent::state(BrandID) const: no brand matching BrandID";
  return i->second;
}

Agent::States& Agent::state( BrandID b ) {
  std::map<BrandID, States>::iterator i = _states.find(b);
  if( i == _states.end() )
    i = _states.insert(std::pair<BrandID, States>(b, States())).first;
  return i->second;
}

//-----------------------------------------------------------------------------

float& Agent::param( Agent::paramField p ) 
{
  if( p < 0 || p >= N_PARAM_FIELDS ) 
    throw "Agent::param(Agent::paramField): unexpected param requested.";
  return _params[p];
}

float Agent::param( Agent::paramField p ) const
{
  if( p < 0 || p >= N_PARAM_FIELDS ) 
    throw "Agent::param(Agent::paramField): unexpected param requested.";
  return _params[p];
}

