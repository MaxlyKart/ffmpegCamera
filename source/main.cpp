#include "mPlayer.h"
#include "mRecorder.h"

int main (int argc, char** args) {
    mPlayer *player = new mPlayer(CAM, VFW_CAP_DEV);
    mRecorder *recorder = new mRecorder("./outputfile.mpeg");
    mPlayer::showDShowDevice();
    player->setRecorder(recorder);
    player->SDLDisplay();
    delete recorder;
    delete player;
    system("pause");
    return 0;
}