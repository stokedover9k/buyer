#ifndef __AD_CAMPAIGN_H__
#define __AD_CAMPAIGN_H__

#include "model-defs.h"

#include <map>
#include <vector>

class AdSeq;

///////////////////////////////////////////////////////////////////////////////

class AdCampaign {
  
 public:
  float get( BrandID brand, uint32_t time ) const;
  const AdSeq& get( BrandID brand ) const;
  AdSeq& get( BrandID brand );
  
 private:
  std::map<BrandID, AdSeq> _campaign;
  
};

///////////////////////////////////////////////////////////////////////////////

class AdSeq {

 public:
  AdSeq() {  }
  AdSeq(std::vector<float> ads);

  float front(void) const;
  float get(uint32_t time) const;
  friend float& operator << (float& v, AdSeq& a);
  friend AdSeq& operator >> (AdSeq& a, float& v);
  
  void add(float);

  uint32_t length(void) const;
  void reset_iterator(void);
  
 private:
  std::vector<float> _ads;
  std::vector<float>::const_iterator _ad;
  
};

#endif
