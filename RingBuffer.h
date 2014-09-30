#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_
/* 'Thread'safe RingBuffer for Atmega.  
 * (c) 2011, Christopher "ScribbleJ" Jansen
 *
 * Note: Safety only assured if all writes from the same 'thread' and all reads from the same 'thread'.  Mixing them is no good.
 *
 */

#include <util/atomic.h>

typedef uint16_t RB_SIZE_TYPE;

template<typename T> class RingBufferT
{
  public:
    typedef T DTYPE;
  private:
    const RB_SIZE_TYPE size;
    volatile RB_SIZE_TYPE count;
    DTYPE* start;
    DTYPE* end;
    DTYPE* head;
    DTYPE* tail;
  public:
    RingBufferT(RB_SIZE_TYPE size, DTYPE* data) :
      size(size), count(0), start(data), end(data + size), head(data), tail(data) 
    {};

    inline void reset()
    {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        head = start;
        tail = start;
        count = 0;
      }
    }

    inline void push(DTYPE const& d)
    {
      *tail = d;
      if(++tail == end)
        tail = start;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        count++;
      }
    }

    inline DTYPE pop()
    {
      DTYPE d = *head;
      if(++head == end)
        head = start;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        count--;
      }
      return d;
    }

    inline RB_SIZE_TYPE getCount()
    {
      RB_SIZE_TYPE c;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        c = count;
      }
      return c;
    }

    inline RB_SIZE_TYPE getCapacity()
    {
      return (size - getCount());
    }

    inline bool isEmpty()
    {
      return (getCount() == 0);
    }

    inline bool isFull()
    {
      return (getCount() >= size);
    }
    
    inline DTYPE& peek(RB_SIZE_TYPE index)
    {
      //return *(head+index);
      DTYPE *t = head;
      for(;index > 0;index--)
      {
        t++;
        if(t == end)
          t = start;
      }
      return *(t);

    }

    inline void remove(RB_SIZE_TYPE count_in)
    {
      for(RB_SIZE_TYPE c = count_in;c > 0;c--)
      {
        if(++head == end)
          head = start;
      }

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        count-=count_in;
      }
    }

    inline DTYPE& getNextWrite(RB_SIZE_TYPE index)
    {
      DTYPE* dp = tail;
      while(index > 0)
      {
        index--;
        dp++;
        if(dp == end) dp = start;
      }
      return *dp;
    }

    inline void finishWrite()
    {
      tail++;
      if(tail == end)
        tail = start;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        count++;
      }
    }

        
};


      
    



#endif // _RINGBUFFER_H_
