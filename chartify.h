#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iterator>
#include <cmath>
namespace Chartify{
    struct Screen{
        static inline unsigned int WIDTH = 1000, HEIGHT = 500;
    };
    struct Flag{
        enum Style{
            Grid = 1 << 0, Axes = 1 << 1, Solid = 1 << 2, Dashed = 1 << 3, Dotted = 1 << 4
        };
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
        RenderProfile() : title_(sf::String("Chartify Graph!")), sizes_(Screen::WIDTH, Screen::HEIGHT), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){}
        RenderProfile(const RenderProfile&) = delete;
        RenderProfile& operator=(const RenderProfile&) = delete;
        RenderProfile(RenderProfile&&) = default;
        RenderProfile& operator=(RenderProfile&&) = default;
        const sf::String& Title() const {return title_;}
        const sf::Vector2u& Data() const {return sizes_;}
        sf::RenderWindow& Profile(){return profile_;}
        virtual ~RenderProfile() = default;
    };
    class Canvas{
        std::unique_ptr<RenderProfile> profile_;
        Color fone_, grid_, axes_;
        unsigned int flag_;
        std::vector<std::vector<double>> x_, y_;
        std::vector<Color> color_;
        std::vector<unsigned int> linestyle_;
    public:
        Canvas(std::unique_ptr<RenderProfile> profile, Color fone, Color grid, Color axes, unsigned int flag) : profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flag_(flag){}
        void ConfigurePlot(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y, const std::vector<Color>& color, const std::vector<unsigned int>& linestyle){
            if(x.size() != y.size() || x.size() != color.size() || x.size() != linestyle.size()){
                throw std::invalid_argument("Invalid vectors data");
            }
            for(std::size_t i = 0; i < x.size(); ++i){
                if(x[i].size() != y[i].size()){
                    throw std::invalid_argument("Invalid vectors data");
                }
                x_.push_back(x[i]), y_.push_back(y[i]);
            }
            color_ = color, linestyle_ = linestyle;
        }
        void Plot() const {
            std::vector<std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator>> it_x, it_y;
            profile_->Profile().clear(fone_.Data());
            for(std::size_t i = 0; i < x_.size(); ++i){
                it_x.push_back(std::minmax_element(x_[i].begin(), x_[i].end()));
                it_y.push_back(std::minmax_element(y_[i].begin(), y_[i].end()));
                double u = *it_x[i].second - *it_x[i].first, v = *it_y[i].second - *it_y[i].first;
                if(u == 0 || v == 0){
                    u = 1, v = 1;
                }
                std::vector<sf::Vertex> chart;
                for(std::size_t l = 0; l < x_[i].size(); ++l){
                    float scr_x = ((x_[i][l] - *it_x[i].first) / (u)) * profile_->Data().x;
                    float scr_y = (profile_->Data().y) - ((y_[i][l] - *it_y[i].first) / (v)) * (profile_->Data().y);
                    chart.emplace_back(sf::Vector2f(scr_x, scr_y), color_[i].Data());
                }
                switch(linestyle_[i]){
                    case Flag::Solid:{
                        profile_->Profile().draw(chart.data(), chart.size(), sf::LineStrip);
                    }
                    break;
                    case Flag::Dashed:{
                        const float dash_len = 10.0f, gap_len = 5.0f;
                        for(std::size_t k = 1; k < chart.size(); ++k){
                            const sf::Vector2f& iu = chart[k - 1].position, iv = chart[k].position;
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
                                    sf::Vertex dash[] = {sf::Vertex(start, color_[i].Data()), sf::Vertex(end, color_[i].Data())};
                                    profile_->Profile().draw(dash, 2, sf::Lines);
                                }
                                
                                drawn += current_step;
                                dash_drawn = !dash_drawn;
                            }
                        }
                    }
                    break;
                    case Flag::Dotted:{
                        const float spacing = 8.0f;
                        const float R = 1.5f;
                        for(std::size_t q = 1; q < chart.size(); ++q){
                            const sf::Vector2f& iu = chart[q - 1].position, iv = chart[q].position;
                            sf::Vector2f div = iv - iu;
                            float segment_len = std::sqrt(div.x * div.x + div.y * div.y);
                            div /= segment_len;
                            for(float s = 0.0f; s < segment_len; s += spacing){
                                sf::Vector2f dx = iu + div * s;
                                sf::CircleShape dot(R);
                                dot.setPosition(dx);
                                dot.setFillColor(color_[i].Data());
                                profile_->Profile().draw(dot);
                            }
                        }
                    }
                    break;
                }
            }
            if(flag_ & Flag::Grid){
                for(int v = 0; v < profile_->Data().x; v += profile_->Data().x / 10){
                    sf::Vertex vl[] = {sf::Vertex(sf::Vector2f(v, 0), grid_.Data()), sf::Vertex(sf::Vector2f(v, profile_->Data().y), grid_.Data())};
                    profile_->Profile().draw(vl, 2, sf::LineStrip);
                }
                for(int h = 0; h < profile_->Data().y; h += profile_->Data().y / 10){
                    sf::Vertex hl[] = {sf::Vertex(sf::Vector2f(0, h), grid_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x, h), grid_.Data())};
                    profile_->Profile().draw(hl, 2, sf::LineStrip);
                }
            }
            if(flag_ & Flag::Axes){
                int x = profile_->Data().x / 2, y = profile_->Data().y / 2;
                sf::Vertex xl[] = {sf::Vertex(sf::Vector2f(0, y), axes_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x, y), axes_.Data())};
                sf::Vertex yl[] = {sf::Vertex(sf::Vector2f(x, 0), axes_.Data()), sf::Vertex(sf::Vector2f(x, profile_->Data().y), axes_.Data())};
                profile_->Profile().draw(xl, 2, sf::LineStrip), profile_->Profile().draw(yl, 2, sf::LineStrip);
            }
            profile_->Profile().display();
        }
        void Show() const {
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
                Plot();
            }
        }
        virtual ~Canvas() = default;
    };
};