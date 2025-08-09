#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
namespace Chartify{
    struct Default{
        static inline int WIDTH = 960, HEIGHT = 480;
    };
    enum Flag{
        GRID = 1 << 0, AXES = 1 << 1, CURVE = 1 << 2
    };
    class Color{
        uint8_t alpha_;
        sf::Color color_;
    public:
        Color() = default;
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
        sf::Vector2u sizes_;
    public:
        RenderProfile(const int& width, const int& height, sf::String& title) : width_(width), height_(height), title_(title), profile_(sf::VideoMode(width_, height_), title), sizes_(width_, height_){
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
        RenderProfile() : width_(Default::WIDTH), height_(Default::HEIGHT), profile_(sf::VideoMode(Default::WIDTH, Default::HEIGHT), sf::String("Chartify Graph!")){}
        
        RenderProfile(const RenderProfile&) = delete;
        RenderProfile& operator=(const RenderProfile&) = delete;
        RenderProfile(RenderProfile&&) = default;
        RenderProfile& operator=(RenderProfile&&) = default;

        void UpdateSizeWindow(std::size_t width, std::size_t height){
            sizes_.x = width, sizes_.y = height;
            profile_.setSize(sizes_);
            return;
        }
        void UpdateAxesPosition(){}
        //
        const sf::Vector2u& ActualSizes() const {return sizes_;}
        const std::size_t Width() const {return width_;}
        const std::size_t Height() const {return height_;}
        sf::RenderWindow& Profile() {return profile_;}
        virtual ~RenderProfile() = default;
    };
    class Canvas{
        std::vector<double> x_, y_;
        std::unique_ptr<RenderProfile> profile_;
        Color fone_, grid_, axes_;
        unsigned int flags_;
    public:
        Canvas(std::unique_ptr<RenderProfile> profile, const Color& fone, const Color& grid, const Color& axes, unsigned int flags) : profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flags_(flags){}
        void Plot(const std::vector<double>& x, const std::vector<double>& y, const Color& color = Color({0, 0, 255}, 255)){
            if((x.size() != y.size()) || (x.empty() || y.empty())){
                throw std::invalid_argument("Invalid data values!");
            }
            x_.resize(x.size()), y_.resize(x.size());
            x_ = x;
            y_ = y;
            auto [min_x, max_x] = std::minmax_element(x.begin(), x.end());
            auto [min_y, max_y] = std::minmax_element(y.begin(), y.end());
            float omega1 = *max_x - *min_x, omega2 = *max_y - *min_y;
            if(omega1 == 0 || omega2 == 0){
                omega1 = omega2 = float(1);
            }
            std::vector<sf::Vertex> curve;
            for(std::size_t i = 0; i < x.size(); ++i){
                float scr1 = static_cast<float>((x[i] - *min_x) / (omega1) * profile_->ActualSizes().x);
                float scr2 = static_cast<float>(profile_->ActualSizes().y - ((y[i] - *min_y) / (omega2)) * profile_->ActualSizes().y);
                curve.emplace_back(sf::Vector2f(scr1, scr2), color.Data());
            }
            sf::RenderWindow& s = profile_->Profile();
            s.clear(fone_.Data());
            if(flags_ & Flag::GRID){
                sf::Color grid_c = grid_.Data();
                for(int x = 0; x < profile_->ActualSizes().x; x += profile_->ActualSizes().x / 48){
                    sf::Vertex vertical[] = {sf::Vertex(sf::Vector2f(float(x), 0), grid_c), sf::Vertex(sf::Vector2f(float(x), float(profile_->ActualSizes().y)), grid_c)};
                    s.draw(vertical, 2, sf::Lines);
                }
                for(int y = 0; y < profile_->ActualSizes().y; y += profile_->ActualSizes().y / 48){
                    sf::Vertex horizontal[] = {sf::Vertex(sf::Vector2f(0, float(y)), grid_c), sf::Vertex(sf::Vector2f(profile_->ActualSizes().x, float(y)), grid_c)};
                    s.draw(horizontal, 2, sf::Lines);
                }
            }
            if(flags_ & Flag::AXES){
                sf::Color line_c = axes_.Data();
                int y = profile_->ActualSizes().y / 2, x = profile_->ActualSizes().x / 2;

                sf::Vertex xline[] = {sf::Vertex(sf::Vector2f(0, float(y)), line_c), sf::Vertex(sf::Vector2f(float(profile_->ActualSizes().x), float(y)), line_c)};
                s.draw(xline, 2, sf::Lines);

                sf::Vertex yline[] = {sf::Vertex(sf::Vector2f(float(x), 0), line_c), sf::Vertex(sf::Vector2f(float(x), float(profile_->ActualSizes().y)), line_c)};
                s.draw(yline, 2, sf::Lines);
            }
            if(flags_ & Flag::CURVE){
                s.draw(curve.data(), curve.size(), sf::LineStrip);
            }
            s.display();
            return;
        }
        void Show() {
            while (profile_->Profile().isOpen()) {
                sf::Event event;
                while (profile_->Profile().pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        profile_->Profile().close();
                    } else if (event.type == sf::Event::Resized){
                        profile_->UpdateSizeWindow(event.size.width, event.size.height);
                        Plot(x_, y_);
                    }
                }
            }
            return;
        }
        virtual ~Canvas() = default;
    };
};