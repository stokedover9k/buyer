#ifndef __MODEL_DEF_H__
#define __MODEL_DEF_H__

#include <iostream>

#include <stdint.h>

#define _FASH_STATES_BLOCK { HATE, DISLIKE, INDIFFERENT, LIKE, LOVE }
#define STATE_FACTOR 0.25

enum FashState _FASH_STATES_BLOCK;
#define N_FASH_STATES LOVE+1

typedef FashState LikeState;

typedef uint32_t AgentID;
typedef uint32_t BrandID;

#endif
