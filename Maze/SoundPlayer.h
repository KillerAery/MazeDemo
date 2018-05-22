#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

namespace SoundName {
	const int walk0 = 0;
	const int walk1 = 1;
	const int BGM = 2;
}


class SoundPlayer
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundPlayer();
	SoundPlayer(const SoundPlayer&);
	~SoundPlayer();

	bool Initialize(HWND);
	bool PlayWaveFile(int index, bool loop = false);
	void Shutdown();
private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(const char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffers[3];
};

#endif