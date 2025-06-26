#include "range.h"
#include <algorithm>

/**
	* @brief Transforms values from 0.0 to 1.0 to given range of Object
	* 
	* @param throttle 
	* @return float 
	*/
float Range::transform(float throttle) const
{
	throttle = std::clamp(throttle, 0.0f, 1.0f);
	float diff {max - min};

	return min + (diff * throttle);
}
