#include "AudioSystem.h"


AudioSystem::AudioSystem() {

	VehicleSoundEngine = irrklang::createIrrKlangDevice();
	MusicSoundEngine = irrklang::createIrrKlangDevice();

	VehicleSoundEngine->setSoundVolume(0.5f);
	MusicSoundEngine->setSoundVolume(0.5f);

	introPlayed = false;
}

void AudioSystem::wait() {
	while (1) {

	}
}

//SOUND SETTINGS
#define MAX_ENGINE_PITCH 1.50f
#define MIN_ENGINE_PITCH 0.5f

#define MAX_ENGINE_VOLUME 1.0f
#define MIN_ENGINE_VOLUME 0.5f

#define MAX_IDLE_VOLUME 0.7f
#define MIN_IDLE_VOLUME 0.3f

#define MAX_IDLE_PITCH 1.5f
#define MIN_IDLE_PITCH 1.0f


void AudioSystem::updateVehicleSounds(Player* player, State* state) {

	if (state->gamestate != GAMESTATE_INGAME) {
		VehicleSoundEngine->stopAllSounds();
		return;
	}

	if (player->vehicleInAir) return; //maybe play wind sounds?

	playSound(ENGINE_LOOP, true);
	playSound(IDLE_LOOP, true);

	float speed = fmin(fmax(player->getSpeedRatio(), 0.0f), 1.0f); //The players speedscale [0,1] 0 means not moving, 1 means max speed.
																	//Requires fmin/fmax [0,1] binding for when falling.

	if (soundPointers[IDLE_LOOP]) {
		soundPointers[IDLE_LOOP]->setVolume(MIN_IDLE_VOLUME + (MAX_IDLE_VOLUME - MIN_IDLE_VOLUME) * speed);
		soundPointers[IDLE_LOOP]->setPlaybackSpeed(MIN_IDLE_PITCH + (MAX_IDLE_PITCH - MIN_IDLE_PITCH) * speed);
	}

	if (soundPointers[ENGINE_LOOP]) {
		soundPointers[ENGINE_LOOP]->setPlaybackSpeed(MIN_ENGINE_PITCH + (MAX_ENGINE_PITCH - MIN_ENGINE_PITCH) * speed);
		soundPointers[ENGINE_LOOP]->setVolume(MIN_ENGINE_VOLUME + (MAX_ENGINE_VOLUME - MIN_ENGINE_VOLUME) * speed);
	}

	if (player->isChangingGears()) {
		printf("SWITCHING GEARS\n");
		unsigned int random = rand() % 1;
		playSound(GEAR_SWITCH0 + random, false);
	}

	if (player->newAccelInput()) {
		printf("newAccelInput\n");
		unsigned int random = rand() % 5;
		playSound(REV0 + random, false);// ->setPlaybackSpeed(0.5f);
	}

}



void AudioSystem::updateMusic(State* state) {

	if (!introPlayed && !MusicSoundEngine->isCurrentlyPlaying(soundPaths[SONG_INTRO].c_str())) {
		soundPointers[SONG_INTRO] = MusicSoundEngine->play2D(soundPaths[SONG_INTRO].c_str(), false);
		introPlayed = true;
	}
	if (soundPointers[SONG_INTRO] && soundPointers[SONG_INTRO]->isFinished()
		&& !MusicSoundEngine->isCurrentlyPlaying(soundPaths[SONG_LOOP].c_str())) {

		soundPointers[SONG_LOOP] = MusicSoundEngine->play2D(soundPaths[SONG_LOOP].c_str(), true);
	}

}

void AudioSystem::updateAudio(Player* player, State* state) {

	updateMusic(state);
	updateVehicleSounds(player, state);

	VehicleSoundEngine->update();
	MusicSoundEngine->update();

}

irrklang::ISound* AudioSystem::playSound(unsigned int selection, bool loop) {

	if (!VehicleSoundEngine->isCurrentlyPlaying(soundPaths[selection].c_str())) {
		irrklang::ISound* sound = VehicleSoundEngine->play2D(soundPaths[selection].c_str(), loop);
		soundPointers[selection] = sound;
		return sound;
	}

}


/*
FMODImplementation* fmod;

FMODImplementation::FMODImplementation() {
	mpStudioSystem = NULL;
	AudioSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	AudioSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
	AudioSystem::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

FMODImplementation::~FMODImplementation() {
	AudioSystem::ErrorCheck(mpStudioSystem->unloadAll());
	AudioSystem::ErrorCheck(mpStudioSystem->release());
}


void FMODImplementation::Update() {
	std::vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			pStoppedChannels.push_back(it);
		}
	}
	for (auto& it : pStoppedChannels)
	{
		mChannels.erase(it);
	}
	AudioSystem::ErrorCheck(mpStudioSystem->update());
}

void AudioSystem::Init() {
	fmod = new FMODImplementation;
}

void AudioSystem::Update() {
	fmod->Update();
}

void AudioSystem::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream){
	auto tFoundIt = fmod->mSounds.find(strSoundName);
	if (tFoundIt != fmod->mSounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	AudioSystem::ErrorCheck(fmod->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if (pSound) {
		fmod->mSounds[strSoundName] = pSound;
	}
}

void AudioSystem::UnLoadSound(const std::string& strSoundName)
{
	auto tFoundIt = fmod->mSounds.find(strSoundName);
	if (tFoundIt == fmod->mSounds.end())
		return;

	AudioSystem::ErrorCheck(tFoundIt->second->release());
	fmod->mSounds.erase(tFoundIt);
}

int AudioSystem::PlaySounds(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB){
	int nChannelId = fmod->mnNextChannelId++;
	auto tFoundIt = fmod->mSounds.find(strSoundName);
	if (tFoundIt == fmod->mSounds.end())
	{
		LoadSound(strSoundName);
		tFoundIt = fmod->mSounds.find(strSoundName);
		if (tFoundIt == fmod->mSounds.end())
		{
			return nChannelId;
		}
	}
	FMOD::Channel* pChannel = nullptr;
	AudioSystem::ErrorCheck(fmod->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFmod(vPos);
			AudioSystem::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}
		AudioSystem::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
		AudioSystem::ErrorCheck(pChannel->setPaused(false));
		fmod->mChannels[nChannelId] = pChannel;
	}
	return nChannelId;
}

void AudioSystem::SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
	auto tFoundIt = fmod->mChannels.find(nChannelId);
	if (tFoundIt == fmod->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFmod(vPosition);
	AudioSystem::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void AudioSystem::SetChannelvolume(int nChannelId, float fVolumedB){
	auto tFoundIt = fmod->mChannels.find(nChannelId);
	if (tFoundIt == fmod->mChannels.end())
		return;

	AudioSystem::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void AudioSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
	auto tFoundIt = fmod->mBanks.find(strBankName);
	if (tFoundIt != fmod->mBanks.end())
		return;
	FMOD::Studio::Bank* pBank;
	AudioSystem::ErrorCheck(fmod->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
	if (pBank) {
		fmod->mBanks[strBankName] = pBank;
	}
}

void AudioSystem::LoadEvent(const std::string& strEventName) {
	auto tFoundit = fmod->mEvents.find(strEventName);
	if (tFoundit != fmod->mEvents.end())
		return;
	FMOD::Studio::EventDescription* pEventDescription = NULL;
	AudioSystem::ErrorCheck(fmod->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
	if (pEventDescription) {
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		AudioSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance) {
			fmod->mEvents[strEventName] = pEventInstance;
		}
	}
}

void AudioSystem::PlayEvent(const std::string& strEventName) {
	auto tFoundit = fmod->mEvents.find(strEventName);
	if (tFoundit == fmod->mEvents.end()) {
		LoadEvent(strEventName);
		tFoundit = fmod->mEvents.find(strEventName);
		if (tFoundit == fmod->mEvents.end())
			return;
	}
	tFoundit->second->start();
}

void AudioSystem::StopEvent(const std::string& strEventName, bool bImmediate) {
	auto tFoundIt = fmod->mEvents.find(strEventName);
	if (tFoundIt == fmod->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	AudioSystem::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool AudioSystem::IsEventPlaying(const std::string& strEventName) const {
	auto tFoundIt = fmod->mEvents.find(strEventName);
	if (tFoundIt == fmod->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
		return true;
	}
	return false;
}

void AudioSystem::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter) {

	
	
	POTENTIAL ISSUE - DIFFERS FROM REFERENCE:

	void CAudioEngine::GetEventParameter(const string &strEventName, const string &strParameterName, float* parameter) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD::Studio::ParameterInstance* pParameter = NULL;
	CAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
	CAudioEngine::ErrorCheck(pParameter->getValue(parameter));
	}

	

	auto tFoundIt = fmod->mEvents.find(strEventName);
	if (tFoundIt == fmod->mEvents.end())
		return;

	float* pParameter;
	AudioSystem::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter)); 
}

void AudioSystem::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue) {

	
	 
	 POTENTIAL ISSUE - DIFFERS FROM REFERENCE:
	
	void CAudioEngine::SetEventParameter(const string &strEventName, const string &strParameterName, float fValue) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD::Studio::ParameterInstance* pParameter = NULL;
	CAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
	CAudioEngine::ErrorCheck(pParameter->setValue(fValue));
}

	

	auto tFoundIt = fmod->mEvents.find(strEventName);
	if (tFoundIt == fmod->mEvents.end())
		return;

	AudioSystem::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), &fValue));
}

FMOD_VECTOR AudioSystem::VectorToFmod(const glm::vec3& vPosition) {
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

float  AudioSystem::dbToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}

float  AudioSystem::VolumeToDB(float volume)
{
	return 20.0f * log10f(volume);
}

int AudioSystem::ErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMOD ERROR " << result << std::endl;
		return 1;
	}
	// cout << "FMOD all good" << endl;
	return 0;
}

void AudioSystem::Shutdown() {
	delete fmod;
}





void AudioSystem::test() {
	LoadSound("audio/rev0.mp3");
	PlaySounds();
}

*/





