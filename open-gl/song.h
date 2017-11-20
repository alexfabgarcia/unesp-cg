#include <windows.h>
#include <mmsystem.h>

void stopAllPlayingSongs() {
	PlaySound(NULL, 0, 0);
}

void playSoundStoppingOthers(LPCTSTR pszSound) {
	stopAllPlayingSongs();
	PlaySound(pszSound, NULL, SND_ASYNC|SND_FILENAME);
}
