# AL CARPONE
## CPSC 585 - Team 3


Libraries:
- OpenGL (https://www.opengl.org//)
- glew (http://glew.sourceforge.net/)
- glfw (https://www.glfw.org/)
- PhysX (https://www.nvidia.com/en-us/drivers/physx/physx-9-19-0218-driver/)
- ImGui (https://github.com/ocornut/imgui)
- Assimp (https://github.com/assimp/assimp)
- stb (https://github.com/nothings/stb)

Hello, here is our second milestone!
We have a few maps set up that we've been using for testing and map building practice.

Tuning Testlevel shows off:
	An enemy police car model (No AI implemented yet)
	A robbable bank (Player's money displayed in window title)
	And some general obstacles used for tuning.

Racetrack has been used to tune the car handling, and to get a feel for how to create
roadways.

AI Testlevel is going to be used to tune the AI. We have a separate OBJ file representing
each intersection, and we plan to use this to work out how to tell an enemy "Go to this node/intersection".

City Scale Testlevel is our first attempt at using Blender-OSM (https://prochitecture.gumroad.com/l/blender-osm)
to generate a cityscape from satellite images. We plan to use this to get a general layout for our main level,
which we will then use as a template to build cleaner geometry/textures overtop of. 

TO RUN STANDALONE:

	Run either:
		CPSC585-G3/x64/Debug/CPSC585.exe
		or
		CPSC585-G3/x64/Release/CPSC585.exe
	
	NOTE: 
	      	Debug is very slow, especially when loading into Tuning Testlevel, 
	     	we recommend using Release.

CONTROLS:

	MAIN MENU:
		W/UpArrow/S/DownArrow: Change level selection
		Enter: Select highlighted level
	        Escape: Close game

	IN GAME:
		Q: Toggle third-person / free camera mode.
		Left Click (hold): Allows user to move mouse to look freely
		F5: Toggle Debug Mode (Camera disabled while in Debug Mode)
		ESC: Return to menu (Level select)
		R: Restart 
		C: Look behind
		F: Rob bank (When in range)
		Shift: Boost (Might be a powerup eventually, but for now it's just for fun while testing)

	THIRD PERSON VIEW:
		W: Accelerate forward
		A: Turn wheels left
		S: Brake/Reverse
		D: Turn wheels left
		Space: Handbrake

	FREE CAMERA VIEW:
		W: Move forward
		A: Move right
		S: Mode backward
		D: Move right
		Shift (hold): Increase movement speed