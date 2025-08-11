#include "chartify.h"
#include <iostream>
#include <cmath>
namespace chr = Chartify;
int main(){
    sf::String title = "My Graph";
    auto profile = std::make_unique<chr::RenderProfile>();
    std::vector<double> f = {1.5, 2.23, std::sqrt(2)}, g = {sqrt(1.5), sqrt(2.23), sqrt(sqrt(2))};
    Chartify::Canvas Graph(std::move(profile), chr::Color({255, 255, 255}, 255), chr::Color({23, 45, 67}, 67), chr::Color({89, 45, 67}, 255), chr::AXES | chr::GRID);
    Graph.Plot(f, g, chr::Color({0,0,255}, 255));
    Graph.Show();
}