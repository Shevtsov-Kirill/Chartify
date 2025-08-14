#include "chartify.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace Chartify;
int main(){
    sf::String title = "As Chartify!";
    auto profile = std::make_unique<RenderProfile>(960, 480, title);
    std::vector<std::vector<double>> f = {{1, 2}, {2, 3}}, g = {{2, 4}, {1, 12}};
    std::vector<unsigned int> flag = {Flag::Dotted, Flag::Solid};
    std::vector<Color> color = {Color({178, 173, 23}, 255), Color::Blue()};
    Chartify::Canvas plt(std::move(profile), Color({0,0, 0}, 255), Color({100, 100, 100}, 67), Color({255, 255, 255}, 255), Flag::Axes | Flag::Grid);
    plt.ConfigurePlot(f, g, color, flag);
    plt.Plot();
    plt.Show();
}