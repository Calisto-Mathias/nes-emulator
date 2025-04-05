#ifndef VIRTUALSCREEN_H
#define VIRTUALSCREEN_H
#include <SFML/Graphics.hpp>

namespace sn
{
    class VirtualScreen : public sf::Drawable
    {
    private:
        float m_pixelSize; // Size of a virtual pixel in real pixels
        sf::VertexArray m_vertices;
        sf::Vector2u m_screenSize;
        
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        
    public:
        void setPixel(std::size_t x, std::size_t y, sf::Color color);
        void create(unsigned int width, unsigned int height, float pixel_size, sf::Color color);
    };
};
#endif // VIRTUALSCREEN_H
