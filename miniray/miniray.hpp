#ifndef MINIRAY_HPP
#define MINIRAY_HPP

/*
 * A single header ray tracer with very basic functionality.
 * Reference: https://scratchapixel.com
 */
#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

const double PI{3.1415926535897932385};
const double INF{std::numeric_limits<double>::infinity()};
const int MAX_DEPTH{3};

namespace mini_ray {

template <typename T> class Vec3 {
  public:
    T x{}, y{}, z{};

    Vec3() : x{0}, y{0}, z{0} {};
    explicit Vec3(T x) : x{x}, y{x}, z{x} {};
    Vec3(T x, T y, T z) : x{x}, y{y}, z{z} {};

    // Operator overloads
    Vec3<T> operator*(const T &f) const { return Vec3<T>{x * f, y * f, z * f}; }
    Vec3<T> operator*(const Vec3<T> &v) const {
        return Vec3<T>{x * v.x, y * v.y, z * v.z};
    } // For colour scaling
    Vec3<T> &operator*=(const Vec3<T> &v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }

    Vec3<T> operator+(const Vec3<T> &v) const {
        return Vec3<T>{x + v.x, y + v.y, z + v.z};
    }
    Vec3<T> &operator+=(const Vec3<T> &v) {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    Vec3<T> operator-() const { return Vec3<T>{-x, -y, -z}; }
    Vec3<T> operator-(const Vec3<T> &v) const {
        return Vec3<T>{x - v.x, y - v.y, z - v.z};
    }

    // Utility
    T length_squared() const { return x * x + y * y + z * z; }
    T length() const { return std::sqrt(length_squared()); }
    T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3<T> normalise() {
        T normal_squared{length_squared()};

        if (normal_squared > 0) {
            T inv_normal{1 / sqrt(normal_squared)};
            // *this *= inv_normal; // POT_ERR

            x *= inv_normal;
            y *= inv_normal;
            z *= inv_normal;
        }

        return *this;
    }
    friend std::ostream &operator<<(std::ostream &out, const Vec3<T> &v) {
        out << "[" << v.x << ' ' << v.y << ' ' << v.z << "]";
        return out;
    }
};

// Common type aliases
template <typename T> using Point3 = Vec3<T>;
using Vec3f = Vec3<double>;
using Point3f = Point3<double>;

class Sphere {
  public:
    Point3<double> centre{};
    double radius{};
    Vec3f surface_colour{};
    double reflection{}, transparency{};
    Vec3f emission_colour{};
    double radius_squared{};

    Sphere(const Point3f &centre, const double &radius,
           const Vec3f &surface_colour, const double &reflection = 0,
           const double &transparency = 0,
           const Vec3f &emission_colour = Vec3f{})
        : centre{centre}, radius{radius}, surface_colour{surface_colour},
          reflection{reflection}, transparency{transparency},
          emission_colour{emission_colour}, radius_squared{radius * radius} {}

    bool intersect(const Vec3f &ray_orig, const Vec3f &ray_dir, double &t0,
                   double &t1) const {
        Vec3f l{centre - ray_orig}; // Distance to sphere centre
        auto tca{l.dot(ray_dir)};

        if (tca < 0)
            return false;

        auto d_squared{l.length_squared() - tca * tca};

        if (d_squared > radius_squared)
            return false;

        auto thc{sqrt(radius_squared - d_squared)};
        t0 = tca - thc;
        t1 = tca + thc;

        return true;
    }
};

inline double mix(const double &a, const float &b, const float &mix) {
    return b * mix + a * (1 - mix);
}

inline Vec3f trace(const Vec3f &ray_orig, const Vec3f &ray_dir,
                   const std::vector<Sphere> &spheres, const int &depth) {
    auto t_near{INF};
    const Sphere *sphere{nullptr};

    // Find ray -> sphere intersection
    for (const auto &curr_sphere : spheres) {
        auto t0{INF}, t1{INF};

        auto intersected{curr_sphere.intersect(ray_orig, ray_dir, t0, t1)};

        if (intersected) {
            if (t0 < 0)
                t0 = t1;

            if (t0 < t_near) {
                t_near = t0;
                sphere = &curr_sphere;
            }
        }
    }

    // No intersection, return background colour
    if (!sphere)
        return Vec3f{2};

    Vec3f surface_colour{};
    Point3f p_hit{ray_orig + ray_dir * t_near}; // Point of intersection
    Vec3f n_hit{p_hit - sphere->centre};        // Normal at intersection
    n_hit.normalise();

    double bias{1e-4};
    bool inside{false};

    // If normal vector is in same direction as ray, we have intersected with
    // the inside of a sphere. Reverse the direction of the normal and set
    // inside flag to true.
    if (ray_dir.dot(n_hit) > 0) {
        n_hit = -n_hit;
        inside = true;
    }

    // Adjust colour based on object transparency and reflectivity properties
    if ((sphere->transparency > 0 || sphere->reflection > 0) &&
        depth < MAX_DEPTH) {
        auto facing_ratio{-ray_dir.dot(n_hit)};
        double fresnel_effect{mix(std::pow(1 - facing_ratio, 3), 1, 0.1)};

        auto refl_dir{ray_dir - n_hit * 2 * ray_dir.dot(n_hit)};
        refl_dir.normalise();

        auto reflection{trace(p_hit + n_hit * bias, refl_dir, spheres,
                              depth + 1)}; // Recursively bounce ray
        Vec3f refraction{};

        // Calculate refraction ray if sphere is transparent
        if (sphere->transparency > 0) {
            double ior{1.1};
            double eta{inside ? ior : 1 / ior};
            double cosi{-n_hit.dot(ray_dir)};
            auto k{1 - (eta * eta) * (1 - (cosi * cosi))};

            auto refr_dir{ray_dir * eta + n_hit * (eta * cosi - sqrt(k))};
            refr_dir.normalise();

            refraction =
                trace(p_hit - n_hit * bias, refr_dir, spheres, depth + 1);
        }

        surface_colour =
            (reflection * fresnel_effect +
             refraction * (1 - fresnel_effect) * sphere->transparency) *
            sphere->surface_colour;
    } else {
        // Diffuse object, no need to trace any further
        for (unsigned int i = 0; i < spheres.size(); ++i) {
            // Check for light
            if (spheres[i].emission_colour.x > 0) {
                Vec3f transmission{1};

                auto light_direction{spheres[i].centre - p_hit};
                light_direction.normalise();

                // Check light -> world object interactions
                for (unsigned int j = 0; j < spheres.size(); ++j) {
                    // Don't compare same sphere
                    if (i != j) {
                        double t0{}, t1{};

                        if (spheres[j].intersect(p_hit + n_hit * bias,
                                                 light_direction, t0, t1)) {
                            transmission = Vec3f{0};

                            break;
                        }
                    }
                }

                surface_colour += sphere->surface_colour * transmission *
                                  std::max(static_cast<double>(0),
                                           n_hit.dot(light_direction)) *
                                  spheres[i].emission_colour;
            }
        }
    }

    return surface_colour + sphere->emission_colour;
}

// Compute a ray for each pixel. If the ray hits an object, calculate colour of
// object at intersection point. Otherwise, return the background colour.
inline void render(const std::vector<Sphere> &spheres) {
    int image_width{600}, image_height{480};

    auto *image = new Vec3f[image_width * image_height];
    Vec3f *pixel = image;

    const double inv_width{1 / static_cast<double>(image_width)},
        inv_height{1 / static_cast<double>(image_height)};

    const int fov{30};
    const double aspect_ratio{image_width / static_cast<double>(image_height)};
    const double look_angle{tan(PI * 0.5 * fov / 180.)};

    // Trace
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x, ++pixel) {
            double xx{(2 * ((x + 0.5) * inv_width) - 1) * look_angle *
                      aspect_ratio};
            double yy{(1 - 2 * ((y + 0.5) * inv_height)) * look_angle};

            Vec3f ray_dir{xx, yy, -1};
            ray_dir.normalise();

            *pixel = trace(Vec3f{}, ray_dir, spheres, 0);
        }
    }

    std::ofstream ofs("./miniray/image.ppm");
    ofs << "P6\n" << image_width << " " << image_height << "\n255\n";

    for (int i = 0; i < image_width * image_height; ++i) {
        ofs << static_cast<unsigned char>(
                   std::min(static_cast<double>(1), image[i].x) * 255)
            << static_cast<unsigned char>(
                   std::min(static_cast<double>(1), image[i].y) * 255)
            << static_cast<unsigned char>(
                   std::min(static_cast<double>(1), image[i].z) * 255);
    }

    ofs.close();
    delete[] image;
}

} // namespace mini_ray

#endif
