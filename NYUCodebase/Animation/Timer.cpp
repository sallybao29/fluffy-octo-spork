#include <SDL.h>
#include "Timer.hpp"

Timer::Timer() : beg(0), running(false), end(0) {}

void Timer::start() {
    beg = (float)SDL_GetTicks() / 1000.0f;
    running = true;
}

float Timer::getElapsed() const {
    if (running)
        return (float)SDL_GetTicks() / 1000.0f - beg;
    else
        return end - beg;
}

bool Timer::isOver(float seconds) const {
    return seconds < getElapsed();
}

bool Timer::isRunning() const {
    return running;
}

void Timer::reset() {
    beg = 0;
    running = false;
    end = 0;
}

void Timer::stop() {
    running = false;
    end = (float)SDL_GetTicks() / 1000.0f;
}
