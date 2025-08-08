#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
namespace Chartify{
    struct Constant{
        static constexpr int WIDTH = 960, HEIGHT = 480;
        static constexpr int GRID_PERIOD = 20;
    };
    enum Flag{
        USE_GRID = 1 << 0, ZERO_XLINE = 1 << 1, ZERO_YLINE = 1 << 2
    };
    class Color{
        uint8_t alpha_;
        sf::Color color_;
    public:
        Color(const std::vector<uint8_t>& color, const uint8_t& alpha) : alpha_(alpha), color_(color[0], color[1], color[2], alpha_){
            if(color.size() != 3){
                throw std::invalid_argument("RGB vector must have len = 3!");
            }
            for(std::size_t i = 0; i < color.size(); ++i){
                if(color[i] > uint8_t(255) || color[i] < uint8_t(0)){
                    throw std::invalid_argument("Elements don't match vector RGB list!");
                }
            }
            if(alpha > uint8_t(255) || alpha < uint8_t(0)){
                throw std::invalid_argument("Alpha must be value from 0 to 255!");
            }
        }
        static std::vector<uint8_t> White() {
            return {uint8_t(255), uint8_t(255), uint8_t(255)};
        }
        static std::vector<uint8_t> Black() {
            return {uint8_t(0), uint8_t(0), uint8_t(0)};
        }
        const sf::Color Data() const {return color_;}
        const uint8_t Alpha() const {return alpha_;}
        virtual ~Color() = default;
    };
    class Line{
        uint8_t alpha_;
        Color color_;
    public:
        Line(const Color& color) : color_(color), alpha_(color.Alpha()){}
        Line() : alpha_(uint8_t(1)), color_(Color::White(), alpha_){}
        const Color Line_Color() const {return color_;}
        virtual ~Line() = default;
    };
    class RenderProfile{
        std::size_t width_, height_;
        sf::String title_;
        sf::RenderWindow profile_;
    public:
        RenderProfile(const int& width, const int& height, sf::String& title) : width_(width), height_(height), title_(title), profile_(sf::VideoMode(width_, height_), title){
            if(title.isEmpty()){
                throw std::invalid_argument("Title of graph is empty!");
            }
            if(width_ < 0 || height_ < 0){
                const std::string whitespace = " ";
                std::string l = "Invalid Render profile parameter(s):";
                if(width_ < 0){
                    l += whitespace + "Width";
                }
                if(height_ < 0){
                    l += whitespace + "Height";
                }
                throw std::invalid_argument(l);
            }
        }
        RenderProfile() : width_(Constant::WIDTH), height_(Constant::HEIGHT), profile_(sf::VideoMode(Constant::WIDTH, Constant::HEIGHT), sf::String("Chartify Graph!")){}
        
        RenderProfile(const RenderProfile&) = delete;
        RenderProfile& operator=(const RenderProfile&) = delete;
        RenderProfile(RenderProfile&&) = default;
        RenderProfile& operator=(RenderProfile&&) = default;

        const std::size_t Width() const {return width_;}
        const std::size_t Height() const {return height_;}
        const sf::String Title() const {return title_;}
        sf::RenderWindow& Profile() {return profile_;}
        virtual ~RenderProfile() = default;
    };
    class Canvas{
        std::unique_ptr<RenderProfile> profile_;
        Color fone_, grid_, axes_;
        unsigned int flags_;
    public:
        Canvas(std::unique_ptr<RenderProfile> profile, const Color& fone, const Color& grid, const Color& axes, unsigned int flags) : profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flags_(flags){}
        void Render(){
            sf::RenderWindow& s = profile_->Profile();
            s.clear(fone_.Data());
            if(flags_ & Flag::USE_GRID){
                sf::Color grid_c = grid_.Data();
                for(int x = 0; x < profile_->Width(); x += Constant::GRID_PERIOD){
                sf::Vertex vertical[] = {sf::Vertex(sf::Vector2f(float(x), 0), grid_c), sf::Vertex(sf::Vector2f(float(x), float(profile_->Height())), grid_c)};
                s.draw(vertical, 2, sf::Lines);
                }
                for(int y = 0; y < profile_->Height(); y += Constant::GRID_PERIOD){
                    sf::Vertex horizontal[] = {sf::Vertex(sf::Vector2f(0, float(y)), grid_c), sf::Vertex(sf::Vector2f(profile_->Width(), float(y)), grid_c)};
                    s.draw(horizontal, 2, sf::Lines);
                }
            }
            sf::Color line_c = axes_.Data();
            if(flags_ & Flag::ZERO_XLINE){
                std::size_t y = profile_->Height() / 2;
                sf::Vertex line[] = {sf::Vertex(sf::Vector2f(0, float(y)), line_c), sf::Vertex(sf::Vector2f(float(profile_->Width()), float(y)), line_c)};
                s.draw(line, 2, sf::Lines);
            }
            if(flags_ & Flag::ZERO_YLINE){
                int x_zero = profile_->Width() / 2;
                sf::Vertex line[] = {sf::Vertex(sf::Vector2f(float(x_zero), 0), line_c), sf::Vertex(sf::Vector2f(float(x_zero), float(profile_->Height())), line_c)};
                s.draw(line, 2, sf::Lines);
            }
            return;
        }
        void Show() {
            while (profile_->Profile().isOpen()) {
                sf::Event event;
                while (profile_->Profile().pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        profile_->Profile().close();
                    }
                }
                profile_->Profile().clear();
                Render();
                profile_->Profile().display();
            }
            return;
        }
        virtual ~Canvas() = default;
    };
};