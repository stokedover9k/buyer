/* This file contains the headers for the following classes 
 * (in order of descendance):
 *   - ModelData
 *   - Model
 *   - TickModel
 *
 * Descriptions:
 * - ModelData
 *     A container for the main model elements with getters and setter.
 * - Model
 *     The model with formulas to find the probability of purchase and other
 *     necessary helper probabilities.
 * - TickerModel
 *     A model which can progress in time which holds the necessary
 *     probabilities for the current state and the previous state.
 */

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
#include <algorithm>

//-----------------------------------------------------------------------------

#define ITERATE_LIKE_STATES \
  std::vector<LikeState>::const_iterator like_itr \
  = Model::all_like_states.begin();		  \
  like_itr != Model::all_like_states.end(); \
  like_itr++
#define ITERATE_FASH_STATES \
  std::vector<FashState>::const_iterator fash_itr \
  = Model::all_fash_states.begin();		  \
  fash_itr != Model::all_fash_states.end(); \
  fash_itr++

///////////////////////////////////////////////////////////////////////////////

class ModelData {
  
 public:
  //return the ID given to the agent
  BrandID add_brand(float price);
  AgentID add_agent(const Agent&);  //add new agent, and a campaign for it
  void    add_ads(AgentID a, BrandID b, const AdSeq& s);

  void set_brand_price(BrandID, float price);

  Agent& get_agent(AgentID a);
  float  get_price(BrandID b);
  float  get_ad(AgentID, BrandID b, size_t time);
  
  AdSeq& get_ad_seq(AgentID, BrandID);

 protected:
  std::map<BrandID, float>      _brands;
  std::map<AgentID, Agent>      _agents;
  std::map<AgentID, AdCampaign> _ads;
};

///////////////////////////////////////////////////////////////////////////////

class Model : public ModelData {
  
 public:
  enum ModelMode { AVERAGE   = 1, 
		   ROUND     = 1<<1, 
		   MAX       = 1<<2, 
		   ROUND_AVG = AVERAGE|ROUND };

  static std::vector<LikeState> all_like_states;
  static std::vector<FashState> all_fash_states;

  // the expected value of purchase
  float v_purchase( AgentID, BrandID, ModelMode=AVERAGE );

  // the price-like evaluation method
  float m_price_like( AgentID, BrandID, float purchase, ModelMode=AVERAGE );

  // the probability that like is in the given state
  float p_like ( AgentID, BrandID, LikeState like );

  // the expected value of like (between 0 and max-state-val)
  float v_like ( AgentID, BrandID, LikeState& max_like_dest );

  float m_FTFa ( AgentID, BrandID, LikeState like );

  float m_fit  ( AgentID, BrandID, LikeState like );
  float m_trust( AgentID, BrandID, LikeState like );
  float m_fash ( AgentID, BrandID, LikeState like );

    
 private:
  float _m_price_like( const Agent&, BrandID, float purchase, ModelMode=AVERAGE);
  
  float _v_like ( const Agent&, BrandID, LikeState& max_like_dest );

  float _m_FTFa ( const Agent&, BrandID, LikeState );
  float _m_fit  ( const Agent&, BrandID, LikeState );
  float _m_trust( const Agent&, BrandID, LikeState );
  float _m_fash ( const Agent&, BrandID, LikeState );
};

///////////////////////////////////////////////////////////////////////////////

class TickerModel : public Model {

 public:
  enum TMparam { A_, AA };

  TickerModel(uint32_t t=0);
  ~TickerModel();

  void  add_social(AgentID, AgentID, float, TMparam);
  float get_social(AgentID, AgentID, TMparam);

  /* Modes: 
   *   ROUND_AVG:  round the expected value to the nearest whole FashState.
   *   AVERAGE:    truncate the expected value to a floor FashState
   *   MAX:        return the FashState with a single highest probability
   */
  void init_fashion_distrib(AgentID, BrandID);
  float update_agent_fash(AgentID, BrandID, Model::ModelMode=Model::AVERAGE);
  float calc_fashion_distrib(AgentID, BrandID, 
			     Model::ModelMode = Model::AVERAGE);

  float fash_state_prob(AgentID, BrandID, FashState) const;

  void tick(void);

 private:
  float _p_fash_adv(FashState, float ad_v, AgentID, BrandID);

  class FashDistrib {
  private:
    float _f[N_FASH_STATES];
  public:
    float& operator ()(FashState);
    float  operator ()(FashState) const;
  };

  uint32_t _t;
  std::map<std::pair<AgentID,AgentID>, std::pair<float,float> >  _social;
  std::map<std::pair<AgentID,BrandID>, FashDistrib> * _current_tastes;
  std::map<std::pair<AgentID,BrandID>, FashDistrib> * _prev_tastes;

};

#endif
