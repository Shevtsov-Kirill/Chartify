#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iterator>
#include <cmath>
namespace Chartify{
    struct Screen{
        enum Size{
            Width = 1200, Height = 500
        };
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
        static Color White(){return Color({255, 255, 255}, 255);}
        static Color Black(){return Color({0, 0, 0}, 255);}
        static Color Blue(){return Color({0, 0, 255}, 255);}
        static Color Red(){return Color({255, 0, 0}, 255);}
        static Color Green(){return Color({0, 255, 0}, 255);}
        static Color Orange(){return Color({255, 165, 0}, 255);}
        static Color Violet(){return Color({148, 0, 211}, 255);}
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
        RenderProfile(unsigned int width, unsigned int height, const sf::String& title) : title_(title), sizes_(width, height), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){
            // if(width < 900 || height < 400){
            //     throw std::invalid_argument("Small scale!");
            // }
            if(title_.isEmpty()){
                throw std::invalid_argument("Title is absented!");
            }
            if(sizes_.x == 0 || sizes_.y == 0){
                throw std::invalid_argument("Invalid sizes for profile!");
            }
        }
        // RenderProfile() : title_(sf::String("As Chartify!")), sizes_(Screen::Width, Screen::Height), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){}
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
        const float space_ = 70.0f;
        const unsigned int fontsize_ = 20;
        sf::Font font_;
        sf::String title_;
    public:
        // Canvas(std::unique_ptr<RenderProfile> profile, Color fone, Color grid, Color axes, unsigned int flag) : profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flag_(flag){}
        Canvas() : profile_(std::make_unique<RenderProfile>(Screen::Width, Screen::Height, sf::String("As Chartify!"))), fone_(Color::White()), grid_(Color({Color({180, 180, 180}, 200)})),
        axes_(Color::Black()), flag_(Flag::Axes | Flag::Grid){}
        void ConfigurePlot(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y, const std::vector<Color>& color, const std::vector<unsigned int>& linestyle){
            if(x.size() != y.size() || x.size() != color.size() || x.size() != linestyle.size() || x.empty() || y.empty()){
                throw std::invalid_argument("Invalid vectors data: sizes are dif, or vectors are empty!");
            }
            for(std::size_t i = 0; i < x.size(); ++i){
                if(x[i].size() != y[i].size() || x[i].size() <= 2 || y[i].size() <= 2){
                    throw std::invalid_argument("Invalid vectors data: to low to create graph or subvector sizes are dif!");
                }
                x_.push_back(x[i]), y_.push_back(y[i]);
            }
            color_ = color;
            linestyle_ = linestyle;
        }
        void Plot() const {
            const float extra_space = 20.0f;
            std::vector<std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator>> it_x, it_y;
            profile_->Profile().clear(fone_.Data());
            for(std::size_t i = 0; i < x_.size(); ++i){
                it_x.push_back(std::minmax_element(x_[i].begin(), x_[i].end()));
                it_y.push_back(std::minmax_element(y_[i].begin(), y_[i].end()));
                double u = *it_x[i].second - *it_x[i].first;
                double v = *it_y[i].second - *it_y[i].first;
                if(u == 0 || v == 0){
                    u = 1;
                    v = 1;
                }
                std::vector<sf::Vertex> chart;
                for(std::size_t l = 0; l < x_[i].size(); ++l){
                    float scr_x = space_ + extra_space + ((x_[i][l] - *it_x[i].first) / (u)) * (profile_->Data().x - 2 * space_ - extra_space * 2);
                    float scr_y = space_ + extra_space + (profile_->Data().y - 2 * space_ - 2 * extra_space) * (1 - ((y_[i][l] - *it_y[i].first) / (v)));
                    chart.emplace_back(sf::Vector2f(scr_x, scr_y), color_[i].Data());
                }
                switch(linestyle_[i]){
                    case Flag::Solid:{
                        const float spacing = 0.5f;
                        const float R = 1.0f;
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
                    case Flag::Dashed:{
                        const float R = 1.0f;
                        const float dash_len = 10.0f;
                        const float gap_len = 5.0f;
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
                                    for(float s = 0; s < current_step; s += 0.5f) {
                                        sf::Vector2f pos = iu + div * (drawn + s);
                                        sf::CircleShape dot(R);
                                        dot.setPosition(pos.x - R, pos.y - R);
                                        dot.setFillColor(color_[i].Data());
                                        profile_->Profile().draw(dot);
                                    }
                                }
                                drawn += current_step;
                                dash_drawn = !dash_drawn;
                            }
                        }
                    }
                    break;
                    case Flag::Dotted:{
                        const float spacing = 7.0f;
                        const float R = 2.0f;
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
                for(int v = space_; v <= profile_->Data().x - space_; v += (profile_->Data().x - 2 * space_)/ 10){
                    sf::Vertex vl[] = {sf::Vertex(sf::Vector2f(v, space_), grid_.Data()), sf::Vertex(sf::Vector2f(v, (profile_->Data().y - space_)), grid_.Data())};
                    profile_->Profile().draw(vl, 2, sf::LineStrip);
                }
                for(int h = space_; h <= profile_->Data().y - space_; h += (profile_->Data().y - 2 * space_)/ 10){
                    sf::Vertex hl[] = {sf::Vertex(sf::Vector2f(space_, h), grid_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x - space_, h), grid_.Data())};
                    profile_->Profile().draw(hl, 2, sf::LineStrip);
                }
            }
            if(flag_ & Flag::Axes){
                const float R = 1.0f;
                const float spacing = 0.5f;
                for(float y = space_; y < profile_->Data().y - space_; y += spacing){
                    sf::CircleShape dx(R);
                    dx.setFillColor(Color::Black().Data());
                    dx.setPosition(space_, y);
                    profile_->Profile().draw(dx);
                }
                for(float x = space_; x < profile_->Data().x - space_; x += spacing){
                    sf::CircleShape dy(R);
                    dy.setFillColor(Color::Black().Data());
                    dy.setPosition(x, profile_->Data().y - space_);
                    profile_->Profile().draw(dy);
                }
            }
            if(!title_.isEmpty()){
                if(title_.getSize() > profile_->Data().x){
                    throw std::invalid_argument("Too low signs to make title!");
                }
                sf::Text text(title_, font_, fontsize_);
                sf::FloatRect text_bound = text.getGlobalBounds();
                float xPos = (profile_->Data().x - text_bound.width) / 2;
                text.setFillColor(Color::Black().Data());
                text.setPosition(xPos, space_ / 2);
                profile_->Profile().draw(text);
            }
            profile_->Profile().display();
        }
        void Title(const sf::String& title) {
            font_.loadFromFile("Font/Ubuntu-Regular.ttf");
            if(!font_.loadFromFile("Font/Ubuntu-Regular.ttf")){
                throw std::invalid_argument("Font didn't been installed!");
            }
            title_ = title;
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