#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <array>
#include <cmath>

constexpr size_t max_particles{500};

// ---- DNA-Definition ----

enum class Gene {
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

// ---- Hilfsfunktionen ----
float length(sf::Vector2f vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float geneticDifference(const std::array<float, GENE_COUNT>& a, const std::array<float, GENE_COUNT>& b) {
    float diff = 0.0f;
    for (size_t i = 0; i < GENE_COUNT; ++i) {
        float d = a[i] - b[i];
        diff += d * d;
    }
    return std::sqrt(diff / GENE_COUNT); // Wurzel des mittleren quadratischen Unterschieds
}

// ---- Partikel ----
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::Color color;
    std::array<float, GENE_COUNT> dna;

    float age = 0.f;
    float energy = 1.f;
    bool alive = true;

    Particle(float x, float y, float r, sf::Color c, std::mt19937& rng)
        : position(x, y), velocity(0.f, 0.f), radius(r), color(c) {
        std::uniform_real_distribution<float> geneDist(0.0f, 1.0f);
        for (auto& gene : dna) {
            gene = geneDist(rng);
        }
    }

    void update(float dt, std::mt19937& rng, std::vector<Particle>& others) {
        age += dt;

        // Energieverbrauch durch Bewegung
        float speed = dna[static_cast<size_t>(Gene::SPEED)] * 500.f;

        // Jagd: suche nächstes Ziel in Sichtweite
        float vision = dna[static_cast<size_t>(Gene::VISION_RADIUS)] * 100.f;
        Particle* target = nullptr;
        float minDist = vision;

        for (auto& other : others) {
            if (&other == this || !other.alive) continue;

            float dx = other.position.x - position.x;
            float dy = other.position.y - position.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < minDist) {
                minDist = dist;
                target = &other;
            }
        }
        if (target && target->alive) {
            float attackVal = dna[static_cast<size_t>(Gene::ATTACK)];
            float threshold = dna[static_cast<size_t>(Gene::ATTACK_THRESHOLD)];
            float cannibalism = dna[static_cast<size_t>(Gene::CANNIBALISM)];
            float similarity = geneticDifference(dna, target->dna);

            if (attackVal > threshold && similarity > (1.0f - cannibalism)) {
                // Bewegung in Richtung Ziel
                sf::Vector2f dir = target->position - position;
                float len = length(dir);
                if (len > 0) {
                    dir /= len;
                    velocity = dir * speed;
                    position += velocity * dt;
                }

                if (minDist < radius + target->radius) {
                    target->alive = false;
                    energy += target->dna[static_cast<size_t>(Gene::NUTRITION)] * 2.f;
                }
            }
        } else {
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

    void draw(sf::RenderWindow& window) {
        if (!alive) return;
        sf::CircleShape shape(radius);
        shape.setPosition(position);
        shape.setFillColor(color);
        window.draw(shape);
    }

    bool canReproduce() const {
        return
            energy > 1.5f * dna[static_cast<size_t>(Gene::FERTILE_ENERGY_LEVEL)]
            && age > (1.0f + 10.0f *dna[static_cast<size_t>(Gene::FERTILE_AGE)]);
    }

    Particle reproduce(std::mt19937& rng) {
        Particle child = *this;
        child.position.x += radius * 2; // leichte Verschiebung
        child.position.y += radius * 2;
        child.age = 0.f;
        child.energy = energy / 2.f;
        energy /= 2.f;

        // Mutation
        std::normal_distribution<float> mut(0.0f, 0.02f);
        for (auto& gene : child.dna) {
            gene = std::clamp(gene + mut(rng), 0.0f, 1.0f);
        }

        return child;
    }
};

// ---- MAIN ----
int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 800;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Jagd und Vermehrung");
    window.setFramerateLimit(60);

    std::vector<Particle> particles;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> posDist(0, windowWidth);
    std::uniform_real_distribution<float> sizeDist(3.f, 6.f);
    std::uniform_int_distribution<int> colorDist(100, 255);

    for (int i = 0; i < 300; ++i) {
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
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update + Reproduktion
        std::vector<Particle> newParticles;
        for (auto& p : particles) {
            if (!p.alive) continue;
            p.update(dt, rng, particles);
            if (p.canReproduce() && particles.size() < max_particles) {
                newParticles.push_back(p.reproduce(rng));
            }
        }

        particles.insert(particles.end(), newParticles.begin(), newParticles.end());

        // Zeichnen
        window.clear(sf::Color::Black);
        for (auto& p : particles) {
            p.draw(window);
        }
        window.display();
    }

    return 0;
}
