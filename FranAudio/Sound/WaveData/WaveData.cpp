// FranticDreamer 2022-2025

#include <algorithm>

#include "WaveData.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

FranAudio::Sound::WaveData::WaveData(const std::string& filename, WaveFormat format, double length, int channels, int sampleRate)
	: filename(filename), format(format), length(length), channels(channels), sampleRate(sampleRate), frameSize(0)
{
	//sizeInFrames = frames.size() / channels;
	//frameSize = sizeof(float) * channels;
	//length = static_cast<double>(frames.size()) / (sampleRate * channels);
}

void FranAudio::Sound::WaveData::SetFilename(const std::string& filename)
{
	this->filename = filename;
}

void FranAudio::Sound::WaveData::SetFormat(WaveFormat format)
{
	this->format = format;
}

void FranAudio::Sound::WaveData::SetLength(double length)
{
	this->length = length;
}

void FranAudio::Sound::WaveData::SetChannels(char channels)
{
	this->channels = channels;
}

void FranAudio::Sound::WaveData::SetSampleRate(int sampleRate)
{
	this->sampleRate = sampleRate;
}

const std::string& FranAudio::Sound::WaveData::GetFilename() const
{
	return filename;
}

FranAudio::Sound::WaveFormat FranAudio::Sound::WaveData::GetFormat() const
{
	return format;
}

double FranAudio::Sound::WaveData::GetLength() const
{
	return length;
}

char FranAudio::Sound::WaveData::GetChannels() const
{
	return channels;
}

int FranAudio::Sound::WaveData::GetSampleRate() const
{
	return sampleRate;
}

const size_t FranAudio::Sound::WaveData::SizeInFrames() const
{
	return framesFloat.size() / channels;
}

void FranAudio::Sound::WaveData::SetFrameSize(unsigned char frameSize)
{
	this->frameSize = frameSize;
}

unsigned char FranAudio::Sound::WaveData::GetFrameSize() const
{
	return frameSize;
}

void FranAudio::Sound::WaveData::SetFrames(const std::vector<float>& frames)
{
	framesFloat = frames;
}

std::vector<float>& FranAudio::Sound::WaveData::GetFramesRef()
{
	return framesFloat;
}

const std::vector<float>& FranAudio::Sound::WaveData::GetFrames() const
{
	return framesFloat;
}