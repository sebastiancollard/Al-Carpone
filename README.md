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
- Irrklang (https://www.ambiera.com/irrklang/downloads.html)

Hello, here is our third milestone!

The goal of the game is to acquire enough money to escape town (currently $100).
This goal is very easy right now but we will make the game harder as we go forward.

To get money, you must rob the bank by holding F while close to it. (see map.png for bank and other building locations).

With this money, you can visit corner stores to purchase powerups and consumables to help out. 
- You can purchase a $5 donut to throw which will divert a police car to path to the donut and idle there for some time (Currently throwable but does nothing).
- You can purchase a $5 spike trap which will stun any police that pass over it (Currently throwable but does nothing).
- You can purchase camouflauge which will make you undetectable by the police for 15 seconds after activated (Currently usable but no visual indication).
Each consumable is activated by pressing E or L1 on controller.

Eventually the garages will have a selection of purchasable permanent upgrades to the car (Speed, rob rate, etc).

If you are detected by the police they will begin to chase you. If you can evade them for long enough they will return to their regular patrol state. If they 
catch up to you and are close to you for 2 seconds you will be sent to jail to pay a $10 fine. If you cannot pay this fine you lose, and will have to restart.

Sidenotes: 
- The boost functionality still exists on keyboard by pressing SHIFT, but that is only meant to be used for debugging and the player should not be able to use it.
- There are a few areas that need to be fixed where the walls have no collision and cause you to fall under the map. 

TO RUN STANDALONE:

	1) Download the Release.zip archive provided below
	2) Extract Release.zip to create the 'Release' folder
	3) Run Release/AL_CARPONE.exe
	
CONTROLS:

	MAIN MENU:
		↑ & ↓: Change selection
		Enter: Select
	        Escape: Close game

	IN GAME:
		W: Accelerate
		A: Turn left
		S: Brake/Reverse
		D: Turn left
		WASD: Mid-air control
		Space: Handbrake
		Left Click (Hold): Move camera
		ESC: Pause
		C: Look behind
		E: Use powerup
		F: Interact
		
	PAUSE MENU:
		↑ & ↓: Change selection
		Enter: Select
		ESC: Go back to game
	
	CORNER STORE & GARAGE MENUS:
		↑ & ↓: Change selection
		Enter: Select
		Esc: Exit menu (only works on garage menu for the moment)

CONTROLLER:

	MAIN MENU:
		↑ & ↓: Change level selection
		X (Playstation) / A (Xbox): Select highlighted level
		O (Playstation) / B (Xbox): Close game
	
	IN GAME:
		Left trigger: Reverse
		Right trigger: Move forward
		Left stick: Turn / Mid-air control
		Options (Playstation) / Menu (Xbox)/ + (NS pro): Pause menu
		□ (Playstation) / X (Xbox) / Y (NS pro): Interact
		▲ (Playstation) / Y (Xbox) / X (NS pro): reset
		L1 (Playstation) / LB (Xbox): Use powerup
		R1 (Playstation) / RB (Xbox): Handbrake
		R3 (Playstation) / RS (Xbox): Look behind
		
	PAUSE MENU:
		↑ & ↓: Change selection
		X (Playstation) / A (Xbox): Select
		Options (Playstation) / Menu (Xbox) / + (NS pro): Go back to game
	
	CORNER STORE & GARAGE MENUS:
		↑ & ↓: Change selection
		X (Playstation) / A (Xbox): Select
		O (Playstation) / B (Xbox): Exit menu
		
	
