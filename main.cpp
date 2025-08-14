#include "chartify.h"
#include <vector>
#include <cmath>
using namespace Chartify;
int main() {
    std::vector<std::vector<double>> f(3);
    for(double j = -10; j < 10; j += 0.1) {
        f[0].push_back(j);
        f[1].push_back(j);
        f[2].push_back(j);
    }
    std::vector<std::vector<double>> g(3);
    for(double x : f[0]) {
        g[0].push_back(cos(x));
        g[1].push_back(sin(x));
        g[2].push_back(sin(x + M_PI / 3));
    }
    std::vector<unsigned int> flag = {Flag::Solid, Flag::Solid, Flag::Dashed};
    std::vector<Color> color = {Color::Blue(), Color::Red(), Color::Orange()};
    Canvas plt;
    plt.ConfigurePlot(f, g, color, flag);
    plt.Plot();
    plt.Show();
    return 0;
}