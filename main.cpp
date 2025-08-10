#include "chartify.h"
#include <iostream>
namespace chr = Chartify;
int main(){
    sf::String title = "My Graph";
    auto profile = std::make_unique<chr::RenderProfile>(960, 480, title);
    Chartify::Canvas Graph(std::move(profile), chr::Color({255, 255, 255}, 255), chr::Color({23, 45, 67}, 67), chr::Color({89, 45, 67}, 45));
    Graph.Show();
}