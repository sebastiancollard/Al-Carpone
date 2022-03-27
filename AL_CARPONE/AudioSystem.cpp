#include "AudioSystem.h"


AudioSystem::AudioSystem() {

	VehicleSoundEngine = irrklang::createIrrKlangDevice();
	VehicleEngineSpecificSoundEngine = irrklang::createIrrKlangDevice();
	MusicSoundEngine = irrklang::createIrrKlangDevice();
	PoliceSoundEngine = irrklang::createIrrKlangDevice();
	EffectsSoundEngine = irrklang::createIrrKlangDevice();

	VehicleSoundEngine->setSoundVolume(1.0f);
	MusicSoundEngine->setSoundVolume(musicVolume);
	EffectsSoundEngine->setSoundVolume(0.3f);

	for (int i = 0; i < 11; i++) {
		policeSirenPointers[i] = PoliceSoundEngine->play3D(soundPaths[SIREN_LOOP].c_str(), irrklang::vec3df(0,0,0), true, true);
		policeSirenPointers[i]->setVolume(3.0f);
		policeSirenPointers[i]->setMinDistance(10.0f);
	}

	introPlayed = false;
}

void AudioSystem::wait() {

}

void AudioSystem::setMusicVolume(float volume) {
	MusicSoundEngine->setSoundVolume(volume);
}

void AudioSystem::toggleMusic() {
	if (MusicSoundEngine->getSoundVolume() == 0) MusicSoundEngine->setSoundVolume(musicVolume);
	else MusicSoundEngine->setSoundVolume(0);
}

void AudioSystem::setPitchAndVolume(SOUND_SELECTION selection, float pitch, float volume) {
	if (soundPointers[selection]) {
		soundPointers[selection]->setPlaybackSpeed(pitch);
		soundPointers[selection]->setVolume(volume);
	}
}

void AudioSystem::playRobLoop() {
	if (!soundPointers[SOUND_SELECTION::ROB_LOOP]){
		irrklang::ISound* sound = EffectsSoundEngine->play2D(soundPaths[SOUND_SELECTION::ROB_LOOP].c_str(), false);
		soundPointers[SOUND_SELECTION::ROB_LOOP] = sound;
	}
	else if (soundPointers[SOUND_SELECTION::ROB_LOOP]->isFinished()) {
		irrklang::ISound* sound = EffectsSoundEngine->play2D(soundPaths[SOUND_SELECTION::ROB_LOOP].c_str(), false);
		soundPointers[SOUND_SELECTION::ROB_LOOP] = sound;
	}
}

void AudioSystem::playSoundEffect(SOUND_SELECTION selection) {
	if (EffectsSoundEngine->isCurrentlyPlaying(soundPaths[selection].c_str())) {
		soundPointers[selection]->stop();
	}
	irrklang::ISound* sound = EffectsSoundEngine->play2D(soundPaths[selection].c_str(), false);
	soundPointers[selection] = sound;
}

void AudioSystem::stopMusic() {
	musicShouldPlay = false;
	introPlayed = false;
	
	MusicSoundEngine->stopAllSounds();
}

void AudioSystem::triggerBankAlarm(float playerx, float playery, float playerz) {
	if (!EffectsSoundEngine->isCurrentlyPlaying(soundPaths[SOUND_SELECTION::BANK_ALARM].c_str())) {
		//irrklang::vec3df bankpos(490 - playerx, 30 - playery, -430 - playerz);
		irrklang::ISound* sound = EffectsSoundEngine->play2D(soundPaths[SOUND_SELECTION::BANK_ALARM].c_str());
		soundPointers[SOUND_SELECTION::BANK_ALARM] = sound;
	}
}

//SOUND SETTINGS


const float MAX_IDLE_PITCH = 1.1f;
const float MIN_IDLE_PITCH = 0.75f;
const float MAX_IDLE_VOLUME = 0.35f;
const float MIN_IDLE_VOLUME = 0.03f;

const float MIN_REV_PITCH = 0.75f;
const float MAX_REV_PITCH = 1.0f;
const float MIN_REV_VOLUME = 0.3f;
const float MAX_REV_VOLUME = 0.7f;

const float MIN_WIND_PITCH = 1.0f;
const float MAX_WIND_PITCH = 1.5f;
const float MIN_WIND_VOLUME = 0.05f;
const float MAX_WIND_VOLUME = 1.5f;

const float MIN_GROUND_PITCH = 1.5f;
const float MAX_GROUND_PITCH = 2.0f;
const float MIN_GROUND_VOLUME = 0.0f;
const float MAX_GROUND_VOLUME = 0.2f;

const float MIN_BRAKE_PITCH = 0.7f;
const float MAX_BRAKE_PITCH = 1.3f;
const float MIN_BRAKE_VOLUME = 0.0f;
const float MAX_BRAKE_VOLUME = 0.75f;

float revScale = 0.0f;
float revScaleGoal = 0.0f;

int gearChanged;

//TO DO:
//	MAYBE ADD EXTRA SOUND ON GEAR CHANGES
//	ADD CRASH NOISES
//	ADD BRAKE NOISES
//	FIX LOUD AUDIO WHEN LOADING NEW LEVEL
//	ADD QUIET REV WHEN REVERSING
//	ADD GROUND SOUNDS ON TIGHT TURNS
//	ADD THUNKS WHEN THUMPING UP/DOWN ON BUMPS (SPEED BUMP IN TUNING LEVEL TO TEST)

bool footOnGas;

void AudioSystem::updateEngineAudio(Player* player, float pitch, float volume) {


	if (footOnGas) {
		if (!player->footOnGas()) {

			stopSound(SOUND_SELECTION::REV_LOOP);
			playENGINESound(SOUND_SELECTION::REV_DOWN, false);

			if (soundPointers[SOUND_SELECTION::REV_UP] && !soundPointers[SOUND_SELECTION::REV_UP]->isFinished()) {
				
				
				unsigned int revDownLength = soundPointers[SOUND_SELECTION::REV_DOWN]->getPlayLength();
				unsigned int revUpLength = soundPointers[SOUND_SELECTION::REV_UP]->getPlayLength();

				unsigned int revUpPosition = soundPointers[SOUND_SELECTION::REV_UP]->getPlayPosition();

				float scale = (float)revUpPosition / (float)revUpLength;
				
				unsigned int posUpdate = revDownLength * (0.4 - scale);

				soundPointers[SOUND_SELECTION::REV_UP]->stop();

				//printf("REVDOWNLENGTH = %.2fs	REVUPPOSITION = %.2fs	POSUPDATE = %.2f\n", (float)revDownLength / 1000.0f, (float)revUpPosition / 1000.0f, (float)posUpdate/1000.0f);

				soundPointers[SOUND_SELECTION::REV_DOWN]->setPlayPosition(posUpdate);
				//This plays the revdown quieter the less far you are into the rev up.
			}
				

			footOnGas = false;
		}

		else if (soundPointers[SOUND_SELECTION::REV_UP] && soundPointers[SOUND_SELECTION::REV_UP]->isFinished()) {
			playENGINESound(SOUND_SELECTION::REV_LOOP, true);
		}
	}

	else {
		if (player->footOnGas()) {

			if (soundPointers[SOUND_SELECTION::REV_DOWN] && !soundPointers[SOUND_SELECTION::REV_DOWN]->isFinished())
				stopSound(SOUND_SELECTION::REV_DOWN);

			playENGINESound(SOUND_SELECTION::REV_UP, false);
			footOnGas = true;
		}
	}

	setPitchAndVolume(SOUND_SELECTION::REV_UP, pitch, 1);
	setPitchAndVolume(SOUND_SELECTION::REV_LOOP, pitch, 1);
	setPitchAndVolume(SOUND_SELECTION::REV_DOWN, pitch, 1);
	//To prevent clipping, have to set all to the same volume and then change the overall volume.
	VehicleEngineSpecificSoundEngine->setSoundVolume(volume);

}

void AudioSystem::updateVehicleSounds(Player* player, State* state) {

	if (state->gamestate != GAMESTATE::GAMESTATE_INGAME || state->gameWon) {
		//if gameWon play winsound
		VehicleSoundEngine->stopAllSounds();
		VehicleEngineSpecificSoundEngine->stopAllSounds();
		return;
	}

	float speed = fmin(length(player->getLinearVelocity()), 200);
	int gear = player->vehiclePtr->mDriveDynData.getCurrentGear();

	revScaleGoal = ((gear == 0) ? 0 : (float)gear - 2.0f) / 5.0f;
	revScale += (revScaleGoal - revScale) * (float)state->timeStep;

	float pitch = MIN_REV_PITCH + revScale * (MAX_REV_PITCH - MIN_REV_PITCH);
	float volume = MIN_REV_VOLUME + revScale * (MAX_REV_VOLUME - MIN_REV_VOLUME);

	//Start all loops
	playSound(SOUND_SELECTION::WIND_LOOP, true);
	setPitchAndVolume(SOUND_SELECTION::WIND_LOOP,
		MIN_WIND_PITCH + (MAX_WIND_PITCH - MIN_WIND_PITCH) * (speed / 350.0f),
		MIN_WIND_VOLUME + (MAX_WIND_VOLUME - MIN_WIND_VOLUME) * (speed / 350.0f));

	
		



	updateEngineAudio(player, pitch, volume);

	

	if (player->vehicleInAir) {
		stopSound(SOUND_SELECTION::GROUND_SOUNDS);
		stopSound(SOUND_SELECTION::BRAKE_LOOP);;
		return;
	}
	
	playSound(SOUND_SELECTION::GROUND_SOUNDS, true);
	setPitchAndVolume(SOUND_SELECTION::GROUND_SOUNDS,
		MIN_GROUND_PITCH + (MAX_GROUND_PITCH - MIN_GROUND_PITCH) * (speed / 10.0f),
		MIN_GROUND_VOLUME + (MAX_GROUND_VOLUME - MIN_GROUND_VOLUME) * (speed / 10.0f));

	playSound(IDLE_LOOP, true);

	if (soundPointers[IDLE_LOOP]) {
		soundPointers[IDLE_LOOP]->setVolume(MIN_IDLE_VOLUME + (MAX_IDLE_VOLUME - MIN_IDLE_VOLUME) * revScale); // maybe make just speed based
		soundPointers[IDLE_LOOP]->setPlaybackSpeed(MIN_IDLE_PITCH + (MAX_IDLE_PITCH - MIN_IDLE_PITCH) * revScale);
	}

	if (gear == 1) {
		if (!gearChanged) {
			unsigned int random = rand() % 1;
			//playSound(GEAR_SWITCH0 + random, false)->setVolume(0.2f); sounds bad
		}
		gearChanged = true;
	}

	else gearChanged = false;

	if (player->footOnBrake() && speed > 10.0f) {
		pitch = MIN_BRAKE_PITCH + (MAX_BRAKE_PITCH - MIN_BRAKE_PITCH) * speed / 50.0f;
		volume = MIN_BRAKE_VOLUME + (MAX_BRAKE_VOLUME - MIN_BRAKE_VOLUME) * speed / 50.0f;

		playSound(SOUND_SELECTION::BRAKE_LOOP, true);
		setPitchAndVolume(SOUND_SELECTION::BRAKE_LOOP, pitch, volume);
	}
	else stopSound(SOUND_SELECTION::BRAKE_LOOP);
}



void AudioSystem::updateMusic(State* state) {

	if (!musicShouldPlay) return;

	if (!introPlayed && !MusicSoundEngine->isCurrentlyPlaying(soundPaths[SONG_INTRO].c_str())) {
		soundPointers[SONG_INTRO] = MusicSoundEngine->play2D(soundPaths[SONG_INTRO].c_str(), false);
		introPlayed = true;
	}
	if (soundPointers[SONG_INTRO] && soundPointers[SONG_INTRO]->isFinished()
		&& !MusicSoundEngine->isCurrentlyPlaying(soundPaths[SONG_LOOP].c_str())) {

		soundPointers[SONG_LOOP] = MusicSoundEngine->play2D(soundPaths[SONG_LOOP].c_str(), true);
	}

}

void AudioSystem::updatePoliceSounds(Player* player, State* state) {

	if (state->gamestate == GAMESTATE::GAMESTATE_MAIN_MENU) {
		for (irrklang::ISound* p : policeSirenPointers) {
			p->setIsPaused(true);
		}
		return;
	}

	glm::vec3 playerPosGLM = player->getPos();

	assert(state->activePoliceVehicles.size() <= 11);

	for (int i = 0; i < state->activePoliceVehicles.size(); i++) {

		PoliceCar* policecar = state->activePoliceVehicles[i];

		if (policecar->ai_state == AISTATE::CHASE) {

			glm::vec3 policePosGLM = policecar->getPos();

			irrklang::vec3df audioPos(policePosGLM.x - playerPosGLM.x, policePosGLM.y - playerPosGLM.y, policePosGLM.z - playerPosGLM.z);
			
			if (policeSirenPointers[i] != NULL) {
				policeSirenPointers[i]->setPosition(audioPos);
				policeSirenPointers[i]->setPan(glm::dot(player->getRight(), glm::normalize(policePosGLM - playerPosGLM)));
				policeSirenPointers[i]->setIsPaused(false);
			}
		
		}

		else if (policeSirenPointers[i] != NULL) {
			policeSirenPointers[i]->setIsPaused(true);
		}

	}
}


void AudioSystem::updateAudio(Player* player, State* state) {

	updateMusic(state);
	updateVehicleSounds(player, state);
	updatePoliceSounds(player, state);

	VehicleEngineSpecificSoundEngine->update();
	VehicleSoundEngine->update();
	MusicSoundEngine->update();
	PoliceSoundEngine->update();


}

void AudioSystem::stopSound(unsigned int selection) {
	if (soundPointers[selection]) soundPointers[selection]->stop();
}

irrklang::ISound* AudioSystem::playSound(unsigned int selection, bool loop) {
	if (!VehicleSoundEngine->isCurrentlyPlaying(soundPaths[selection].c_str())) {
		irrklang::ISound* sound = VehicleSoundEngine->play2D(soundPaths[selection].c_str(), loop);
		soundPointers[selection] = sound;
		return sound;
	}
}

irrklang::ISound* AudioSystem::playENGINESound(unsigned int selection, bool loop) {

	if (!VehicleEngineSpecificSoundEngine->isCurrentlyPlaying(soundPaths[selection].c_str())) {
		irrklang::ISound* sound = VehicleEngineSpecificSoundEngine->play2D(soundPaths[selection].c_str(), loop);
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





