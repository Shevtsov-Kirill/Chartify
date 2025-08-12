#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iterator>
#include <cmath>
namespace Chartify{
    struct Default{
        static inline unsigned int WIDTH = 1000, HEIGHT = 500;
    };
    enum Flag{
        GRID = 1 << 0, AXES = 1 << 1, SOLID = 1 << 2, DASHED = 1 << 3, DOTTED = 1 << 4
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
                if(color[i] > 255 || color[i] < 0){
                    throw std::invalid_argument("Elements don't match vector RGB list!");
                }
            }
            if(alpha > 255 || alpha < 0){
                throw std::invalid_argument("Alpha must be value from 0 to 255!");
            }
        }
        Color(const Color& o) : alpha_(o.alpha_), color_(o.color_){}
        Color& operator=(const Color& o){
            if(this != &o){
                alpha_ = o.alpha_;
                color_ = o.color_;
            }
            return *this;
        }
        static Color White(){
            return Color({255, 255, 255}, 255);
        }
        static Color Black(){
            return Color({0, 0, 0}, 255);
        }
        static Color Blue(){
            return Color({0, 0, 255}, 255);
        }
        const sf::Color& Data() const {return color_;}
        const uint8_t& Alpha() const {return alpha_;}
        virtual ~Color() = default;
    };
    class Line{
        Color color_;
        uint8_t alpha_;
    public:
        Line() : color_(Color::Blue()), alpha_(255){}
        Line(const Color& color, const uint8_t& alpha) : color_(color), alpha_(alpha){
            if(alpha_ < 0 || alpha_ > 255){
                throw std::invalid_argument("Invalid alpha value!");
            }
        }
        const Color& Data() const {return color_;}
    };
    class RenderProfile{
        sf::String title_;
        sf::Vector2u sizes_;
        sf::RenderWindow profile_;
    public:
        RenderProfile(unsigned int width, unsigned int height, sf::String& title) : title_(title), sizes_(width, height), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){
            if(width < 960 || height < 480){
                throw std::invalid_argument("Small scale!");
            }
            if(title_.isEmpty()){
                throw std::invalid_argument("Title is absented!");
            }
            if(sizes_.x == 0 || sizes_.y == 0){
                throw std::invalid_argument("Invalid sizes for profile!");
            }
        }
        RenderProfile() : title_(sf::String("Chartify Graph!")), sizes_(Default::WIDTH, Default::HEIGHT), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){}
        RenderProfile(const RenderProfile&) = delete;
        RenderProfile& operator=(const RenderProfile&) = delete;
        RenderProfile(RenderProfile&&) = default;
        RenderProfile& operator=(RenderProfile&&) = default;
        void UpdateSizesData(unsigned int width, unsigned int height){
            if(width == 0 || height == 0){
                throw std::invalid_argument("Invalid size values!");
            }
            sizes_.x = width, sizes_.y = height;
            profile_.setSize(sizes_);
            return;
        }
        const sf::String& Title() const {return title_;}
        const sf::Vector2u& Data() const {return sizes_;}
        sf::RenderWindow& Profile(){return profile_;}
        virtual ~RenderProfile() = default;
    };
    class Canvas{
        std::vector<double> x_, y_;
        std::unique_ptr<RenderProfile> profile_;
        Color fone_, grid_, axes_, plot_;
        unsigned int flag_, linestyle_;
    public:
        Canvas(std::unique_ptr<RenderProfile> profile, Color fone, Color grid, Color axes, unsigned int flag) : profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flag_(flag){}
        void Plot(const std::vector<double>& x, const std::vector<double>& y, const Color& color, unsigned int linestyle = Flag::DOTTED) {
            plot_ = color;
            linestyle_ = linestyle;
            x_.resize(x.size()), y_.resize(y.size());
            x_ = x;
            y_ = y;
            if(x.size() != y.size() || x.empty() || y.empty()){
                throw std::invalid_argument("Invalid data vectors for plot function!");
            }
            std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator> rg_x, rg_y;
            rg_x = std::minmax_element(x_.begin(), x_.end());
            rg_y = std::minmax_element(y_.begin(), y_.end());
            std::pair<double, double> omega;
            omega.first = *rg_x.second - *rg_x.first, omega.second = *rg_y.second - *rg_y.first;
            std::vector<sf::Vertex> plot;
            for(std::size_t i = 0; i < x.size(); ++i){
                float scr_x = ((x[i] - *rg_x.first) / (omega.first)) * profile_->Data().x;
                float scr_y = profile_->Data().y - ((y[i] - *rg_y.first) / (omega.second)) * profile_->Data().y;
                plot.emplace_back(sf::Vector2f(scr_x, scr_y), color.Data());
            }
            profile_->Profile().clear(fone_.Data());
            if(flag_ & Flag::GRID){
                for(int v = 0; v < profile_->Data().x; v += profile_->Data().x / 10){
                    sf::Vertex vl[] = {sf::Vertex(sf::Vector2f(v, 0), grid_.Data()), sf::Vertex(sf::Vector2f(v, profile_->Data().y), grid_.Data())};
                    profile_->Profile().draw(vl, 2, sf::LineStrip);
                }
                for(int h = 0; h < profile_->Data().y; h += profile_->Data().y / 10){
                    sf::Vertex hl[] = {sf::Vertex(sf::Vector2f(0, h), grid_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x, h), grid_.Data())};
                    profile_->Profile().draw(hl, 2, sf::LineStrip);
                }
            }
            if(flag_ & Flag::AXES){
                int x = profile_->Data().x / 2, y = profile_->Data().y / 2;
                sf::Vertex xl[] = {sf::Vertex(sf::Vector2f(0, y), axes_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x, y), axes_.Data())};
                sf::Vertex yl[] = {sf::Vertex(sf::Vector2f(x, 0), axes_.Data()), sf::Vertex(sf::Vector2f(x, profile_->Data().y), axes_.Data())};
                profile_->Profile().draw(xl, 2, sf::LineStrip), profile_->Profile().draw(yl, 2, sf::LineStrip);
            }
            switch(linestyle){
                case Flag::SOLID:{
                    profile_->Profile().draw(plot.data(), plot.size(), sf::LineStrip);
                }
                break;
                case Flag::DASHED:{
                    const float dash_len = 10, gap_len = 5;
                    for(std::size_t i = 1; i < plot.size(); ++i){
                        const sf::Vector2f& iu = plot[i - 1].position, iv = plot[i].position;
                        sf::Vector2f div = iv - iu;
                        float segment_len = std::sqrt(div.x * div.x + div.y * div.y);
                        div /= segment_len;
                        float drawn = 0;
                        bool dash_drawn = true;
                        while(drawn < segment_len) {
                            float step = dash_drawn ? dash_len : gap_len;
                            float current_step = std::min(step, segment_len - drawn);
                            sf::Vector2f start = iu + div * drawn;
                            sf::Vector2f end = iu + div * (drawn + current_step);
                            
                            if(dash_drawn) {
                                sf::Vertex dash[] = {sf::Vertex(start, color.Data()), sf::Vertex(end, color.Data())};
                                profile_->Profile().draw(dash, 2, sf::Lines);
                            }
                            
                            drawn += current_step;
                            dash_drawn = !dash_drawn;
                        }
                    }
                }
                break;
                case Flag::DOTTED:{
                    const float spacing = 10;
                    const float R = 2.5;
                    for(std::size_t i = 1; i < plot.size(); ++i){
                        const sf::Vector2f& iu = plot[i - 1].position, iv = plot[i].position;
                        sf::Vector2f div = iv - iu;
                        float segment_len = std::sqrt(div.x * div.x + div.y * div.y);
                        div /= segment_len;
                        for(float s = 0.0f; s < segment_len; s += spacing){
                            sf::Vector2f dot_position = iu + div * s;
                            sf::CircleShape dot(R);
                            dot.setPosition(dot_position);
                            dot.setFillColor(color.Data());
                            profile_->Profile().draw(dot);
                        }
                    }
                }
                break;
            }
            profile_->Profile().display();
        }
        void Show(){
            sf::RenderWindow& s = profile_->Profile();
            while(s.isOpen()){
                sf::Event event;
                while(s.pollEvent(event)){
                    switch(event.type){
                        case sf::Event::Closed:
                            s.close();
                            break;
                    }
                }
                Plot(x_, y_, plot_, linestyle_);
            }
        }
        virtual ~Canvas() = default;
    };
};