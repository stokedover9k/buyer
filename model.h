#ifndef __MODEL_H__
#define __MODEL_H__

#include "model-defs.h"
#include "agent.h"
#include "ad-campaign.h"

#include <map>
#include <set>
#include <vector>
#include <math.h>
#include <cmath>

//-----------------------------------------------------------------------------

#define ITERATE_LIKE_STATES \
  std::vector<LikeState>::const_iterator like_itr \
  = Model::all_like_states.begin();		  \
  like_itr != Model::all_like_states.end(); \
  like_itr++

///////////////////////////////////////////////////////////////////////////////

class ModelData {
  
 public:
  //return the ID given to the agent
  BrandID add_brand(float price);
  AgentID add_agent(const Agent&);  //add new agent, and a campaign for it
  void add_ads(AgentID a, BrandID b, const AdSeq& s);

  Agent& get_agent(AgentID a);
  float  get_price(BrandID b);

 private:
  std::map<BrandID, float>      _brands;
  std::map<AgentID, Agent>      _agents;
  std::map<AgentID, AdCampaign> _ads;
};

///////////////////////////////////////////////////////////////////////////////

class Model : public ModelData {
  
 public:
  static std::vector<LikeState> all_like_states;
  //static std::vector<LikeState>::const_iterator like_itr;

  float p_like ( AgentID, BrandID, LikeState like );

  float m_FTFa ( AgentID, BrandID, LikeState like );

  float m_fit  ( AgentID, BrandID, LikeState like );
  float m_trust( AgentID, BrandID, LikeState like );
  float m_fash ( AgentID, BrandID, LikeState like );

    
 private:
  float _m_fit  ( const Agent&, BrandID, LikeState );
  float _m_trust( const Agent&, BrandID, LikeState );
  float _m_fash ( const Agent&, BrandID, LikeState );
  
};

#endif
