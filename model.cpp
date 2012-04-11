#include "model.h"

LikeState _all_like_states[] = _FASH_STATES_BLOCK;
std::vector<LikeState> Model::all_like_states(_all_like_states, 
					      _all_like_states + 5);

///////////////////////////////////////////////////////////////////////////////

float Model::p_like( AgentID a_id, BrandID b_id, LikeState like ) {
  float req_val;
  float Z = 0;

  for( ITERATE_LIKE_STATES )
    {
      float p = exp(-m_FTFa( a_id, b_id, *like_itr ));
      if( *like_itr == like )
	req_val = p;
      Z += p;
    }
  return req_val / Z;
}

float Model::m_FTFa( AgentID a_id, BrandID b_id, LikeState like ) {
  Agent& a = get_agent(a_id);
  return 
    _m_fit  ( a, b_id, like ) +
    _m_trust( a, b_id, like ) +
    _m_fash ( a, b_id, like );
}

float Model::m_fit( AgentID a_id, BrandID b_id, LikeState like ) {
  return _m_fit( get_agent(a_id), b_id, like );
}

float Model::m_trust( AgentID a_id, BrandID b_id, LikeState like ) {
  return _m_trust( get_agent(a_id), b_id, like );
}

float Model::m_fash( AgentID a_id, BrandID b_id, LikeState like ) {
  return _m_fash( get_agent(a_id), b_id, like );
}

float Model::_m_fit( const Agent& a, BrandID b_id, LikeState like ) {
  float f = a.get(Agent::F_);
  float F = a.get(Agent::FF);
  return f * pow( std::abs(STATE_FACTOR*like - a.state(b_id).fit), F);
}

float Model::_m_trust( const Agent& a, BrandID b_id, LikeState like ) {
  float t = a.get(Agent::T_);
  float T = a.get(Agent::TT);
  return t * pow( std::abs(STATE_FACTOR*like - a.state(b_id).trust), T);
}

float Model::_m_fash( const Agent& a, BrandID b_id, LikeState like ) {
  float g = a.get(Agent::G_);
  float G = a.get(Agent::GG);
  return g * pow( std::abs(STATE_FACTOR*like - 
			   STATE_FACTOR*a.state(b_id).fash), G );
}


///////////////////////////////////////////////////////////////////////////////

AgentID ModelData::add_agent(const Agent& a) {
  AgentID new_key = (_agents.size() == 0) ? 0 : _agents.rbegin()->first + 1;
  _ads[new_key];                                 //allocate new campaign
  return _agents.insert(std::pair<AgentID, Agent>(new_key, a)).first->first;
}

BrandID ModelData::add_brand(float price) {
  BrandID new_id = ( _brands.size() == 0 ) ? 0 : _brands.rbegin()->first + 1;
  return _brands.insert(std::pair<BrandID,float>(new_id,price)).first->first;
}

void ModelData::add_ads(AgentID a, BrandID b, const AdSeq& adds) {
  std::map<AgentID, AdCampaign>::iterator i = _ads.find(a);
  if( i == _ads.end() )
    throw "ModelData::add_ads(AgentID, BrandID, const AdSeq&): no add sequence "
      "matching requested AgentID";
  _ads[a].get(b) = adds;
}



Agent& ModelData::get_agent(AgentID a) {
  std::map<AgentID, Agent>::iterator i = _agents.find(a);
  if( i == _agents.end() )
    throw "ModelData::get_agent(AgentID): no Agent matching requested AgentID";
  return i->second;
}

float ModelData::get_price(BrandID b) {
  std::map<BrandID, float>::iterator i = _brands.find(b);
  if( i == _brands.end() )
    throw "ModelData::get_price(BrandID): no Brand matching requested BrandID";
  return i->second;
}

///////////////////////////////////////////////////////////////////////////////

