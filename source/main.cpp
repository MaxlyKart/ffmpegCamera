#include "mPlayer.h"
#include "mRecorder.h"

int main (int argc, char** args) {
    mPlayer *player = new mPlayer(CAM, D_SHOW_DEV);
    player->showDShowDevice();
    player->SDLDisplay();
    delete player;
    system("pause");
    return 0;
}