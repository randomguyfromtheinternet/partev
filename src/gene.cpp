#include "gene.h"

#include <cmath>

float geneticDifference(const std::array<float, GENE_COUNT>& a, const std::array<float, GENE_COUNT>& b)
{
	float diff = 0.0f;
	for (size_t i = 0; i < GENE_COUNT; ++i)
	{
		float d = a[i] - b[i];
		diff += d * d;
	}
	return std::sqrt(diff / GENE_COUNT); // Wurzel des mittleren quadratischen Unterschieds
}