// FranticDreamer 2022-2025

#include <filesystem>
#include <algorithm>
#include <cctype>

#include "Backend.hpp"
#include "miniaudio/Backend_miniaudio.hpp"
#ifdef FRANAUDIO_USE_OPENAL
#include "OpenALSoft/Backend_OpenALSoft.hpp"
#endif

#include <ranges>

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Backend
{
	FRANAUDIO_API Backend::~Backend()
	{

	}

	FRANAUDIO_API bool Backend::Init(FranAudio::Decoder::DecoderType decoderType)
	{
		bool decoderFail = false;
		SetDecodeSettings(GetDefaultDecodeSettings());

		if (decoderType == FranAudio::Decoder::DecoderType::None)
		{
			FranAudioShared::Logger::LogError(std::format("{}: No decoder type specified", GetBackendName()));
			decoderFail = true;
		}

		// Check if the requested decoder is supported
		const auto& supportedDecoders = GetSupportedDecoders();
		if (std::find(supportedDecoders.begin(), supportedDecoders.end(), decoderType) == supportedDecoders.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Requested decoder is not supported by this backend", GetBackendName()));
			decoderFail = true;
		}

		if (decoderFail)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Defaulting to miniaudio decoder", GetBackendName()));
			decoderType = FranAudio::Decoder::DecoderType::miniaudio;
		}

		// Decoder will be initialised by the FranAudio::Init
		currentDecoderType = decoderType;

		FranAudioShared::Logger::LogMessage(std::format("{}: Initialised Backend.", GetBackendName()));
		return true; // Default backend does nothing
	}

	FRANAUDIO_API void Backend::Reset()
	{
		Shutdown(true);
		if (currentDecoder)
		{
			currentDecoder->Reset();
		}
		Init(currentDecoderType);
	}

	FRANAUDIO_API void Backend::Shutdown(bool forReset)
	{
		FranAudioShared::Logger::LogMessage(std::format("{}: Shutting down backend...", GetBackendName()));

		nextSoundID = 0;
		activeSounds.clear();
		groupVolumes.clear();
		groupExclusive.clear();
		exclusiveGroupSound.clear();
		soundGroups.clear();
		soundBaseVolumes.clear();

		if (!forReset)
		{
			DestroyDecoder();
		}

		FranAudioShared::Logger::LogMessage(std::format("{}: Backend shut down.", GetBackendName()));
	}

	// ========================
	// Decoder Management
	// ========================

	FRANAUDIO_API FranAudio::Decoder::DecoderType Backend::GetDecoderType() const
	{
		return currentDecoderType;
	}

	FRANAUDIO_API FranAudio::Decoder::Decoder* Backend::GetCurrentDecoder() const
	{
		return currentDecoder.get();
	}

	FRANAUDIO_API bool Backend::SetDecoder(FranAudio::Decoder::DecoderType decoderType, bool force)
	{
		if (currentDecoderType == decoderType && !force)
		{
			return true; // No need to change the decoder
		}

		if (currentDecoder != nullptr)
		{
			currentDecoder->Shutdown();
			currentDecoder.reset();
		}

		// Check if the decoder type is supported
		for (const auto& supportedDecoder : GetSupportedDecoders())
		{
			if (supportedDecoder == decoderType)
			{
				currentDecoder = FranAudio::Decoder::Decoder::CreateDecoder(decoderType);
				currentDecoderType = decoderType;
				FranAudioShared::Logger::LogMessage(std::format("{}: Initialised decoder type {}", GetBackendName(), GetDecoderName()));
				break;
			}
		}

		if (currentDecoder == nullptr)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Decoder type not supported or failed to initialise", GetBackendName()));
			return false;
		}

		return true;
	}

	FRANAUDIO_API void Backend::DestroyDecoder()
	{
		if (currentDecoder != nullptr)
		{
			currentDecoder->Shutdown();
			currentDecoder.reset();
		}
	}

	const FRANAUDIO_API FranAudio::Decoder::DecodeSettings& Backend::GetDefaultDecodeSettings() const noexcept
	{
		return defaultDecodeSettings;
	}

	FRANAUDIO_API void Backend::SetDecodeSettings(const FranAudio::Decoder::DecodeSettings& settings) noexcept
	{
		currentDecodeSettings = settings;
	}

	FRANAUDIO_API void Backend::SetListenerTransform(const FranAudioShared::ListenerTransform& transform)
	{
		SetListenerTransform(transform.position, transform.forward, transform.up);
	}

	FRANAUDIO_API void Backend::SetListenerOrientation(const FranAudioShared::ListenerOrientation& orientation)
	{
		SetListenerOrientation(orientation.forward, orientation.up);
	}

	// ========================
	// Audio File Management
	// ========================

	std::string Backend::CanonicalisePath(const std::string& filename)
	{
		std::error_code errorCode;
		std::filesystem::path canonical = std::filesystem::weakly_canonical(filename, errorCode);

		std::string result = errorCode ? filename : canonical.string();

	#ifdef _WIN32
		// Windows paths are case-insensitive.
		std::ranges::transform(result, result.begin(), [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
	#endif

		return result;
	}

	FRANAUDIO_API size_t Backend::LoadAudioFile(const std::string& filename)
	{
		return LoadAudioFile(filename, currentDecodeSettings);
	}

	FRANAUDIO_API size_t Backend::LoadAudioFile(const std::string& filename, const FranAudio::Decoder::DecodeSettings& decodeSettings)
	{
		const std::string pathKey = CanonicalisePath(filename);

		if (const auto it = filenameWaveMap.find(pathKey); it != filenameWaveMap.end())
			return it->second;

		const std::filesystem::path filePath(filename);

		if (!std::filesystem::exists(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File does not exist: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		if (std::filesystem::is_directory(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File is a directory: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		if (std::filesystem::is_empty(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File is empty: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		FranAudio::Sound::WaveData waveData;
		bool result = currentDecoder->DecodeAudioFile(filename, waveData, *this, decodeSettings);

		if (!result)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Failed to decode audio file: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		// Wave data IDs are unique and never reused, just like sound IDs.
		const size_t waveDataID = nextWaveDataID++;
		waveData.SetWaveDataID(waveDataID);
		waveDataCache[waveDataID] = std::move(waveData);
		filenameWaveMap[pathKey] = waveDataID;

		const auto& loadedWaveData = waveDataCache[waveDataID];
		FranAudioShared::Logger::LogMessage(std::format("{}: Loaded audio file: {} ({}s, {} channels, {}Hz, Format: {})", GetBackendName(), filename, loadedWaveData.GetLength(), (int)loadedWaveData.GetChannels(), loadedWaveData.GetSampleRate(), FranAudio::Sound::WaveFormatNames[(size_t)loadedWaveData.GetFormat()]));

		return waveDataID;
	}

	FRANAUDIO_API size_t Backend::PlayAudioFile(const std::string& filename)
	{
		const auto it = filenameWaveMap.find(CanonicalisePath(filename)); // Canonical path - Wave data ID
		if (it == filenameWaveMap.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Audio file not loaded: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		auto waveIt = waveDataCache.find(it->second);
		if (waveIt == waveDataCache.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Wave data missing for loaded audio file: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		return PlayAudioWave(waveIt->second);
	}

	FRANAUDIO_API size_t Backend::PlayAudioFileStream(const std::string& filename, bool looping)
	{
		// Default implementation for backends without native streaming: full in-memory decode.
		FranAudioShared::Logger::LogWarning(std::format("{}: Streaming is not supported by this backend, falling back to a full decode for: {}", GetBackendName(), filename));

		if (LoadAudioFile(filename) == SIZE_MAX)
		{
			return SIZE_MAX;
		}

		const size_t soundID = PlayAudioFile(filename);

		if (soundID != SIZE_MAX && looping)
		{
			SetSoundLooping(soundID, true);
		}

		return soundID;
	}

	FRANAUDIO_API bool Backend::UnloadAudioFile(const std::string& filename)
	{
		CleanupFinishedSounds();

		const auto it = filenameWaveMap.find(CanonicalisePath(filename));
		if (it == filenameWaveMap.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Cannot unload audio file that is not loaded: {}", GetBackendName(), filename));
			return false;
		}

		const size_t waveDataID = it->second;

		for (const auto& [soundID, sound] : activeSounds)
		{
			if (sound.GetWaveDataID() == waveDataID)
			{
				FranAudioShared::Logger::LogError(std::format("{}: Cannot unload audio file still in use by sound {}: {}", GetBackendName(), soundID, filename));
				return false;
			}
		}

		// The ID is never reused; reloading the file later gets a fresh ID.
		waveDataCache.erase(waveDataID);
		filenameWaveMap.erase(it);

		FranAudioShared::Logger::LogMessage(std::format("{}: Unloaded audio file: {}", GetBackendName(), filename));
		return true;
	}

	const FRANAUDIO_API FranAudioShared::Containers::UnorderedMap<size_t, FranAudio::Sound::WaveData>& Backend::GetWaveDataCache()
	{
		return waveDataCache;
	}

	// ========================
	// Sound Management
	// ========================

	void Backend::CleanupFinishedSoundsThrottled()
	{
		const auto now = std::chrono::steady_clock::now();

		if (now - lastCleanupTime < cleanupInterval)
		{
			return;
		}

		lastCleanupTime = now;
		CleanupFinishedSounds();
	}

	FRANAUDIO_API bool Backend::IsSoundValid(size_t soundIndex)
	{
		if (soundIndex == SIZE_MAX)
		{
			return false;
		}

		CleanupFinishedSoundsThrottled();

		if (!activeSounds.contains(soundIndex))
		{
			return false;
		}

		return !IsSoundFinished(soundIndex);
	}

	FRANAUDIO_API void Backend::SetSoundVolume(size_t soundID, float volume)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set volume of an invalid sound.", GetBackendName()));
			return;
		}

		soundBaseVolumes[soundID] = volume;
		SetSoundVolumeRaw(soundID, volume * GetGroupVolumeForSound(soundID));
	}

	FRANAUDIO_API float Backend::GetSoundVolume(size_t soundID)
	{
		if (const auto it = soundBaseVolumes.find(soundID); it != soundBaseVolumes.end())
		{
			return it->second;
		}

		// No multiplier yet.
		return GetSoundVolumeRaw(soundID);
	}

	// ========================
	// Sound Groups
	// ========================

	float Backend::GetGroupVolumeForSound(size_t soundID) const
	{
		return GetGroupVolume(GetSoundGroup(soundID));
	}

	FRANAUDIO_API void Backend::SetSoundGroup(size_t soundID, const std::string& groupName)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set group of an invalid sound.", GetBackendName()));
			return;
		}

		if (!soundBaseVolumes.contains(soundID))
		{
			const float previousGroupVolume = GetGroupVolumeForSound(soundID);
			soundBaseVolumes[soundID] = previousGroupVolume > 0.0f ? GetSoundVolumeRaw(soundID) / previousGroupVolume : 1.0f;
		}

		const std::string targetGroup = groupName.empty() ? FranAudioShared::defaultSoundGroupName : groupName;
		soundGroups[soundID] = targetGroup;
		SetSoundVolumeRaw(soundID, soundBaseVolumes[soundID] * GetGroupVolume(soundGroups[soundID]));

		if (IsGroupExclusive(targetGroup))
		{
			// The group can only ever hold one sound, so the occupant is known without looking at any of the others.
			if (const auto it = exclusiveGroupSound.find(targetGroup); it != exclusiveGroupSound.end() && it->second != soundID)
			{
				StopPlayingSound(it->second);
			}

			exclusiveGroupSound[targetGroup] = soundID;
		}
	}

	const FRANAUDIO_API std::string& Backend::GetSoundGroup(size_t soundID) const
	{
		if (const auto it = soundGroups.find(soundID); it != soundGroups.end())
		{
			return it->second;
		}

		// Every sound belongs to a group. Unassigned sounds are in the default group.
		static const std::string defaultGroup = FranAudioShared::defaultSoundGroupName;
		return defaultGroup;
	}

	FRANAUDIO_API void Backend::SetGroupVolume(const std::string& groupName, float volume)
	{
		const std::string targetGroup = groupName.empty() ? FranAudioShared::defaultSoundGroupName : groupName;

		// Needed to recover base volumes of first-touched sounds. Be careful.
		const float oldVolume = GetGroupVolume(targetGroup);
		groupVolumes[targetGroup] = volume;

		// Lazily drop state of sounds that finished or were stopped.
		for (auto it = soundGroups.begin(); it != soundGroups.end();)
		{
			if (!IsSoundValid(it->first))
			{
				if (const auto exclusiveIt = exclusiveGroupSound.find(it->second); exclusiveIt != exclusiveGroupSound.end() && exclusiveIt->second == it->first)
				{
					exclusiveGroupSound.erase(exclusiveIt);
				}

				soundBaseVolumes.erase(it->first);
				it = soundGroups.erase(it);
				continue;
			}

			++it;
		}

		// Iterate all active sounds so members of the default group are reachable too.
		for (const auto& soundID : activeSounds | std::views::keys)
		{
			if (GetSoundGroup(soundID) != targetGroup)
			{
				continue;
			}

			if (!soundBaseVolumes.contains(soundID))
			{
				soundBaseVolumes[soundID] = oldVolume > 0.0f ? GetSoundVolumeRaw(soundID) / oldVolume : 1.0f;
			}

			SetSoundVolumeRaw(soundID, soundBaseVolumes[soundID] * volume);
		}
	}

	FRANAUDIO_API float Backend::GetGroupVolume(const std::string& groupName) const
	{
		if (const auto it = groupVolumes.find(groupName.empty() ? FranAudioShared::defaultSoundGroupName : groupName); it != groupVolumes.end())
		{
			return it->second;
		}

		return 1.0f;
	}

	FRANAUDIO_API void Backend::SetGroupExclusive(const std::string& groupName, bool exclusive)
	{
		const std::string targetGroup = groupName.empty() ? FranAudioShared::defaultSoundGroupName : groupName;

		// Every sound which was never assigned a group reports as a member of the default one, so making it exclusive would let a single sound cut all of them.
		if (targetGroup == FranAudioShared::defaultSoundGroupName && exclusive)
		{
			FranAudioShared::Logger::LogError(std::format("{}: The default sound group cannot be made exclusive.", GetBackendName()));
			return;
		}

		groupExclusive[targetGroup] = exclusive;

		if (!exclusive)
		{
			exclusiveGroupSound.erase(targetGroup);
			return;
		}

		CleanupFinishedSounds();

		size_t newestSoundID = SIZE_MAX;
		for (const auto& soundID : activeSounds | std::views::keys)
		{
			if (GetSoundGroup(soundID) == targetGroup && (newestSoundID == SIZE_MAX || soundID > newestSoundID))
			{
				newestSoundID = soundID;
			}
		}

		FranAudioShared::Containers::Vector<size_t> soundsToStop;
		for (const auto& soundID : activeSounds | std::views::keys)
		{
			if (soundID != newestSoundID && GetSoundGroup(soundID) == targetGroup)
			{
				soundsToStop.push_back(soundID);
			}
		}

		for (const size_t soundID : soundsToStop)
		{
			StopPlayingSound(soundID);
		}

		// Seed the index so later assignments can take the fast path.
		if (newestSoundID != SIZE_MAX)
		{
			exclusiveGroupSound[targetGroup] = newestSoundID;
		}
	}

	FRANAUDIO_API bool Backend::IsGroupExclusive(const std::string& groupName) const
	{
		const std::string targetGroup = groupName.empty() ? FranAudioShared::defaultSoundGroupName : groupName;
		if (const auto it = groupExclusive.find(targetGroup); it != groupExclusive.end())
		{
			return it->second;
		}

		return false;
	}

	FRANAUDIO_API void Backend::SetSoundPositions(std::span<const FranAudioShared::SoundPositionUpdate> positions)
	{
		for (const auto& update : positions)
		{
			if (!IsSoundValid(update.soundID))
			{
				continue;
			}

			SetSoundPosition(update.soundID, update.position);
		}
	}

	void Backend::OnSoundRemoved(size_t soundID)
	{
		// Read the group before erasing it below, so an exclusive group is released when the sound holding it ends on its own rather than being replaced.
		if (const auto groupIt = soundGroups.find(soundID); groupIt != soundGroups.end())
		{
			if (const auto it = exclusiveGroupSound.find(groupIt->second); it != exclusiveGroupSound.end() && it->second == soundID)
			{
				exclusiveGroupSound.erase(it);
			}
		}

		soundGroups.erase(soundID);
		soundBaseVolumes.erase(soundID);
	}

	FRANAUDIO_API FranAudio::Sound::Sound& Backend::GetSound(size_t soundID)
	{
		return activeSounds[soundID];
	}

	const FRANAUDIO_API FranAudioShared::Containers::UnorderedMap<size_t, FranAudio::Sound::Sound>& Backend::GetActiveSounds()
	{
		CleanupFinishedSounds();
		return activeSounds;
	}

	const FRANAUDIO_API FranAudioShared::Containers::Vector<size_t> Backend::GetActiveSoundIDs()
	{
		CleanupFinishedSounds();
		FranAudioShared::Containers::Vector<size_t> soundIDs;

		soundIDs.clear();
		soundIDs.reserve(activeSounds.size());

		for (const auto& soundID : activeSounds | std::views::keys)
		{
			soundIDs.push_back(soundID);
		}

		return soundIDs;
	}

	FRANAUDIO_API std::unique_ptr<FranAudio::Backend::Backend> Backend::CreateBackend(BackendType backendType)
	{
		std::unique_ptr<FranAudio::Backend::Backend> newBackend = nullptr;

		switch (backendType)
		{
		case BackendType::miniaudio:
			newBackend = std::make_unique<FranAudio::Backend::miniaudio>();
			break;
#ifdef FRANAUDIO_USE_OPENAL
		case BackendType::OpenALSoft:
			newBackend = std::make_unique<FranAudio::Backend::OpenALSoft>();
			break;
#endif
		default:
			return nullptr;
			break;
		}

		if (!newBackend->Init())
		{
			return nullptr;
		}

		return newBackend;
	}
}
