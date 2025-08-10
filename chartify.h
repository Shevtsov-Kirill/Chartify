#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
namespace Chartify{
    struct Default{
        static inline unsigned int WIDTH = 1000, HEIGHT = 500;
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
                if(color[i] > 255 || color[i] < 0){
                    throw std::invalid_argument("Elements don't match vector RGB list!");
                }
            }
            if(alpha > 255 || alpha < 0){
                throw std::invalid_argument("Alpha must be value from 0 to 255!");
            }
        }
        static std::vector<uint8_t> White() {
            return std::vector<uint8_t>{255, 255, 255};
        }
        static std::vector<uint8_t> Black() {
            return std::vector<uint8_t>{0, 0, 0};
        }
        const sf::Color& Data() const {return color_;}
        const uint8_t& Alpha() const {return alpha_;}
        virtual ~Color() = default;
    };
    class RenderProfile{
        sf::String title_;
        sf::Vector2u sizes_;
        sf::RenderWindow profile_;
    public:
        RenderProfile(unsigned int width, unsigned int height, sf::String& title) : title_(title), sizes_(width, height), profile_(sf::VideoMode(sizes_.x, sizes_.y), title_){
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
        sf::Vector2u& ActualData(){return sizes_;}
        sf::RenderWindow& Profile(){return profile_;}
        virtual ~RenderProfile() = default;
    };
    class Canvas{
        std::vector<double> u_, v_;
        std::unique_ptr<RenderProfile> profile_;
        Color fone_, grid_, axes_;
        unsigned int flag_;
    public:
        Canvas(std::unique_ptr<RenderProfile> profile, Color fone, Color grid, Color axes, unsigned int flag = Flag::GRID | Flag::AXES) :
        profile_(std::move(profile)), fone_(fone), grid_(grid), axes_(axes), flag_(flag){}
        void Render(){
            sf::RenderWindow& s = profile_->Profile();
            s.clear(fone_.Data());
            s.display();
        }
        void Show() {
            while(profile_->Profile().isOpen()){
                sf::Event event;
                while(profile_->Profile().pollEvent(event)){
                    if(event.type == sf::Event::Closed){
                        profile_->Profile().close();
                    } else if(event.type == sf::Event::Resized){
                        unsigned int o_width = std::max(10u, event.size.width), o_height = std::max(10u, event.size.height);
                        profile_->UpdateSizesData(o_width, o_height);
                        sf::View f(sf::FloatRect(0, 0, o_width, o_height));
                        profile_->Profile().setView(f);
                    }
                }
                profile_->Profile().clear(fone_.Data());
                profile_->Profile().display();
            }
            return;
        }
        virtual ~Canvas() = default;
    };
};