#include <featherkit/userinterface.h>
#include <SFML/Window.hpp>

class InputAdaptor
{
    public:
        InputAdaptor(sf::Window& sfw);

    private:
        sf::Window& sfWindow;
        fea::InputHandler inputHandler;
        fea::ActionHandler<std::string> actionHandler;
};
