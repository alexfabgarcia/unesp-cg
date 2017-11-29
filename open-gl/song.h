#include <windows.h>
#include <mmsystem.h>

void stopAllPlayingSongs() {
	PlaySound(NULL, 0, 0);
}

void playSoundStoppingOthers(LPCTSTR pszSound) {
	stopAllPlayingSongs();
	if (!paused) {
		PlaySound(pszSound, NULL, SND_ASYNC|SND_FILENAME);
	}
}
