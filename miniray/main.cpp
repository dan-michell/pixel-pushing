#include "miniray.hpp"

#include <cstdlib>
#include <vector>

int main(int argc, char const *argv[]) {
    srand48(13);
    std::vector<mini_ray::Sphere> spheres{};

    spheres.emplace_back(mini_ray::Vec3f{0.0, -10004, -20}, 10000,
                         mini_ray::Vec3f{0.20, 0.20, 0.20}, 0, 0.0);
    spheres.emplace_back(mini_ray::Vec3f{0.0, 0, -20}, 4,
                         mini_ray::Vec3f{1.00, 0.32, 0.36}, 1, 0.5);
    spheres.emplace_back(mini_ray::Vec3f{5.0, -1, -15}, 2,
                         mini_ray::Vec3f{0.90, 0.76, 0.46}, 1, 0.0);
    spheres.emplace_back(mini_ray::Vec3f{5.0, 0, -25}, 3,
                         mini_ray::Vec3f{0.65, 0.77, 0.97}, 1, 0.0);
    spheres.emplace_back(mini_ray::Vec3f{-5.5, 0, -15}, 3,
                         mini_ray::Vec3f{0.90, 0.90, 0.90}, 1, 0.0);
    // light
    spheres.emplace_back(mini_ray::Vec3f{0.0, 20, -30}, 3,
                         mini_ray::Vec3f{0.00, 0.00, 0.00}, 0, 0.0,
                         mini_ray::Vec3f{3});

    render(spheres);

    return 0;
}
