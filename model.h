#ifndef __MODEL_H__
#define __MODEL_H__

#include "model-defs.h"
#include "agent.h"
#include "ad-campaign.h"

#include <map>
#include <set>
#include <vector>

class ModelData;

///////////////////////////////////////////////////////////////////////////////

class Model;

///////////////////////////////////////////////////////////////////////////////

class ModelData {
  
 public:
  //return the ID given to the agent
  BrandID add_brand(float price);
  AgentID add_agent(const Agent&);  //add new agent, and a campaign for it
  void add_ads(AgentID a, BrandID b, const AdSeq& s);

  Agent& get_agent(AgentID a);

 private:
  std::map<BrandID, float>      _brands;
  std::map<AgentID, Agent>      _agents;
  std::map<AgentID, AdCampaign> _ads;
};

#endif
