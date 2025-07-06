#pragma once
#include <functional>

class TimedCall {
public:
    TimedCall(std::function<void()> callback, float time, bool isLoop)
        : callback_(callback), callTime_(time), isLoop_(isLoop) {
    }

    void Update();

    void Stop() {
        isFinished_ = true;
    }

    bool IsLoop() const {
        return isLoop_;
    }
    bool IsFinished() const {
        return isFinished_;
    }

    void SetCallback(std::function<void()> callback) {
        callback_ = callback;
    }
    void SetCallTime(float time) {
        callTime_ = time;
    }
    void SetElapsedTime(float elapsed) {
        elapsedTime_ = elapsed;
    }
    void SetLoop(bool isLoop) {
        isLoop_ = isLoop;
    }

    std::function<void()> GetCallback() const {
        return callback_;
    }
    float GetCallTime() const {
        return callTime_;
    }
    float GetElapsedTime() const {
        return elapsedTime_;
    }

private:
    std::function<void()> callback_;
    float callTime_ = 0.0f;
    float elapsedTime_ = 0.0f;
    bool isLoop_ = false;
    bool isFinished_ = false;
};
