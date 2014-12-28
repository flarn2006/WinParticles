// This is intended to be #included from within a function.
// The source file must also include "ParamAgent.h", "Animation.h", and "AnimFunctions.h".

const int paramIDMappingPolar[] = { CParamAgent::MIN_VELOCITY, CParamAgent::MAX_VELOCITY, CParamAgent::BASE_ANGLE, CParamAgent::ANGLE_SIZE, CParamAgent::ACCELERATION_X, CParamAgent::ACCELERATION_Y, CParamAgent::MAXIMUM_AGE, CParamAgent::EMISSION_RATE, CParamAgent::EMISSION_RADIUS, CParamAgent::INNER_RADIUS };
const int paramIDMappingRect[] = { CParamAgent::MIN_VELOCITY_X, CParamAgent::MAX_VELOCITY_X, CParamAgent::MIN_VELOCITY_Y, CParamAgent::MAX_VELOCITY_Y, CParamAgent::ACCELERATION_X, CParamAgent::ACCELERATION_Y, CParamAgent::MAXIMUM_AGE, CParamAgent::EMISSION_RATE, CParamAgent::EMISSION_RADIUS, CParamAgent::INNER_RADIUS };
const CAnimation<double>::AnimFunction funcIDMapping[] = { AnimFunctions::Saw, AnimFunctions::Sine, AnimFunctions::Square, AnimFunctions::Triangle, AnimFunctions::Random };
