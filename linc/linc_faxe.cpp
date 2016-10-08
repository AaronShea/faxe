/**
* Faxe - FMOD bindings for Haxe
*
* The MIT License (MIT)
*
* Copyright (c) 2016 Aaron M. Shea
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <hxcpp.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <map>

#include "linc_faxe.h"

namespace linc 
{
	namespace faxe
	{
		// FMOD Sound System
		FMOD::Studio::System* fmodSoundSystem;
		FMOD::System* fmodLowLevelSoundSystem;

		// Maps to track what has been loaded already
		std::map<::String, FMOD::Studio::Bank*> loadedBanks;
		std::map<::String, FMOD::Sound*> loadedSounds;
		std::map<::String, FMOD::Studio::EventInstance*> loadedEvents;

		//// FMOD Init
		void faxe_init(int numChannels)
		{
			// Create our new fmod system
			if (FMOD::Studio::System::create(&fmodSoundSystem) != FMOD_OK)
			{
				printf("Failure starting FMOD sound system!");
				return;
			}

			// All OK - Setup some channels to work with!
			fmodSoundSystem->initialize(numChannels, NULL, NULL, nullptr);
			fmodSoundSystem->getLowLevelSystem(&fmodLowLevelSoundSystem);
			printf("FMOD Sound System Started with %d channels!\n", numChannels);
		}

		//// Sound Banks
		void faxe_load_bank(const ::String& bankName)
		{
			// Ensure this isn't already loaded
			if (loadedBanks.find(bankName) != loadedBanks.end())
			{
				return;
			}

			// Try and load the bank file
			FMOD::Studio::Bank* tempBank;
			auto result = fmodSoundSystem->loadBankFile(bankName.c_str(), NULL, &tempBank);
			if (result != FMOD_OK)
			{
				printf("FMOD failed to LOAD sound bank %s with error %s\n", bankName.c_str(), FMOD_ErrorString(result));
				return;
			}

			// List is as loaded
			loadedBanks[bankName] = tempBank;
		}

		void faxe_unload_bank(const ::String& bankName)
		{
			// Ensure this bank exists
			auto found = loadedBanks.find(bankName);
			if (found != loadedBanks.end())
			{
				// Remove from loaded banks map
				loadedBanks.erase(bankName);

				// Unload the bank that matches
				found->second->unload();
			}
		}

		void faxe_load_sound(const ::String& sndName, bool looping, bool streaming)
		{
			// Ensure the sound has not already been loaded
			if (loadedSounds.find(sndName) != loadedSounds.end())
			{
				return;
			}

			// Determine the loading mode based on boolean params
			FMOD_MODE loadSndMode = FMOD_DEFAULT;

			if (looping)
			{
				loadSndMode |= FMOD_LOOP_NORMAL;
			}

			if (streaming)
			{
				loadSndMode |= FMOD_CREATESTREAM;
			}

			// Try and load this sound
			FMOD::Sound* tempSound;
			auto result = fmodLowLevelSoundSystem->createSound(sndName.c_str(), loadSndMode, nullptr, &tempSound);
			if (result != FMOD_OK)
			{
				printf("FMOD failed to LOAD sound %s with error %s\n", sndName.c_str(), FMOD_ErrorString(result));
				return;
			}

			// Store in loaded sounds map
			loadedSounds[sndName] = tempSound;
		}

		void faxe_unload_sound(const ::String& sndName)
		{
			auto found = loadedSounds.find(sndName);

			// Ensure the sound has already been loaded
			if (found != loadedSounds.end())
			{
				// Remove from loaded map
				loadedSounds.erase(sndName);

				// Unload the sound
				found->second->release();
			}
		}

		void faxe_load_event(const ::String& eventName)
		{
			// Check it's not already loaded
			if (loadedEvents.find(eventName) != loadedEvents.end())
			{
				return;
			}

			// Try and load this event description
			FMOD::Studio::EventDescription* tempEvnDesc;
			auto result = fmodSoundSystem->getEvent(eventName.c_str(), &tempEvnDesc);

			if (result != FMOD_OK)
			{
				printf("FMOD failed to LOAD event instance %s with error %s\n", eventName.c_str(), FMOD_ErrorString(result));
				return;
			}

			// Now create an instance of this event that we can keep in memory
			FMOD::Studio::EventInstance* tempEvnInst;
			result = tempEvnDesc->createInstance(&tempEvnInst);

			if (result != FMOD_OK)
			{
				printf("FMOD failed to CREATE INSTANCE of event instance %s with error %s\n", eventName.c_str(), FMOD_ErrorString(result));
				return;
			}

			// Store in event map
			loadedEvents[eventName] = tempEvnInst;
		}

	} // faxe + fmod namespace
} // linc namespace
