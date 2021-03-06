//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: sig.h,v 1.2 2004/01/11 18:55:34 wschweer Exp $
//
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __SIG_H__
#define __SIG_H__

#include <map>

#ifndef MAX_TICK
#define MAX_TICK (0x7fffffff/100)
#endif

class Xml;

//---------------------------------------------------------
//   Signature Event
//---------------------------------------------------------

struct SigEvent
{
    int z, n; // takt signatur
    unsigned tick; // ab dieser Position gilt signatur
    int bar; // precomputed

    int read(Xml&);
    void write(int, Xml&, int) const;

    SigEvent()
    {
    }

    SigEvent(int Z, int N, unsigned tk)
    {
        z = Z;
        n = N;
        tick = tk;
        bar = 0;
    }
};

//---------------------------------------------------------
//   SigList
//---------------------------------------------------------

typedef std::map<unsigned, SigEvent*, std::less<unsigned> > SIGLIST;
typedef SIGLIST::iterator iSigEvent;
typedef SIGLIST::const_iterator ciSigEvent;
typedef SIGLIST::reverse_iterator riSigEvent;
typedef SIGLIST::const_reverse_iterator criSigEvent;

class SigList : public SIGLIST
{
    int ticks_beat(int N) const;
    void normalize();
    int ticksMeasure(int z, int n) const;

public:
    SigList();
    void clear();
    void add(unsigned tick, int z, int n);
    void del(unsigned tick);

    void read(Xml&);
    void write(int, Xml&) const;
    void dump() const;

    void timesig(unsigned tick, int& z, int& n) const;
    void tickValues(unsigned t, int* bar, int* beat, unsigned* tick) const;
    unsigned bar2tick(int bar, int beat, unsigned tick) const;

    int ticksMeasure(unsigned tick) const;
    int ticksBeat(unsigned tick) const;
    unsigned raster(unsigned tick, int raster) const;
    unsigned raster1(unsigned tick, int raster) const;
    unsigned raster2(unsigned tick, int raster) const;
    int rasterStep(unsigned tick, int raster) const;
};

extern SigList sigmap;
#endif
