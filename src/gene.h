#ifndef __gene_h__
  #define __gene_h__

#include <vector>
#include <array>

enum class Gene
{
	SPEED,
	CAMOUFLAGE,
	HP,
	NUTRITION,
	ATTACK,
	VISION_RADIUS,
	REPRODUCTION_RATE,
	ATTRACTION,
	MOTION_THRESHOLD, // nicht verwendet
	ATTACK_THRESHOLD,
	CAMOUFLAGE_THRESHOLD,
	FERTILE_AGE,
	FERTILE_ENERGY_LEVEL,
	CANNIBALISM,
	COUNT // Muss am Ende stehen
};

constexpr size_t GENE_COUNT = static_cast<size_t>(Gene::COUNT);

/**
 * @brief Compute amount of genetic difference between two genes
 * @param a
 * @param b
 * @return Measure of difference
 */
float geneticDifference(const std::array<float, GENE_COUNT>& a, const std::array<float, GENE_COUNT>& b);

#endif