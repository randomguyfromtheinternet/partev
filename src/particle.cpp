#include "particle.h"
#include "utils.h"

Particle::Particle(float x, float y, float r, sf::Color c, std::mt19937& rng)
	: position(x, y), velocity(0.f, 0.f), radius(r), color(c)
{
	std::uniform_real_distribution<float> geneDist(0.0f, 1.0f);
	for (auto& gene : dna)
	{
		gene = geneDist(rng);
	}
}

void Particle::update(float dt, std::mt19937& rng, std::vector<Particle>& others)
{
	age += dt;

	// Energieverbrauch durch Bewegung
	float speed = dna[static_cast<size_t>(Gene::SPEED)] * 500.f;

	// Jagd: suche nächstes Ziel in Sichtweite
	float vision = dna[static_cast<size_t>(Gene::VISION_RADIUS)] * 100.f;
	Particle* target = nullptr;
	float minDist = vision;

	for (auto& other : others)
	{
		if (&other == this || !other.alive) continue;

		float dx = other.position.x - position.x;
		float dy = other.position.y - position.y;
		float dist = std::sqrt(dx*dx + dy*dy);

		if (dist < minDist)
		{
			minDist = dist;
			target = &other;
		}
	}
	if (target && target->alive)
	{
		float attackVal = dna[static_cast<size_t>(Gene::ATTACK)];
		float threshold = dna[static_cast<size_t>(Gene::ATTACK_THRESHOLD)];
		float cannibalism = dna[static_cast<size_t>(Gene::CANNIBALISM)];
		float similarity = geneticDifference(dna, target->dna);

		if (attackVal > threshold && similarity > (1.0f - cannibalism))
		{
			// Bewegung in Richtung Ziel
			sf::Vector2f dir = target->position - position;
			float len = length(dir);
			if (len > 0)
			{
				dir /= len;
				velocity = dir * speed;
				position += velocity * dt;
			}

			if (minDist < radius + target->radius)
			{
				target->alive = false;
				energy += target->dna[static_cast<size_t>(Gene::NUTRITION)] * 2.f;
			}
		}
	}
	else
	{
		// Zufallsbewegung
		std::uniform_real_distribution<float> dir(-1.f, 1.f);
		velocity.x = dir(rng) * speed;
		velocity.y = dir(rng) * speed;
		position += velocity * dt;
	}

	energy -= dt * 0.01f * length(velocity);

	// Fenstergrenzen
	position.x = std::clamp(position.x, 0.f, 800.f - radius * 2);
	position.y = std::clamp(position.y, 0.f, 800.f - radius * 2);
}

void Particle::draw(sf::RenderWindow& window)
{
	if (!alive) return;
	sf::CircleShape shape(radius);
	shape.setPosition(position);
	shape.setFillColor(color);
	window.draw(shape);
}

bool Particle::canReproduce() const
{
	return
		energy > 1.5f * dna[static_cast<size_t>(Gene::FERTILE_ENERGY_LEVEL)]
		&& age > (1.0f + 10.0f *dna[static_cast<size_t>(Gene::FERTILE_AGE)]);
}

Particle Particle::reproduce(std::mt19937& rng)
{
	Particle child = *this;
	child.position.x += radius * 2; // leichte Verschiebung
	child.position.y += radius * 2;
	child.age = 0.f;
	child.energy = energy / 2.f;
	energy /= 2.f;

	// Mutation
	std::normal_distribution<float> mut(0.0f, 0.02f);
	for (auto& gene : child.dna)
	{
		gene = std::clamp(gene + mut(rng), 0.0f, 1.0f);
	}

	return child;
}