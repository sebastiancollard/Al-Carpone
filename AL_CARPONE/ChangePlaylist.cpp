#include "ChangePlaylist.h"

ChangePlaylist::ChangePlaylist() {
	radio_list = 
	{
		Model("models/RadioSelect/radio1.obj"),
		Model("models/RadioSelect/radio2.obj"),
		Model("models/RadioSelect/radio3.obj")
	};

}


void ChangePlaylist::drawMenu(GraphicsSystem& graphics, State& state, AudioSystem* audio) {
	graphics.shader2D->use();
	radio_list[currentPlaylist].Draw(*graphics.shader2D);
}
