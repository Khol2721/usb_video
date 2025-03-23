#include "MyElapsedTimer.h"

MyElapsedTimer::MyElapsedTimer() :
    timeStartWorkEpochMs(0), timeStartPauseEpochMs(0), currentTimeMs(0), delPauseMs(0), addTimeMs(0) { }

void MyElapsedTimer::Start() {
    timeStartWorkEpochMs = QDateTime::currentMSecsSinceEpoch();
    timeStartPauseEpochMs = 0;
    currentTimeMs = 0;
    delPauseMs = 0;
    addTimeMs = 0;
}

void MyElapsedTimer::SetPause(bool isPause) {

    if(GetIsStop()) {
        return;
    }

    if(isPause == GetIsPause()) {
        return;
    }

    if(isPause) {
        timeStartPauseEpochMs = QDateTime::currentMSecsSinceEpoch();
    }
    else {
        delPauseMs += QDateTime::currentMSecsSinceEpoch() - timeStartPauseEpochMs;
        timeStartPauseEpochMs = 0;
    }
}

void MyElapsedTimer::Stop() {
    timeStartWorkEpochMs = 0;
    timeStartPauseEpochMs = 0;
    currentTimeMs = 0;
    delPauseMs = 0;
    addTimeMs = 0;
}

void MyElapsedTimer::Restart() {
    Start();
}

bool MyElapsedTimer::GetIsStop() const {
    return timeStartWorkEpochMs <= 0;
}

bool MyElapsedTimer::GetIsPause() const {
    return timeStartPauseEpochMs > 0;
}

qint64 MyElapsedTimer::GetElapsedMs() {
    currentTimeMs = GetConstElapsedMs();
    return currentTimeMs;
}

qint64 MyElapsedTimer::GetConstElapsedMs() const {

    if(GetIsStop()) {
        return currentTimeMs;
    }

    if(!GetIsPause()) {
        return QDateTime::currentMSecsSinceEpoch() - timeStartWorkEpochMs - delPauseMs + addTimeMs;
    }
    else {
        return timeStartPauseEpochMs - timeStartWorkEpochMs - delPauseMs + addTimeMs;
    }
}

void MyElapsedTimer::AddElapsedMs(qint64 value) {
    addTimeMs += value;
}
