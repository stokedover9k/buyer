#include "model.h"

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
