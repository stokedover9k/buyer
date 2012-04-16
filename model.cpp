#include "model.h"

LikeState _all_like_states[] = _FASH_STATES_BLOCK;
std::vector<LikeState> Model::all_like_states(_all_like_states, 
					      _all_like_states + 5);
std::vector<FashState> Model::all_fash_states(_all_like_states, 
					      _all_like_states + 5);

///////////////////////////////////////////////////////////////////////////////

TickerModel::TickerModel(uint32_t t) : _t(t)
{
  _current_tastes = new std::map<std::pair<AgentID,BrandID>, 
    TickerModel::FashDistrib>();
  _prev_tastes    = new std::map<std::pair<AgentID,BrandID>, 
    TickerModel::FashDistrib>();
}

TickerModel::~TickerModel()
{ 
  delete _current_tastes;
  delete _prev_tastes;
}

float TickerModel::update_agent_fash(AgentID a, BrandID b, 
					 Model::ModelMode mode) {
  float E_fash = calc_fashion_distrib(a, b, mode);
  get_agent( a ).state( b ).fash = E_fash;
  return E_fash;
}

float TickerModel::calc_fashion_distrib(AgentID a, BrandID b, 
					    Model::ModelMode avg_mode) {
  if( avg_mode != Model::MAX && avg_mode != Model::AVERAGE 
      && avg_mode != Model::ROUND_AVG )
    throw "TickerModel::calc_fashion_distrib(AgentID, BrandID, bool): "
      "invalid mode for calculating the expected FashState requested.";

  FashDistrib& d = (*_current_tastes)[std::pair<AgentID, BrandID>(a,b)];
  float E_fash = 0.0;
  FashState MAX_fash_s;

  const FashDistrib& p = (*_prev_tastes)[std::pair<AgentID,BrandID>(a,b)];
  float Z_fash = 0.0;
  for( ITERATE_FASH_STATES )
    {
      float ad_v = get_ad(a, b, _t-1);
      float prob = _p_fash_adv(*fash_itr, ad_v, a, b);
      d(*fash_itr) = prob;
      Z_fash += prob;
    }
  for( ITERATE_FASH_STATES ) {  //normalize the distribution, find average or max
    d(*fash_itr) /= Z_fash;

    if( avg_mode & Model::MAX ) {
      if( E_fash < d(*fash_itr) ) 
	{ E_fash = d(*fash_itr); MAX_fash_s = *fash_itr; }
    }
    else if( avg_mode & Model::AVERAGE ) {
      E_fash += d(*fash_itr) * (*fash_itr);
    }
  }
  
  switch( avg_mode )
    {
    case Model::AVERAGE:               return E_fash;
    case Model::ROUND_AVG:             return 
	(E_fash-(int)E_fash < 0.5) ? std::floor(E_fash) : std::ceil(E_fash);
    case Model::MAX:                   return (float)MAX_fash_s;
    default: throw "TickerModel::calc_fashion_distrib: Should never come here";
    };
}

void TickerModel::init_fashion_distrib(AgentID a, BrandID b) {
  FashDistrib& d = (*_current_tastes)[std::pair<AgentID, BrandID>(a,b)];
  float init_fashion = get_agent( a ).state( b ).fash;
  for( ITERATE_FASH_STATES ) {
    d(*fash_itr) = *fash_itr == init_fashion ? 1.0 : 0.0;
  }
}

float TickerModel::fash_state_prob(AgentID a, BrandID b, FashState f) const {
  return (*_current_tastes)[std::pair<AgentID, BrandID>(a,b)](f);
}

float TickerModel::_p_fash_adv(FashState fash, float ad_v, AgentID a, BrandID b) {
  const Agent& agent = get_agent(a);
  FashState fash_prev;
  const FashDistrib& dist_prev = (*_prev_tastes)[std::pair<AgentID,BrandID>(a,b)];

  float sum_total = 0;
  for( ITERATE_FASH_STATES ) {
    fash_prev = *fash_itr;
    
    float exponent = 
      ad_v * std::pow( std::abs(fash - std::min<int>(fash_prev+1, (int)LOVE)),
		       agent.get( Agent::DD ) )
      + get_social(a, a, TickerModel::A_) 
      * std::pow( std::abs(fash - fash_prev), 
		  get_social(a, a, TickerModel::AA) );
    
    sum_total += std::exp(-exponent) * dist_prev(fash_prev);
  }

  //std::cout << "sum: " << sum_total << std::endl;

  float product_total = 1.0;

  for( std::map<AgentID, Agent>::const_iterator a_itr = _agents.begin();
       a_itr != _agents.end(); a_itr++ )
    {
      if( a_itr->first == a ) continue;
      
      float j_sum = 0;
      for( ITERATE_FASH_STATES ) {
	FashState j_fash_prev = *fash_itr;

	j_sum += 
	  std::exp(- get_social(a, a_itr->first, TickerModel::A_)
		   * std::pow( std::abs( fash - j_fash_prev ),
			       get_social(a, a_itr->first, TickerModel::AA)))
	  * (*_prev_tastes)[std::pair<AgentID,BrandID>
			    (a_itr->first, b)](j_fash_prev);
      }

      product_total *= j_sum;
    } //*/
  return sum_total * product_total;
}

void TickerModel::tick(void) {
  _t++;
  std::map<std::pair<AgentID,BrandID>, FashDistrib> *tmp;
  tmp = _current_tastes;
  _current_tastes = _prev_tastes;
  _prev_tastes = tmp;
}

void TickerModel::add_social(AgentID i, AgentID j, float val, 
			     TickerModel::TMparam param) {
  std::pair<AgentID,AgentID> key(i, j);
  std::pair<float, float> p = _social.find(key)->second;

  switch( param )
    {
    case TickerModel::A_:   p.first  = val;  break;
    case TickerModel::AA:   p.second = val;  break;
    default:
      throw "TickerModel::add_social(AgentID, AgentID, float, "
	"TickerModel::TMparam): unexpected parameter requested.";
    }

  _social[key] = p;
}

float TickerModel::get_social(AgentID i, AgentID j, TickerModel::TMparam param) {
  std::pair<AgentID,AgentID> key(i, j);
  std::map<std::pair<AgentID,AgentID>, std::pair<float,float> >
    ::const_iterator itr = _social.find(key);
  if( itr == _social.end() )
    throw "TickerModel::get_social(AgentID, AgentID, TickerModel::TMparam): "
      "requested AgentID pair not found.";

  switch( param )
    {
    case TickerModel::A_:  return itr->second.first;
    case TickerModel::AA:  return itr->second.second;
    default:
      throw "TickerModel::get_social(AgentID, AgentID, TickerModel::TMparam): "
	"unexpected parameter requested.";
    };
}

//-----------------------------------------------------------------------------

float& TickerModel::FashDistrib::operator ()(FashState n)
{ return _f[n]; }

float  TickerModel::FashDistrib::operator ()(FashState n) const 
{ return _f[n]; }

///////////////////////////////////////////////////////////////////////////////

float Model::v_purchase( AgentID a_id, BrandID b_id, Model::ModelMode mode ) {
  return 1 / (1 + std::exp( m_price_like(a_id, b_id, 1.0, mode) ));
}

float Model::m_price_like( AgentID a_id, BrandID b_id, float purchase,
			   Model::ModelMode mode ) {
  return _m_price_like( get_agent(a_id), b_id, purchase, mode );
}

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

float Model::v_like( AgentID a_id, BrandID b_id, LikeState& max_like_dest ) {
  return _v_like( get_agent(a_id), b_id, max_like_dest );
}

float Model::_v_like( const Agent& a, BrandID b_id, LikeState& max_like_dest ) {
  float val = 0;
  float Z = 0;
  float max_p = 0;

  for( ITERATE_LIKE_STATES )
    {
      float p = exp(-_m_FTFa( a, b_id, *like_itr ));
      val += p * (*like_itr);
      Z   += p;
      if( p > max_p ) { max_p = p;  max_like_dest = *like_itr; }
    }
  return val / Z;
}

float Model::m_FTFa( AgentID a_id, BrandID b_id, LikeState like ) {
  return _m_FTFa( get_agent(a_id), b_id, like );
}

float Model::_m_FTFa( const Agent& a, BrandID b_id, LikeState like ) {
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

float Model::_m_price_like( const Agent& a, BrandID b_id, float purchase,
			    Model::ModelMode expected_like ) {
  float pr   = get_price(b_id);
  float bu   = a.get( Agent::BUDGET );
  float r    = a.get( Agent::R_ );
  float R    = a.get( Agent::RR );
  float l    = a.get( Agent::L_ );
  float L    = a.get( Agent::LL );
  float like; //= a.state( b_id ).like;

  LikeState max_like;
  like = _v_like( a, b_id, max_like );

  switch( expected_like )
    {
    case Model::AVERAGE:                                        break;
    case Model::AVERAGE|Model::ROUND:  
      like = (like-(int)like < 0.5) ? 
	std::floor(like) : std::ceil(like);                     break;
    case Model::MAX:
      like = (float)max_like;                                   break;
    default: 
      throw "Model::_m_price_like(const Agent&, BrandID, float,"
	" Model::ModelMode): unexpected mode requested.";
    };

  return purchase * (r * pow((pr / (bu-pr)), R) + 
		     l * pow(std::abs(1.0 - STATE_FACTOR * like), L) - 
		     2.0);
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

float ModelData::get_ad(AgentID a, BrandID b, size_t t) {
  std::map<AgentID, AdCampaign>::const_iterator ad_i = _ads.find(a);
  if( ad_i == _ads.end() )
    throw "ModelData::get_ad(AgentID, BrandID, size_t): "
      "no Agent matching requested AgentID";
  return ad_i->second.get( b ).get( t );
}

///////////////////////////////////////////////////////////////////////////////

