#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>

void generate_bounded_points_oversample(
    std::vector<double>& x_bounded,
    std::vector<double>& y_bounded,
    std::mt19937& gen,
    std::uniform_real_distribution<>& dis,
    size_t z,
    int factor = 4)
{
    size_t oversample_size = z * factor;
    std::vector<double> x(oversample_size);
    std::vector<double> y(oversample_size);

    for (size_t i = 0; i < oversample_size; ++i) {
        x[i] = dis(gen);
        y[i] = dis(gen);
    }

    std::vector<bool> mask(oversample_size);
    size_t count = 0;
    for (size_t i = 0; i < oversample_size; ++i) {
        if (y[i] < x[i] && y[i] < 1.0 - x[i]) {
            mask[i] = true;
            ++count;
        } else {
            mask[i] = false;
        }
    }

    while (count < z) {
        factor += 1;
        oversample_size = z * factor;
        x.resize(oversample_size);
        y.resize(oversample_size);
        mask.resize(oversample_size);

        for (size_t i = 0; i < oversample_size; ++i) {
            x[i] = dis(gen);
            y[i] = dis(gen);
        }

        count = 0;
        for (size_t i = 0; i < oversample_size; ++i) {
            if (y[i] < x[i] && y[i] < 1.0 - x[i]) {
                mask[i] = true;
                ++count;
            } else {
                mask[i] = false;
            }
        }
    }

    x_bounded.clear();
    y_bounded.clear();
    for (size_t i = 0; i < oversample_size && x_bounded.size() < z; ++i) {
        if (mask[i]) {
            x_bounded.push_back(x[i]);
            y_bounded.push_back(y[i]);
        }
    }
}

int main() {
    size_t z = 10000000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    std::vector<double> x1(z);
    std::vector<double> y1(z);
    std::vector<double> x2(z);
    std::vector<double> y2(z);

    std::vector<double> x_bounded;
    std::vector<double> y_bounded;

    std::cout << "Generating bounded points..." << std::endl;
    generate_bounded_points_oversample(x_bounded, y_bounded, gen, dis, z);

    std::copy(x_bounded.begin(), x_bounded.end(), x1.begin());
    std::copy(y_bounded.begin(), y_bounded.end(), y1.begin());

    std::cout << "Generating free points..." << std::endl;
    for (size_t i = 0; i < z; ++i) {
        x2[i] = dis(gen);
        y2[i] = dis(gen);
    }

    std::cout << "Computing probabilities..." << std::endl;
    std::vector<double> x_sol(z, std::numeric_limits<double>::quiet_NaN());

    size_t count = 0;

    for (size_t i = 0; i < z; ++i) {
        if (x1[i] != x2[i]) {
            double numerator = (x1[i]*x1[i] - x2[i]*x2[i]) + (y1[i]*y1[i] - y2[i]*y2[i]);
            double denominator = 2.0 * (x1[i] - x2[i]);
            x_sol[i] = numerator / denominator;
        } else {
            if (y1[i] == y2[i]) {
                x_sol[i] = 0.5;
            }
        }

        if (!std::isnan(x_sol[i]) && x_sol[i] >= 0.0 && x_sol[i] <= 1.0) {
            count += 1;
        }
    }

    double probability = static_cast<double>(count) / static_cast<double>(z);
    std::cout << "Probability: " << probability << std::endl;

    return 0;
}
