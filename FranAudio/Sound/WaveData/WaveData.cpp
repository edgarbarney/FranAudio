// FranticDreamer 2022-2025

#include <algorithm>

#include "WaveData.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Sound
{
	FRANAUDIO_API WaveData::WaveData(const std::string& filename, size_t waveDataIndex, WaveFormat format, double length, int channels, int sampleRate, bool isPersistent)
		: filename(filename), waveDataIndex(waveDataIndex), format(format), length(length), channels(channels), sampleRate(sampleRate), frameSize(0), isPersistent(isPersistent)
	{
		//sizeInFrames = frames.size() / channels;
		//frameSize = sizeof(float) * channels;
		//length = static_cast<double>(frames.size()) / (sampleRate * channels);
	}

	FRANAUDIO_API void WaveData::SetFilename(const std::string& filename)
	{
		this->filename = filename;
	}

	FRANAUDIO_API void WaveData::SetWaveDataIndex(size_t index)
	{
		this->waveDataIndex = index;
	}

	FRANAUDIO_API void WaveData::SetFormat(WaveFormat format)
	{
		this->format = format;
	}

	FRANAUDIO_API void WaveData::SetLength(double length)
	{
		this->length = length;
	}

	FRANAUDIO_API void WaveData::SetChannels(char channels)
	{
		this->channels = channels;
	}

	FRANAUDIO_API void WaveData::SetSampleRate(int sampleRate)
	{
		this->sampleRate = sampleRate;
	}

	const FRANAUDIO_API std::string& WaveData::GetFilename() const
	{
		return filename;
	}

	FRANAUDIO_API size_t WaveData::GetWaveDataIndex() const
	{
		return waveDataIndex;
	}

	FRANAUDIO_API WaveFormat WaveData::GetFormat() const
	{
		return format;
	}

	FRANAUDIO_API double WaveData::GetLength() const
	{
		return length;
	}

	FRANAUDIO_API char WaveData::GetChannels() const
	{
		return channels;
	}

	FRANAUDIO_API int WaveData::GetSampleRate() const
	{
		return sampleRate;
	}

	FRANAUDIO_API bool WaveData::IsPersistent() const
	{
		return isPersistent;
	}

	const FRANAUDIO_API size_t WaveData::SizeInFrames() const
	{
		//return frames.size() / channels;

		return std::visit([this](auto const& vec) -> size_t
		{
			return vec.size() / static_cast<size_t>(channels);
		}, frames);
	}

	FRANAUDIO_API void WaveData::SetFrameSize(unsigned char frameSize)
	{
		this->frameSize = frameSize;
	}

	FRANAUDIO_API unsigned char WaveData::GetFrameSize() const
	{
		return frameSize;
	}

	FRANAUDIO_API void WaveData::SetFrames(const SampleFrameContainer& frames)
	{
		this->frames = frames;
	}

	FRANAUDIO_API SampleFrameContainer& WaveData::GetFramesRef()
	{
		return frames;
	}

	const FRANAUDIO_API SampleFrameContainer& WaveData::GetFrames() const
	{
		return frames;
	}

	FRANAUDIO_API void WaveData::MixToMono()
	{
		if (channels == 1)
			return;


		if (channels < 1)
		{
			FranAudioShared::Logger::LogError(std::format("WaveData::MixToMono: Invalid number of channels: {}", static_cast<int>(channels)));
			return;
		}

		std::visit([this](auto& vec)
		{
			using T = std::decay_t<decltype(vec)>;
			using SampleType = typename T::value_type;

			const size_t numChannels = static_cast<size_t>(channels);
			const size_t totalFrames = vec.size() / numChannels;

			FranAudioShared::Containers::Vector<SampleType> monoFrames(totalFrames);

			for (size_t i = 0; i < totalFrames; ++i)
			{
				double sum = 0.0;
				for (size_t ch = 0; ch < numChannels; ++ch)
				{
					sum += static_cast<double>(vec[i * numChannels + ch]);
				}
				monoFrames[i] = static_cast<SampleType>(sum / static_cast<double>(numChannels));
			}

			vec = std::move(monoFrames);

			frameSize /= numChannels;
			channels = 1;
		}, frames);
	}

	FRANAUDIO_API void WaveData::MixToStereo()
	{
		if (channels == 2)
			return;

		if (channels < 1)
		{
			FranAudioShared::Logger::LogError(std::format("WaveData::MixToStereo: Invalid number of channels: {}", static_cast<int>(channels)));
			return;
		}

		std::visit([this](auto& vec)
		{
			using T = std::decay_t<decltype(vec)>;
			using SampleType = typename T::value_type;
			
			const size_t numChannels = static_cast<size_t>(channels);
			const size_t totalFrames = vec.size() / numChannels;

			FranAudioShared::Containers::Vector<SampleType> stereoFrames(totalFrames * 2);

			for (size_t i = 0; i < totalFrames; ++i)
			{
				double sum = 0.0;
				for (size_t ch = 0; ch < numChannels; ++ch)
				{
					sum += static_cast<double>(vec[i * numChannels + ch]);
				}
				SampleType monoSample = static_cast<SampleType>(sum / static_cast<double>(numChannels));
				stereoFrames[i * 2] = monoSample;       // Left channel
				stereoFrames[i * 2 + 1] = monoSample;   // Right channel
			}

			vec = std::move(stereoFrames);

			frameSize = (frameSize / numChannels) * 2;
			channels = 2;
		}, frames);
	}
}
