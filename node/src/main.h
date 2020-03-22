#ifndef __MAIN_INCLUDED__
#define __MAIN_INCLUDED__
#include <cubeshellconfig.h>
#include <queue>
#include "MContainer.h"

bool isStopped();
cubeshell::IConfig *getConfig();
std::queue<MContainer>& getContainerQueue();

#endif
