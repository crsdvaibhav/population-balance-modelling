#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Constants for simulation
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PARTICLE_SCALE = 100.0f; // Scale particle size for visualization

// Function to calculate growth rate G(x, c)
double growthRate(double x, double c, double D, double rho_c, double delta, double c_star) {
    return (4 * D / rho_c) * ((1.0 / x) - (1.0 / (2 * delta))) * (c - c_star);
}

// Function to calculate breakage probability per unit time
double breakageRate(double x, double k) {
    return k * x;
}

// Function to calculate agglomeration kernel
double agglomerationKernel(double x1, double x2) {
    return (x1 + x2) / 2;
}

// Function to generate daughter particles
std::vector<double> generateDaughters(double parentSize) {
    return {parentSize / 2.0, parentSize / 2.0};
}

// Function to initialize particles with random sizes and positions
void initializeParticles(std::vector<sf::CircleShape>& particles, std::vector<double>& sizes,
                         int numParticles, double minSize, double maxSize) {
    for (int i = 0; i < numParticles; ++i) {
        double size = minSize + static_cast<double>(rand()) / RAND_MAX * (maxSize - minSize);
        sizes.push_back(size);

        // Create a circle shape for the particle
        sf::CircleShape circle(size * PARTICLE_SCALE);
        circle.setFillColor(sf::Color::White);
        circle.setPosition(static_cast<float>(rand() % WINDOW_WIDTH),
                           static_cast<float>(rand() % WINDOW_HEIGHT));
        particles.push_back(circle);
    }
}

int main() {
    // Seed random number generator
    srand(static_cast<unsigned>(time(0)));

    // Simulation parameters
    const int initialParticles = 100;    // Number of particles
    const double minSize = 1e-6;         // Minimum size
    const double maxSize = 1e-4;         // Maximum size
    const double nucleationRate = 1e3;   // Nucleation rate
    const double c0 = 1.0;               // Initial supersaturation
    const double c_star = 0.8;           // Critical concentration
    const double D = 1e-10;              // Diffusion coefficient
    const double rho_c = 1e3;            // Crystal density
    const double delta = 1e-6;           // Boundary layer thickness
    const double breakageConstant = 1e4;
    const double agglomerationConstant = 1e-4;
    const double timeStep = 0.01;        // Time step
    const int simulationSteps = 5000;   // Number of simulation steps

    // Initialize particles
    std::vector<sf::CircleShape> particles;
    std::vector<double> sizes;
    initializeParticles(particles, sizes, initialParticles, minSize, maxSize);

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Dynamics");

    // Simulation loop
    for (int step = 0; step < simulationSteps && window.isOpen(); ++step) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Particle growth
        for (size_t i = 0; i < sizes.size(); ++i) {
            double G = growthRate(sizes[i], c0, D, rho_c, delta, c_star);
            sizes[i] += G * timeStep;
            particles[i].setRadius(sizes[i] * PARTICLE_SCALE);
        }

        // Particle breakage
        std::vector<sf::CircleShape> newParticles;
        std::vector<double> newSizes;
        for (size_t i = 0; i < sizes.size();) {
            double breakageProb = breakageRate(sizes[i], breakageConstant) * timeStep;
            if ((static_cast<double>(rand()) / RAND_MAX) < breakageProb) {
                // Break the particle
                std::vector<double> daughters = generateDaughters(sizes[i]);
                for (double daughterSize : daughters) {
                    sf::CircleShape daughter(daughterSize * PARTICLE_SCALE);
                    daughter.setFillColor(sf::Color::White);
                    daughter.setPosition(particles[i].getPosition());
                    newParticles.push_back(daughter);
                    newSizes.push_back(daughterSize);
                }
                sizes.erase(sizes.begin() + i);
                particles.erase(particles.begin() + i);
            } else {
                ++i;
            }
        }
        particles.insert(particles.end(), newParticles.begin(), newParticles.end());
        sizes.insert(sizes.end(), newSizes.begin(), newSizes.end());

        // Particle agglomeration
        for (size_t i = 0; i < sizes.size(); ++i) {
            for (size_t j = i + 1; j < sizes.size(); ++j) {
                double kernel = agglomerationKernel(sizes[i], sizes[j]) * agglomerationConstant * timeStep;
                if ((static_cast<double>(rand()) / RAND_MAX) < kernel) {
                    // Merge the particles
                    sizes[i] += sizes[j];
                    particles[i].setRadius(sizes[i] * PARTICLE_SCALE);
                    sizes.erase(sizes.begin() + j);
                    particles.erase(particles.begin() + j);
                    --j;
                }
            }
        }

        // Rendering
        window.clear();
        for (const auto& particle : particles) {
            window.draw(particle);
        }
        window.display();
    }

    return 0;
}
