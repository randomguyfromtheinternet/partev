#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <array>
#include <cmath>

#include "gene.h"
#include "particle.h"

constexpr size_t max_particles {500};
constexpr size_t initial_particles {300};

// ---- MAIN ----
int main()
{
	const unsigned int windowWidth {800};
	const unsigned int windowHeight {800};

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Jagd und Vermehrung");
	window.setFramerateLimit(60);

	std::vector<Particle> particles;
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> posDist(0, windowWidth);
	std::uniform_real_distribution<float> sizeDist(3.f, 6.f);
	std::uniform_int_distribution<int> colorDist(100, 255);

	for (int i = 0; i < initial_particles; ++i)
	{
		float x = posDist(rng);
		float y = posDist(rng);
		float radius = sizeDist(rng);
		sf::Color color(colorDist(rng), colorDist(rng), colorDist(rng));
		particles.emplace_back(x, y, radius, color, rng);
	}

	// Einfärben nach Kannibalismus
	for(auto& p : particles)
	{
		float cannibalism = p.dna[static_cast<size_t>(Gene::CANNIBALISM)];
		p.color = sf::Color(
			static_cast<sf::Uint8>(255 * cannibalism),
			100,
			static_cast<sf::Uint8>(255 * (1.0f - cannibalism))
			);
	}

	sf::Clock clock;
	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Update + Reproduktion
		std::vector<Particle> newParticles;
		for (auto& p : particles)
		{
			if (!p.alive) continue;
			p.update(dt, rng, particles);
			if (p.canReproduce() && particles.size() < max_particles) {
				newParticles.push_back(p.reproduce(rng));
			}
		}

		particles.insert(particles.end(), newParticles.begin(), newParticles.end());

		// Zeichnen
		window.clear(sf::Color::Black);
		for (auto& p : particles)
		{
			p.draw(window);
		}
		window.display();
	}

	return 0;
}
