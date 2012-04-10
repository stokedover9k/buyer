#include "ad-campaign.h"

float AdCampaign::get( BrandID b, uint32_t t ) const {
  try {
    return get( b )
      .get( t );
  } catch (const char* e) {
    throw "AdCampaign::get(BrandID, uint32_t) const: brand not found.";
  }
}

const AdSeq& AdCampaign::get( BrandID b ) const {
  std::map<BrandID, AdSeq>::const_iterator itr = _campaign.find( b );
  if( itr == _campaign.end() )
    throw "AdCampaign::get(BrandID) const: brand not found.";
  return itr->second;
}

AdSeq& AdCampaign::get( BrandID b ) {
  std::map<BrandID, AdSeq>::iterator itr = _campaign.find( b );
  if( itr == _campaign.end() )
    itr = _campaign.insert( std::pair<BrandID, AdSeq>(b, AdSeq()) ).first;
  return itr->second;
}

///////////////////////////////////////////////////////////////////////////////

AdSeq::AdSeq( std::vector<float> ads ) : _ads(ads)
{ _ad = _ads.begin(); }

float AdSeq::front(void) const
{ return *_ad; }

float AdSeq::get(uint32_t t) const {
  if( _ads.size() <= t ) 
    throw "AdSeq::get(uint32_t): unexpected index (time) requested.";
  return _ads[t]; 
}

float& operator << (float& v, AdSeq& a)
{ v = *(a._ad++);  return v; }

AdSeq& operator >> (AdSeq& a, float& v)
{ v = *(a._ad++);  return a; }

uint32_t AdSeq::length(void) const
{ return _ads.size(); }

void AdSeq::reset_iterator(void)
{ _ad = _ads.begin(); }
