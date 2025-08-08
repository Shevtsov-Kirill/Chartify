#include "chartify.h"
int main(){
    sf::String title = "My Graph";
    auto profile = std::make_unique<Chartify::RenderProfile>(960, 480, title);
    Chartify::Color back({255, 255, 255}, 255), grid({230, 230, 230}, 255), axes({40, 40, 40}, 255);
    Chartify::Canvas Graph(std::move(profile), back, grid, axes, Chartify::Flag::USE_GRID | Chartify::Flag::ZERO_XLINE | Chartify::Flag::ZERO_YLINE);
    Graph.Render();
    Graph.Show();
}
