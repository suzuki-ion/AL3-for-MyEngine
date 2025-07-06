#include <KashipanEngine.h>
#include "TimedCall.h"

void TimedCall::Update() {
    if (isFinished_) {
        return;
    }

    elapsedTime_ += Engine::GetDeltaTime();
    if (elapsedTime_ < callTime_) {
        return;
    }

    callback_();
    if (isLoop_) {
        elapsedTime_ = 0.0f;
    } else {
        isFinished_ = true;
    }
}