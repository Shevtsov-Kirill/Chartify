#include "chartify.h"
#include <vector>
using namespace Chartify;
int main(){
    std::size_t data = 3, value = 40;
    sf::String title = "I(R)";
    Color back = Color::White(), grid = Color({180, 180, 180}, 200);
    auto profile = std::make_unique<RenderProfile>(1200, 600, title);
    std::vector<std::vector<double>> f = {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, g = {{1, 1, 1}, {1, 9, 81}, {1, 8, 27}};
    std::vector<unsigned int> flag = {Flag::Solid, Flag::Solid, Flag::Solid};
    std::vector<Color> color = {Color::Blue(), Color::Red(), Color::Green()};
    Chartify::Canvas plt(std::move(profile), back, grid, Color({20, 20, 20}, 255), Flag::Axes | Flag::Grid);
    plt.ConfigurePlot(f, g, color, flag);
    plt.Plot();
    plt.Show();
}