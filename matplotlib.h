#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iterator>
#include <cmath>
namespace matplotlib{
    struct Screen{
        enum Size{
            Width = 1200, Height = 600
        };
    };
    struct Flag{
        enum Style{
            Solid = 1 << 0, Dashed = 1 << 1, Dotted = 1 << 2
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
            if(title_.isEmpty()){
                throw std::invalid_argument("Title is absented!");
            }
            if(sizes_.x == 0 || sizes_.y == 0){
                throw std::invalid_argument("Invalid sizes for profile!");
            }
        }
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
        std::vector<std::vector<double>> x_, y_;
        std::vector<Color> color_;
        sf::Font font_;
        sf::String title_;
        std::vector<unsigned int> linestyle_;
        const float space_ = 70;
        const unsigned int fontsize_ = 18;
        const float extra_space = 20;
        bool title_enabled = false;
        sf::Text text_;
        sf::View view_;
        std::vector<std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator>> it_x, it_y;
        void Solid(const std::vector<sf::Vertex>& c, const Color& color){
            const float spacing = 0.5;
            const float R = 1;
            for(std::size_t i = 1; i < c.size(); ++i){
                const sf::Vector2f& iu = c[i - 1].position, iv = c[i].position;
                sf::Vector2f div = iv - iu;
                float segment_len = std::sqrt(div.x * div.x + div.y * div.y);
                div /= segment_len;
                for(float s = 0.0f; s < segment_len; s += spacing){
                    sf::Vector2f dx = iu + div * s;
                    sf::CircleShape dot(R);
                    dot.setPosition(dx);
                    dot.setFillColor(color.Data());
                    profile_->Profile().draw(dot);
                }
            }
        }
        void Dashed(const std::vector<sf::Vertex>& c, const Color& color){
            const float R = 1;
            const float dash_len = 10;
            const float gap_len = 5;
            for(std::size_t i = 1; i < c.size(); ++i){
                const sf::Vector2f& iu = c[i - 1].position, iv = c[i].position;
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
                        for(float s = 0; s < current_step; s += 0.5) {
                            sf::Vector2f pos = iu + div * (drawn + s);
                            sf::CircleShape dot(R);
                            dot.setPosition(pos.x - R, pos.y - R);
                            dot.setFillColor(color.Data());
                            profile_->Profile().draw(dot);
                        }
                    }
                    drawn += current_step;
                    dash_drawn = !dash_drawn;
                }
            }
        }
        void Dotted(const std::vector<sf::Vertex>& c, const Color& color){
            const float spacing = 7;
            const float R = 2;
            for(std::size_t i = 1; i < c.size(); ++i){
                const sf::Vector2f& iu = c[i - 1].position, iv = c[i].position;
                sf::Vector2f div = iv - iu;
                float segment_len = std::sqrt(div.x * div.x + div.y * div.y);
                div /= segment_len;
                for(float s = 0.0f; s < segment_len; s += spacing){
                    sf::Vector2f dx = iu + div * s;
                    sf::CircleShape dot(R);
                    dot.setPosition(dx);
                    dot.setFillColor(color.Data());
                    profile_->Profile().draw(dot);
                }
            }
        }
        void Grid(){
            for(int v = space_; v <= profile_->Data().x - space_; v += (profile_->Data().x - 2 * space_)/ 10){
                sf::Vertex vl[] = {sf::Vertex(sf::Vector2f(v, space_), grid_.Data()), sf::Vertex(sf::Vector2f(v, (profile_->Data().y - space_)), grid_.Data())};
                profile_->Profile().draw(vl, 2, sf::LineStrip);
                // float x_value = *it_x[0].first + ((v - space_) / (profile_->Data().x - 2 * space_)) * (*it_x[0].second - *it_x[0].first);
                // sf::Text x_label(std::to_string(x_value).substr(0, 4), font_, fontsize_ - 4);
                // x_label.setFillColor(axes_.Data());
                // x_label.setPosition(v - 10, profile_->Data().y - space_ + 5);
                // profile_->Profile().draw(x_label);
            }
            for(int h = space_; h <= profile_->Data().y - space_; h += (profile_->Data().y - 2 * space_)/ 10){
                sf::Vertex hl[] = {sf::Vertex(sf::Vector2f(space_, h), grid_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x - space_, h), grid_.Data())};
                profile_->Profile().draw(hl, 2, sf::LineStrip);
                // float y_value = *it_y[0].second - ((h - space_) / (profile_->Data().y - 2 * space_)) * (*it_y[0].second - *it_y[0].first);
                // sf::Text y_label(std::to_string(y_value).substr(0, 4), font_, fontsize_ - 4);
                // y_label.setFillColor(axes_.Data());
                // y_label.setPosition(space_ - 35, h - 10);
                // profile_->Profile().draw(y_label);
            }
        }
        void Axes(){
            int x = space_;
            int y = profile_->Data().y - space_;
            sf::Vertex xl[] = {sf::Vertex(sf::Vector2f(space_, y), axes_.Data()), sf::Vertex(sf::Vector2f(profile_->Data().x - space_, y), axes_.Data())};
            sf::Vertex yl[] = {sf::Vertex(sf::Vector2f(x, space_), axes_.Data()), sf::Vertex(sf::Vector2f(x, profile_->Data().y - space_), axes_.Data())};
            profile_->Profile().draw(xl, 2, sf::LineStrip), profile_->Profile().draw(yl, 2, sf::LineStrip);
        }
        void InitTitle() {
            if (!font_.loadFromFile("Font/font.ttf")) {
                throw std::runtime_error("Failed to load font for title");
            }
            text_.setFont(font_);
            text_.setCharacterSize(fontsize_);
            text_.setFillColor(Color::Black().Data());
            UpdateTitlePosition();
        }
        void UpdateTitlePosition() {
            if (title_enabled) {
                sf::FloatRect textBounds = text_.getLocalBounds();
                text_.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top);
                text_.setPosition(view_.getCenter().x, space_ / 2);
            }
        }
        void HandleTitleResize(const sf::Event& event) {
            view_.setSize(event.size.width, event.size.height);
            view_.setCenter(event.size.width / 2, event.size.height / 2);
            UpdateTitlePosition();
        }
        void DrawTitle() {
            if (title_enabled) {
                profile_->Profile().setView(view_);
                profile_->Profile().draw(text_);
            }
        }
    public:
        Canvas() : profile_(std::make_unique<RenderProfile>(Screen::Width, Screen::Height, sf::String("As Chartify!"))), fone_(Color::White()), grid_(Color({Color({180, 180, 180}, 200)})), axes_(Color::Black()){
            view_.setSize(profile_->Profile().getSize().x, profile_->Profile().getSize().y);
            view_.setCenter(profile_->Profile().getSize().x / 2, profile_->Profile().getSize().y / 2);
        }
        void ConfigurePlot(const std::vector<std::vector<double>>& x, const std::vector<std::vector<double>>& y, const std::vector<Color>& color, const std::vector<unsigned int>& linestyle){
            if(x.size() != y.size() || x.size() != color.size() || x.size() != linestyle.size() || x.empty() || y.empty()){
                throw std::invalid_argument("Invalid vectors data!");
            }
            for(std::size_t i = 0; i < x.size(); ++i){
                if(x[i].size() != y[i].size() || x[i].size() <= 2 || y[i].size() <= 2){
                    throw std::invalid_argument("Invalid vectors data!");
                }
                x_.push_back(x[i]), y_.push_back(y[i]);
            }
            color_ = color;
            linestyle_ = linestyle;
        }
        void Plot(){
            profile_->Profile().clear(fone_.Data());
            for(std::size_t i = 0; i < x_.size(); ++i){
                it_x.push_back(std::minmax_element(x_[i].begin(), x_[i].end()));
                it_y.push_back(std::minmax_element(y_[i].begin(), y_[i].end()));
                double u = *it_x[i].second - *it_x[i].first;
                double v = *it_y[i].second - *it_y[i].first;
                if(u == 0){u = 1;}
                if(v == 0){v = 1;}
                std::vector<sf::Vertex> c;
                for(std::size_t l = 0; l < x_[i].size(); ++l){
                    float scr_x = space_ + extra_space + ((x_[i][l] - *it_x[i].first) / (u)) * (profile_->Data().x - 2 * space_ - extra_space * 2);
                    float scr_y = space_ + extra_space + (profile_->Data().y - 2 * space_ - 2 * extra_space) * (1 - ((y_[i][l] - *it_y[i].first) / (v)));
                    c.emplace_back(sf::Vector2f(scr_x, scr_y), color_[i].Data());
                }
                switch(linestyle_[i]){
                    case Flag::Solid: Solid(c, color_[i]);
                    break;
                    case Flag::Dashed: Dashed(c, color_[i]);
                    break;
                    case Flag::Dotted: Dotted(c, color_[i]);
                    break;
                }
            }
            Grid();
            Axes();
            DrawTitle();
            profile_->Profile().display();
        }
        void Title(const sf::String& title){
            title_ = title;
            title_enabled = true;
            text_.setString(title_);
            InitTitle();
        }
        void Show() {
            sf::RenderWindow& s = profile_->Profile();
            while(s.isOpen()){
                sf::Event event;
                while(s.pollEvent(event)){
                    switch(event.type){
                        case sf::Event::Closed:
                            s.close();
                            break;
                        case sf::Event::Resized:
                            HandleTitleResize(event);
                            break;
                    }
                }
                Plot();
            }
        }
        virtual ~Canvas() = default;
    };
};