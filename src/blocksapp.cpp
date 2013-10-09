#include "blocksapp.h"
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

BlocksApplication::BlocksApplication() : window(new fea::util::SFMLWindowBackend(sfWindow)), inputHandler(new fea::util::SFMLInputBackend(sfWindow)), world(bus), renderer(bus)
{
	movingUp = movingLeft = movingRight = movingDown = false;
}

void BlocksApplication::setup()
{
    window.create(fea::VideoMode(800, 600, 32), "Blocky");

    renderer.setup();

    world.initialise();
}

void BlocksApplication::loop()
{
    fea::Event event;

    inputHandler.processEvents();

    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
            quit();

		if(event.type == fea::Event::MOUSEMOVED)
		{
			float newX = (float)event.mouseMove.x;
			float newY = (float)event.mouseMove.y;
			float mspeed = 0.01f;
			cam.AddDirection((newX - lastX)  * mspeed,(newY - lastY) * mspeed);
			lastX = event.mouseMove.x;
			lastY = event.mouseMove.y;
		}
        if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::ESCAPE)
                quit();
            switch(event.key.code)
            {
				case fea::Keyboard::A:
					movingLeft = true;
				break;
				case fea::Keyboard::D:
					movingRight = true;
				break;
				case fea::Keyboard::W:
					movingUp = true;
				break;
				case fea::Keyboard::S:
					movingDown = true;
				break;
            }
        }else if(event.type == fea::Event::KEYRELEASED){
            switch(event.key.code)
            {
				case fea::Keyboard::A:
					movingLeft = false;
				break;
				case fea::Keyboard::D:
					movingRight = false;
				break;
				case fea::Keyboard::W:
					movingUp = false;
				break;
				case fea::Keyboard::S:
					movingDown = false;
				break;
            }
		}
    }
	glm::vec3 m;
	if(movingRight){
		m.x += moveSpeed;	
	}
	if(movingLeft){
		m.x -= moveSpeed;	
	}
	if(movingUp){
		m.z += moveSpeed;	
	}
	if(movingDown){
		m.z -= moveSpeed;	
	}
	camSpeed += m;

	cam.MoveForward(camSpeed.z);
	cam.Strafe(camSpeed.x);	
	cam.Update();
	camSpeed *= 0.995f;

	renderer.setCameraMatrix(cam.GetMatrix());

    world.update();
    renderer.render();
    window.swapBuffers();
}

void BlocksApplication::destroy()
{
    window.close();
}
