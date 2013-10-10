#include "inputadaptor.h"
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

InputAdaptor::InputAdaptor(sf::Window& sfw)
    :   inputHandler(new fea::util::SFMLInputBackend(sfw)),
        sfWindow(sfw)
{
}
