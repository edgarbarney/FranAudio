// FranticDreamer 2022-2025

#include <algorithm>

#include "WaveData.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Sound
{
	WaveData::WaveData(const std::string& filename, size_t waveDataIndex, WaveFormat format, double length, int channels, int sampleRate)
		: filename(filename), waveDataIndex(waveDataIndex), format(format), length(length), channels(channels), sampleRate(sampleRate), frameSize(0)
	{
		//sizeInFrames = frames.size() / channels;
		//frameSize = sizeof(float) * channels;
		//length = static_cast<double>(frames.size()) / (sampleRate * channels);
	}

	void WaveData::SetFilename(const std::string& filename)
	{
		this->filename = filename;
	}

	void WaveData::SetWaveDataIndex(size_t index)
	{
		this->waveDataIndex = index;
	}

	void WaveData::SetFormat(WaveFormat format)
	{
		this->format = format;
	}

	void WaveData::SetLength(double length)
	{
		this->length = length;
	}

	void WaveData::SetChannels(char channels)
	{
		this->channels = channels;
	}

	void WaveData::SetSampleRate(int sampleRate)
	{
		this->sampleRate = sampleRate;
	}

	const std::string& WaveData::GetFilename() const
	{
		return filename;
	}

	size_t WaveData::GetWaveDataIndex() const
	{
		return waveDataIndex;
	}

	WaveFormat WaveData::GetFormat() const
	{
		return format;
	}

	double WaveData::GetLength() const
	{
		return length;
	}

	char WaveData::GetChannels() const
	{
		return channels;
	}

	int WaveData::GetSampleRate() const
	{
		return sampleRate;
	}

	const size_t WaveData::SizeInFrames() const
	{
		//return frames.size() / channels;

		return std::visit([this](auto const& vec) -> size_t
		{
			return vec.size() / static_cast<size_t>(channels);
		}, frames);
	}

	void WaveData::SetFrameSize(unsigned char frameSize)
	{
		this->frameSize = frameSize;
	}

	unsigned char WaveData::GetFrameSize() const
	{
		return frameSize;
	}

	void WaveData::SetFrames(const SampleFrameContainer& frames)
	{
		this->frames = frames;
	}

	SampleFrameContainer& WaveData::GetFramesRef()
	{
		return frames;
	}

	const SampleFrameContainer& WaveData::GetFrames() const
	{
		return frames;
	}
}
