#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <SFML/Window.hpp>
#include <cstdint>
#include <vector>

namespace sn
{
    using Byte = std::uint8_t;
    
    class Controller
    {
    private:
        bool m_strobe;
        unsigned int m_keyStates;
        std::vector<sf::Keyboard::Key> m_keyBindings;

    public:
        Controller();
        
        Byte read();
        void strobe(Byte b);
        void setKeyBindings(const std::vector<sf::Keyboard::Key>& keys);
        
        enum Buttons
        {
            Left,
            Right,
            Up,
            Down,
            A,
            B,
            Select,
            Start,
            TotalButtons,
        };
    };
}

#endif // CONTROLLER_H
