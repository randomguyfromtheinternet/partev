#ifndef __range_h__
#define __range_h__

class Range
{
	public:
		float min;
		float max;

		Range()
			: min {0.0f}
			, max {1.0f}
		{}

		Range (float min, float max)
			: min {min}
			, max {max}
		{}

		float transform (float throttle) const;
};

#endif