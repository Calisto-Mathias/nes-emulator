#include "Controller.h"

namespace sn
{
    Byte Controller::read()
    {
        Byte ret;
        if (m_strobe) {
            ret = sf::Keyboard::isKeyPressed(m_keyBindings[A]);
        } else {
            ret = (m_keyStates & 1);
            m_keyStates >>= 1;
        }
        return ret | 0x40;
    }

    void Controller::setKeyBindings(const std::vector<sf::Keyboard::Key>& keys)
    {
        m_keyBindings = keys;
    }
    
    Controller::Controller() 
        : m_strobe(false),
          m_keyStates(0),
          m_keyBindings(TotalButtons)
    {
//         m_keyBindings[A] = sf::Keyboard::J;
//         m_keyBindings[B] = sf::Keyboard::K;
//         m_keyBindings[Select] = sf::Keyboard::RShift;
//         m_keyBindings[Start] = sf::Keyboard::Return;
//         m_keyBindings[Up] = sf::Keyboard::W;
//         m_keyBindings[Down] = sf::Keyboard::S;
//         m_keyBindings[Left] = sf::Keyboard::A;
//         m_keyBindings[Right] = sf::Keyboard::D;
    }
    
    void Controller::strobe(Byte b)
    {
        m_strobe = (b & 1);
        
        if (!m_strobe) {
            m_keyStates = 0;
            for (int button = A, shift = 0; button < TotalButtons; ++button, ++shift) {
                if (sf::Keyboard::isKeyPressed(m_keyBindings[static_cast<Buttons>(button)])) {
                    m_keyStates |= (1 << shift);
                }
            }
        }
    }
}