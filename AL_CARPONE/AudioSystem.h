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
	REV0,
	REV1,
	REV2,
	REV3,
	REV4,
	GEAR_SWITCH0,
	GEAR_SWITCH1
	//TOTAL = 11
};

class AudioSystem {
public:
	irrklang::ISoundEngine* VehicleSoundEngine;
	irrklang::ISoundEngine* MusicSoundEngine;

	AudioSystem();

	void wait();

	void updateAudio(Player*, State*);

	void updateVehicleSounds(Player*, State*);

	void updateMusic(State*);

	irrklang::ISound* playSound(unsigned int, bool);

private:

	bool introPlayed = false;

	std::vector<irrklang::ISound*> soundPointers{
		NULL,	//SONG_INTRO
		NULL,	//SONG_LOOP
		NULL,	//ENGINE_LOOP
		NULL,	//IDLE_LOOP
		NULL,	//TURNOVER
		NULL,	//REV0
		NULL,	//REV1
		NULL,	//REV2
		NULL,	//REV3
		NULL,	//REV4
		NULL,	//GEARSWITCH0
		NULL	//GEARSWITCH1
		//TOTAL = 11
	};

	std::vector<std::string> soundPaths{
		"audio/songINTRO.wav",
		"audio/songLOOP.wav",
		"audio/engineloop.wav",
		"audio/idleloop.wav",
		"audio/turnover.mp3",
		"audio/rev0.mp3",
		"audio/rev1.mp3",
		"audio/rev2.mp3",
		"audio/rev3.mp3",
		"audio/rev4.mp3",
		"audio/gearswitch0.mp3",
		"audio/gearswitch1.mp3"
		//TOTAL = 11
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


