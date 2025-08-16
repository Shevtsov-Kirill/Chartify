#include "matplotlib.h"
#include <vector>
#include <cmath>
using namespace matplotlib;
int main() {
    std::vector<std::vector<double>> f(3);
    for(double j = -10; j < 10; j += 0.1) {
        f[0].push_back(j);
        f[1].push_back(j);
        f[2].push_back(j);
    }
    std::vector<std::vector<double>> g(3);
    for(double x : f[0]) {
        g[0].push_back(sin(x));
        g[1].push_back(sin((x * x * x)));
        g[2].push_back(sin(x + M_PI / 12));
    }
    std::vector<unsigned int> flag = {Flag::Solid, Flag::Dotted, Flag::Dashed};
    std::vector<Color> color = {Color({0, 255, 0}, 255), Color({255, 165, 0}, 255), Color::Blue()};
    Canvas plt;
    sf::String title = "Dependence of the magnetic field on the inductance of the coil";
    plt.ConfigurePlot(f, g, color, flag);
    plt.Title(title);
    plt.Plot();
    plt.Show();
    return 0;
}