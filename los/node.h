//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: node.h,v 1.8.2.2 2006/04/13 19:09:48 spamatica Exp $
//
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __AUDIONODE_H__
#define __AUDIONODE_H__

#include <list>

#ifndef i386
#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    int counter;
} los_atomic_t;
#else

typedef struct {
    int counter;
} los_atomic_t;
#endif

static inline int los_atomic_read(los_atomic_t *v) {
#ifndef i386
    int ret;
    pthread_mutex_lock(&v->lock);
    ret = v->counter;
    pthread_mutex_unlock(&v->lock);
    return ret;
#else
    return v->counter;
#endif
}

static inline void los_atomic_set(los_atomic_t *v, int i) {
#ifndef i386
    pthread_mutex_lock(&v->lock);
    v->counter = i;
    pthread_mutex_unlock(&v->lock);
#else
    v->counter = i;
#endif
}

static inline void los_atomic_inc(los_atomic_t *v) {
#ifndef i386
    pthread_mutex_lock(&v->lock);
    v->counter++;
    pthread_mutex_unlock(&v->lock);
#else
    __asm__ __volatile__(
            "lock ; " "incl %0"
            : "=m" (v->counter)
            : "m" (v->counter));
#endif
}

static inline void los_atomic_dec(los_atomic_t *v) {
#ifndef i386
    pthread_mutex_lock(&v->lock);
    v->counter--;
    pthread_mutex_unlock(&v->lock);
#else
    __asm__ __volatile__(
            "lock ; " "decl %0"
            : "=m" (v->counter)
            : "m" (v->counter));
#endif
}
#ifndef i386

static inline void los_atomic_init(los_atomic_t *v) {
    pthread_mutex_init(&v->lock, NULL);
}
#else

static inline void los_atomic_init(los_atomic_t*) {
}
#endif

#ifndef i386

static inline void los_atomic_destroy(los_atomic_t *v) {
    pthread_mutex_destroy(&v->lock);
}
#else

static inline void los_atomic_destroy(los_atomic_t*) {
}
#endif

class Xml;
class Pipeline;
class SndFile;

// superceeded by dynamic allocation of fifoLength
//const int FIFO_BUFFER = 4096;//64;

//---------------------------------------------------------
//   Fifo
//---------------------------------------------------------

struct FifoBuffer {
    float* buffer;
    int size;
    int maxSize;
    unsigned pos;
    int segs;

    FifoBuffer() {
        buffer = 0;
        size = 0;
        maxSize = 0;
    }
};

class Fifo {
    int nbuffer;
    int ridx; // read index; only touched by reader
    int widx; // write index; only touched by writer
    los_atomic_t count; // buffer count; writer increments, reader decrements
    FifoBuffer** buffer;

public:
    Fifo();
    ~Fifo();

    void clear() {
        ridx = 0;
        widx = 0;
        los_atomic_set(&count, 0);
    }
    bool put(int, unsigned long, float** buffer, unsigned pos);
    bool getWriteBuffer(int, unsigned long, float** buffer, unsigned pos);
    void add();
    bool get(int, unsigned long, float** buffer, unsigned* pos);
    void remove();
    int getCount();
};

#endif

