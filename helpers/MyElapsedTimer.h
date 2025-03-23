#ifndef MYELAPSEDTIMER_H
#define MYELAPSEDTIMER_H

#include <QDateTime>

class MyElapsedTimer
{
public:
    MyElapsedTimer();
    virtual ~MyElapsedTimer() = default;

    void Start();

    void SetPause(bool isPause);

    void Stop();

    void Restart();

    bool GetIsStop() const;

    bool GetIsPause() const;

    qint64 GetElapsedMs();

    qint64 GetConstElapsedMs() const;

    void AddElapsedMs(qint64 addTimeMs);

private:
    qint64 timeStartWorkEpochMs;
    qint64 timeStartPauseEpochMs;
    qint64 currentTimeMs;
    qint64 delPauseMs;
    qint64 addTimeMs;
};

#endif // MYELAPSEDTIMER_H
