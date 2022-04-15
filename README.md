# AL CARPONE - CPSC 585 - TEAM 3

## HOW TO PLAY

The goal of the game is to escape town. This requires unlocking the gate which costs $250,000. To get this money, you must rob the bank! Rob speed starts out slow so you have to upgrade your rob speed at one of the garages. To help escape the police (who take your money if they catch you) you can buy upgrades for your car at the other garages. You can also buy temporary powerups at cornerstores to trick and distract police. Explore the symbols on the minimap to find all these locations. 


If you are detected by the police (get too close) they will begin to chase you. If you can evade them for long enough they will return to their regular patrol state. If they get too close they will start arresting you. If your jail countdown reaches 0 you are sent to jail and must pay at least $2000, or 75% of your money.



	
## CONTROLS
### KEYBOARD

MAIN MENU
	
	↑ & ↓: Change selection
	Enter: Select
	Escape: Close game

IN GAME
	
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
		
PAUSE MENU
	
	↑ & ↓: Change selection
	Enter: Select
	ESC: Go back to game
	
CORNER STORE & GARAGE MENUS
		
	↑ & ↓: Change selection
	Enter: Select
	F: Enter/exit menu 

### CONTROLLER

MAIN MENU
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select 
	O (Playstation) / B (Xbox): Close game
	
IN GAME
	
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
	
PAUSE MENU
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select
	Options (Playstation) / Menu (Xbox) / + (NS pro): Go back to game
	
CORNER STORE & GARAGE MENUS
	
	↑ & ↓: Change selection
	X (Playstation) / A (Xbox): Select
	□ (Playstation) / X (Xbox): Enter/exit menu
	
## ACHIEVMENTS LIST
- Freedom!
	- Fall off the map
- Play for the win
	- Finish the game
- Dupe the police
	- Escape with a jail countdown of 1, and 5-star alert level
- Transformer
	- Get all the upgrades
- Millionaire
	-  Get $1,000,000
-  Parkour
	- Get on top of the police station    

## UPGRADE LIST

![engine_upgrades](https://user-images.githubusercontent.com/60115955/163600154-60f97c4f-a2f2-4d2c-8267-41ba8d7fcddc.png)
- Top Speed
	- Increases top speed
	- 5 tiers
		- +10%
		- +20%
		- +30%
		- +40%
		- +50%
	 
- Gear Shift Speed
	- Increases gear shift speed
	- 3 tiers
		- +11%
		- +22%
		- +33%

![handling_upgrades](https://user-images.githubusercontent.com/60115955/163600654-77683ab6-942a-407f-9456-c4bdb607f159.png)
- Tire Friction
	- Increases tire friction
	- 3 tiers
		- +10%
		- +20%
		- +30%

- Steer Angle
	- Increases steering angle
	- 2 tiers
		- +33%
		- +100%

- Car Flipper
	- Allows you to flip yourself over with the steering controls
	- 1 tier

![robbery upgrades](https://user-images.githubusercontent.com/60115955/163600673-fd75b84d-1fc1-481c-aea3-fb87ff1ecbf8.png)
- Alarm Chance
	- Decreases the chances the bank alarm is triggered when robbing. The active percentage increases from the base percentage based on how long you are robbing and the bank trigger is checked every 2 seconds.
	- 2 tiers
		- 10% -> 7.5%
		- 7.5% -> 5%

- Detection Radius
	- Decreases the police's detection radius while patrolling
	- 5 tiers
		- -10%
		- -20%
		- -30%
		- -40%
		- -50%

- Enhanced Minimap
	- Adds additional minimap functionality
	- 3 tiers
		- Closest police car is visible on the minimap
		- Police cars within 200m are visible on the minimap
		- All police cars are visible on the minimap

## MAP
	
![minimap](https://user-images.githubusercontent.com/60115955/163597947-771b13d3-747c-4f82-b9b2-230b72ff7c77.png)


## HOW TO RUN STANDALONE

	1) Download the Release.zip archive provided below
	2) Extract Release.zip to create the 'Release' folder
	3) Run Release/AL_CARPONE.exe

## SIDENOTES 
- Pressing F5 enables the dev menu 
	- You can enter debug mode (stops police and allows debug controls <see Controls>)
	- You can also add money in this menu
	
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
	
