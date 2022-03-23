#pragma once

#include <irrKlang.h>
#include "Player.h"
#include <thread>
#include <string>

enum SOUND_SELECTION {
	SONG_INTRO = 0,
	SONG_LOOP,
	ENGINE_LOOP,
	IDLE_LOOP,
	TURNOVER,
	BRAKE_LOOP,
	REV_UP,
	REV_LOOP,
	REV_DOWN,
	GEAR_SWITCH0,
	GEAR_SWITCH1,
	GROUND_SOUNDS,
	WIND_LOOP,
	SIREN_LOOP
	//TOTAL = 12
};

class AudioSystem {
public:
	irrklang::ISoundEngine* VehicleSoundEngine;
	irrklang::ISoundEngine* VehicleEngineSpecificSoundEngine;
	irrklang::ISoundEngine* MusicSoundEngine;
	irrklang::ISoundEngine* MiscSoundEngine;

	AudioSystem();

	void wait();

	void updateAudio(Player*, State*);

	void updateVehicleSounds(Player*, State*);

	void updateEngineAudio(Player*, float, float);

	void updateMusic(State*);

	void updateMiscSounds(Player*, State*);

	void setMusicVolume(float);

	void toggleMusic();

	void setPitchAndVolume(SOUND_SELECTION, float, float);

	irrklang::ISound* playSound(unsigned int, bool);
	irrklang::ISound* playENGINESound(unsigned int, bool);
	void stopSound(unsigned int);

private:

	bool introPlayed = false;
	float musicVolume = 0.25f;

	std::vector<irrklang::ISound*> soundPointers{
		NULL,	//SONG_INTRO
		NULL,	//SONG_LOOP
		NULL,	//ENGINE_LOOP
		NULL,	//IDLE_LOOP
		NULL,	//TURNOVER
		NULL,	//BRAKE_LOOP
		NULL,	//REV_UP
		NULL,	//REV_LOOP
		NULL,	//REV_DOWN
		NULL,	//GEARSWITCH0
		NULL,	//GEARSWITCH1
		NULL,	//GROUND_SOUNDS
		NULL,	//WIND_LOOP
		//TOTAL = 12
	};

	std::vector<irrklang::ISound*> policeSirenPointers = {
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL 
		//11 Police cars
	};

	std::vector<std::string> soundPaths{
		"audio/songINTRO.wav",
		"audio/songLOOP.wav",
		"audio/engineloop.wav",
		"audio/idleloop.wav",
		"audio/turnover.mp3",
		"audio/brakeLOOP.wav",
		"audio/revUP.wav",
		"audio/revLOOP.wav",
		"audio/revDOWN.wav",
		"audio/gearswitch0.mp3",
		"audio/gearswitch1.mp3",
		"audio/wheelsOnGroundLOOP.wav",
		"audio/windLOOP.wav",
		"audio/sirenloop.wav"
		//TOTAL = 12
	};
};



/*
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <iostream>


struct FMODImplementation {
	FMODImplementation();
	~FMODImplementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	typedef std::map <std::string, FMOD::Studio::EventInstance*> EventMap;
	typedef std::map<std::string, FMOD::Studio::Bank*> SoundbankMap;

	SoundbankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

class AudioSystem {
public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);

	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnLoadSound(const std::string& strSoundName);
	void Set3dListenerAndOrientation(const glm::vec3& vPos = glm::vec3( 0, 0, 0 ), float fVolumedB = 0.0f);
	int  PlaySounds(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3( 0, 0, 0 ), float fVolumedB = 0.0f);
	void PlayEvent(const std::string& strEventName);
	void StopChannel(int nChannelId);
	void StopEvent(const std::string& strEventName, bool bImmediate = false);
	void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
	void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
	void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
	void SetChannelvolume(int nChannelId, float fVolumedB);
	bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const std::string& strEventName) const;
	float dbToVolume(float db);
	float VolumeToDB(float volume);
	FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition);


	void test();
};

*/


