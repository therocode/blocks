#include <featherkit/userinterface.h>
#include <featherkit/messaging.h>
#include <SFML/Window.hpp>

class InputAdaptor
{
    public:
        InputAdaptor(sf::Window& sfw, fea::MessageBus& b);
        void update();

    private:
        sf::Window& sfWindow;
        fea::InputHandler inputHandler;
        fea::ActionHandler<std::string> actionHandler;
        fea::MessageBus bus;
};
