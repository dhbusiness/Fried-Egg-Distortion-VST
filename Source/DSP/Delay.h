/*
  ==============================================================================

    Delay.h
    Created: 21 Oct 2022 4:23:16pm
    Author:  Deivids

  ==============================================================================
*/

#pragma once


#include <iostream>
#include <vector>
#include <math.h>


class DelayUnit {
    std::vector<float> m_buffer;
    int m_bufferSize;
    int m_writePos;
public:
    DelayUnit(int delaySize) :
        m_bufferSize(delaySize),
        m_writePos(0)
    {
        
        for(int n = 0 ; n < m_bufferSize ;++n)
        {
            m_buffer.push_back(0);
        }
        
        std::cout << "Delay Constructed Size:"<< m_bufferSize <<std::endl;
    }
    ///Don't call this in the audio thread!
    void printBuffer()
    {
        for(auto sample : m_buffer)
        {
            std::cout << sample<<std::endl;
        }
    }
    void clearBuffer()
    {
        for(int n = 0 ; n < m_bufferSize; ++n)
        {
            m_buffer[n] = 0;
        }
    }
    void sum(float value)
    {
        m_buffer[ m_writePos ] += value;
        
        ++m_writePos;
        
        while(m_writePos >= m_bufferSize)m_writePos -= m_bufferSize;
        
    }
    void replace(float value)
    {
        m_buffer[ m_writePos ] = value;
        
        ++m_writePos;
        
        while(m_writePos >= m_bufferSize)m_writePos -= m_bufferSize;
        
    }
    
    float getSample( int delayTime  )
    {
        
        int delayPos = m_writePos - delayTime;
        
        while(delayPos < 0 )delayPos += m_bufferSize;
        while(delayPos >= m_bufferSize)delayPos -= m_bufferSize;
        
        return m_buffer[delayPos];
        
        
    }
    
    float getSample(double delayTime )
    {
        
        double delayPos = m_writePos - delayTime;
        
        double previousPosition;
        float fraction = modf(delayPos,&previousPosition );
        
        double nextPosition = previousPosition + 1;
        
        while(previousPosition < 0 )previousPosition += m_bufferSize;
        while(previousPosition >= m_bufferSize)previousPosition -= m_bufferSize;
        
        while(nextPosition < 0 )nextPosition += m_bufferSize;
        while(nextPosition >= m_bufferSize)nextPosition -= m_bufferSize;
        
        
        float previousValue = m_buffer[previousPosition];
        float nextValue = m_buffer[nextPosition];

        return ((1-fraction)*previousValue) + (nextValue * fraction);
        
    }
    
    
    
    
};
