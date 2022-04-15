# AL CARPONE - CPSC 585 - TEAM 3

## HOW TO PLAY

The goal of the game is to escape town. This requires unlocking the gate which costs $250,000. To get this money, you must rob the bank! Rob speed starts out slow so you have to upgrade your rob speed at one of the garages. To help escape the police (who take your money if they catch you) you can buy upgrades for your car at the other garages. You can also buy temporary powerups at cornerstores to trick and distract police. Explore the symbols on the minimap to find all these locations. 


If you are detected by the police (get too close) they will begin to chase you. If you can evade them for long enough they will return to their regular patrol state. If they get too close they will start arresting you. If your jail countdown reaches 0 you are sent to jail and must pay at least $2000, or 75% of your money.

Sidenotes: 
-	Press f5 for dev tools. You can enter debug mode (stops police and allows debug controlls <see Controls>), you can also add money in this menu.

-	Car flip upgrade not yet implemented (flipping your car over & getting stuck is possible, and can cause you to lose the game. to prevent this you can
	buy a car flip upgrade that allows you to flip your car while upside down. This functionality is not yet implemented, so if you get stuck, enter debugmode 
	and hit 'R')

- 	We are planning on adding an additional upgrade to the starting garage which would allow the player to see police locations on the minimap. The upgrade path 
	would consist of increasing the radius around the player and reveal all police locations within that radius.

	
## CONTROLS
### KEYBOARD

MAIN MENU:
	
	↑ & ↓: Change selection
	Enter: Select
	Escape: Close game

IN GAME:
	
	W: Accelerate
	A: Turn left
	S: Brake/Reverse
	D: Turn right
	WASD: Mid-air control
	Space: Handbrake
	Left Click (Hold): Move camera
	ESC: Pause
	C: Look behind
	E: Use powerup
	F: Interact
	H: Headlights
	T: Request towing service 

	Shift <DEBUGMODE ONLY> : Speed boost
	R <DEBUGMODE ONLY> : Reset all positions
	Q <DEBUGMODE ONLY> : Unbound camera
		
PAUSE MENU:
	
	↑ & ↓: Change selection
	Enter: Select
	ESC: Go back to game
	
CORNER STORE & GARAGE MENUS:
		
	↑ & ↓: Change selection
	Enter: Select
	F: Enter/exit menu 

### CONTROLLER

MAIN MENU:
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select 
	O (Playstation) / B (Xbox): Close game
	
IN GAME:
	
	Left trigger: Reverse
	Right trigger: Move forward
	Left stick: Turn / Mid-air control
	Options (Playstation) / Menu (Xbox)/ + (NS pro): Pause menu
	□ (Playstation) / X (Xbox) / Y (NS pro): Interact
	L1 (Playstation) / LB (Xbox): Use powerup
	R1 (Playstation) / RB (Xbox): Handbrake
	R3 (Playstation) / RS (Xbox): Look behind
	L3 (Playstation) / LS (Xbox): Center camera
	DPad Up: Switch headlight mode
	▲ : Request towing service

	▲ (Playstation) / Y (Xbox) / X (NS pro) <DEBUGMODE ONLY> : reset
	
PAUSE MENU:
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select
	Options (Playstation) / Menu (Xbox) / + (NS pro): Go back to game
	
CORNER STORE & GARAGE MENUS:
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select
	□ (Playstation) / X (Xbox): Enter/exit menu
	
## ACHIEVMENTS LIST

## HOW TO RUN STANDALONE

	1) Download the Release.zip archive provided below
	2) Extract Release.zip to create the 'Release' folder
	3) Run Release/AL_CARPONE.exe
	
## Libraries
- OpenGL (https://www.opengl.org//)
- glew (http://glew.sourceforge.net/)
- glfw (https://www.glfw.org/)
- PhysX (https://www.nvidia.com/en-us/drivers/physx/physx-9-19-0218-driver/)
- ImGui (https://github.com/ocornut/imgui)
- Assimp (https://github.com/assimp/assimp)
- stb (https://github.com/nothings/stb)
- Irrklang (https://www.ambiera.com/irrklang/downloads.html)
- Freetype (https://freetype.org/)
	
