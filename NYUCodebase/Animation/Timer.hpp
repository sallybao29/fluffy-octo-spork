#ifndef Timer_hpp
#define Timer_hpp
#include <stdio.h>

/*!
 * @discussion A class for keeping and checking time
 */
class Timer {
public:
    Timer();
    
    /*!
     * @discussion Start the timer
     */
    void start();
    
    /*!
     * @discussion Check if the timer has been running for more than a given amount of time
     * @param seconds Number of seconds
     * @return Whether the elapsed time since the timer started is greater than seconds
     */
    bool isOver(float seconds) const;
    
    /*!
     * @discussion Check the status of the timer
     * @return Whether the timer is running
     */
    bool isRunning() const;
    
    /*!
     * @discussion Get the amount of time that has elapsed since the timer started running.
     * Value will be 0 if the timer is not running.
     * @return The amount of time that has elapsed
     */
    float getElapsed() const;
    
    /*!
     * @discussion Reset the timer
     */
    void reset();
    
    /*!
     * @discussion Stop the timer
     */
    void stop();
    
private:
    float beg;
    float end;
    bool running;
};

#endif
