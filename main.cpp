#include "chartify.h"
#include <vector>
#include <cmath>
using namespace Chartify;
int main() {
    std::vector<std::vector<double>> f(4);
    for(double j = -10; j < 10; j += 0.1) {
        f[0].push_back(j);
        f[1].push_back(j);
        f[2].push_back(j);
        f[3].push_back(j);
    }
    std::vector<std::vector<double>> g(4);
    for(double x : f[0]) {
        g[0].push_back(cos(x));
        g[1].push_back(sin(x));
        g[2].push_back((sin(x + M_PI / 3)));
        g[3].push_back(cos(x + M_PI / 4));
    }
    std::vector<unsigned int> flag = {Flag::Solid, Flag::Solid, Flag::Dashed, Flag::Dashed};
    std::vector<Color> color = {Color::Blue(), Color::Red(), Color::Orange(), Color::Green()};
    Canvas plt;
    sf::String title = "Dependence of the magnetic field on the inductance of the coil y = sin(x)";
    plt.ConfigurePlot(f, g, color, flag);
    plt.Plot();
    plt.Title(title);
    plt.Show();
    return 0;
}