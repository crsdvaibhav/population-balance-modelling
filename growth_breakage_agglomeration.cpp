#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Function to calculate growth rate G(x, c)
double growthRate(double x, double c, double D, double rho_c, double delta, double c_star) {
    return (4 * D / rho_c) * ((1.0 / x) - (1.0 / (2 * delta))) * (c - c_star);
}

// Function to calculate breakage probability per unit time
double breakageRate(double x, double k) {
    return k * x; // Breakage rate proportional to size
}

// Function to calculate agglomeration kernel (simplified example: constant probability)
double agglomerationKernel(double x1, double x2) {
    return (x1 + x2) / 2; // Larger particles have a higher probability of agglomeration
}

// Function to generate daughter particle sizes (equal splitting)
std::vector<double> generateDaughters(double parentSize) {
    return {parentSize / 2.0, parentSize / 2.0}; // Split into two equal parts
}

// Function to initialize particle sizes randomly
std::vector<double> initializeParticles(int numParticles, double minSize, double maxSize) {
    std::vector<double> particles(numParticles);
    for (int i = 0; i < numParticles; ++i) {
        particles[i] = minSize + static_cast<double>(rand()) / RAND_MAX * (maxSize - minSize);
    }
    return particles;
}

int main() {
    // Seed random number generator
    srand(static_cast<unsigned>(time(0)));

    // Parameters
    int initialParticles = 1000; // Initial number of particles
    double minSize = 1e-6;       // Minimum initial size (m)
    double maxSize = 1e-3;       // Maximum initial size (m)
    double nucleationRate = 1e3; // Nucleation rate (particles/s)
    double c0 = 1.0;             // Initial supersaturation
    double c_star = 0.8;         // Critical concentration
    double D = 1e-10;            // Diffusion coefficient
    double rho_c = 1e3;          // Crystal density
    double delta = 1e-6;         // Boundary layer thickness
    double breakageConstant = 1e4; // Breakage constant (arbitrary units)
    double agglomerationConstant = 1e-4; // Agglomeration constant
    double timeStep = 0.01;      // Time step (s)
    double totalTime = 1.0;      // Total simulation time (s)

    // Initialize particle sizes
    std::vector<double> particles = initializeParticles(initialParticles, minSize, maxSize);

    // Simulation
    int steps = static_cast<int>(totalTime / timeStep);
    for (int step = 0; step < steps; ++step) {
        // Particle growth
        for (double& size : particles) {
            double G = growthRate(size, c0, D, rho_c, delta, c_star);
            size += G * timeStep;
        }

        // Particle breakage
        std::vector<double> newParticles; // To hold new particles from breakage
        for (auto it = particles.begin(); it != particles.end();) {
            double size = *it;
            double breakageProb = breakageRate(size, breakageConstant) * timeStep;

            // Randomly decide if the particle breaks
            if ((static_cast<double>(rand()) / RAND_MAX) < breakageProb) {
                // Generate daughter particles
                std::vector<double> daughters = generateDaughters(size);
                newParticles.insert(newParticles.end(), daughters.begin(), daughters.end());

                // Remove the parent particle
                it = particles.erase(it);
            } else {
                ++it;
            }
        }

        // Add new particles from breakage
        particles.insert(particles.end(), newParticles.begin(), newParticles.end());

        // Particle agglomeration
        for (size_t i = 0; i < particles.size(); ++i) {
            for (size_t j = i + 1; j < particles.size(); ++j) {
                double kernel = agglomerationKernel(particles[i], particles[j]) * agglomerationConstant * timeStep;
                if ((static_cast<double>(rand()) / RAND_MAX) < kernel) {
                    // Merge the two particles
                    double newSize = particles[i] + particles[j]; // Simplified: summing sizes
                    particles[i] = newSize;

                    // Remove the second particle
                    particles.erase(particles.begin() + j);
                    --j; // Adjust index after removal
                }
            }
        }

        // Output particle sizes and numbers at each step
        if (step % 100 == 0) {
            std::cout << "Time: " << step * timeStep << " s\n";
            std::cout << "Number of particles: " << particles.size() << "\n";
            for (const double& size : particles) {
                std::cout << size << " ";
            }
            std::cout << "\n";
        }

        // Decrease supersaturation due to growth (simplified)
        c0 -= nucleationRate * timeStep;
        if (c0 < c_star) c0 = c_star; // Ensure supersaturation doesn't drop below critical value
    }

    return 0;
}
