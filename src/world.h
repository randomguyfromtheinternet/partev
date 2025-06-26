#ifndef __world_h__
#define __world_h__

#include "range.h"

namespace world
{
	namespace particle
	{
		const Range speed	{0.0f, 500.0f};
		const Range fertility {0.0f, 1.0f};
		const Range fertility_age {1.0f, 100.0f};
		const Range vision_radius {0.1f, 200.0f};
		const Range attack {0.0f, 100.0f};
		const Range health {10.0f, 1000.0f};
		const Range cannibalism {0.0f, 1.0f};
	}
}

#endif