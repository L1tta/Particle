#include "GPUParticle.h"
//why were my pdb files not loading in my other projects like oh my god

int main()
{
	GPUParticle app;


	if (app.startup() == false)
	{
		return -1;
	}

	while (app.update() == true)
	{
		app.draw();
	}

	app.shutdown();

	return 0;
}