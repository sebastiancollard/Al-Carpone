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
		E: Use item
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

CONTROLLER:

	MAIN MENU:
		↑ & ↓: Change level selection
		X (ps4) / A (xbox): Select highlighted level
		O (ps4) / B (xbox): Close game
	
	IN GAME:
		Left trigger: move reverse
		Right trigger: move forward
		Left stick: controls player left or right turn
		Options(ps4)/menu(xbox)/+(ns pro): pause menu
		□ (ps4) / x (xbox) / y (nspro): building trigger function
		▲ (ps4) / y (xbox) / x (nspro): reset
		L1 (ps4): use item
		Handbrake not yet added
		Look behind not yet added
		
	PAUSE MENU:
		↑ & ↓ ==> change selection
		X(ps4)/A(xbox) ==> select
		options(ps4)/menu(xbox)/+(ns pro) ==> go back to in game state
		corner store:
		↑ & ↓ ==> change selection
		X(ps4) / A (xbox) ==> select
		O (ps4) / B (xbox) ==> back to in game state
	
	CORNER STORE:
		↑ & ↓: change selection
		X (ps4) / A (xbox): select
		O (ps4) / B (xbox): back to in game state
		
	
