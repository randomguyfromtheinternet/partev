#ifndef __particle_h__
	#define __particle_h__

#include "gene.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <random>

class Particle
{
	public:
		sf::Vector2f position;
		sf::Vector2f velocity;
		float radius;
		sf::Color color;
		std::array<float, GENE_COUNT> dna;

		float age = 0.f;
		float energy = 1.f;
		bool alive = true;

	public:
		Particle(float x, float y, float r, sf::Color c, std::mt19937& rng);
		void update(float dt, std::mt19937& rng, std::vector<Particle>& others);
		void draw(sf::RenderWindow& window);
		bool canReproduce() const;
		Particle reproduce(std::mt19937& rng);
};

#endif