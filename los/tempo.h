//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: tempo.h,v 1.2.2.1 2006/09/19 19:07:09 spamatica Exp $
//
//  (C) Copyright 1999/2000 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __TEMPO_H__
#define __TEMPO_H__

#include <map>

#ifndef MAX_TICK
#define MAX_TICK (0x7fffffff/100)
#endif

class Xml;

//---------------------------------------------------------
//   Tempo Event
//---------------------------------------------------------

struct TEvent
{
    int tempo;
    unsigned tick; // new tempo at tick
    unsigned frame; // precomputed time for tick in sec

    int read(Xml&);
    void write(int, Xml&, int) const;

    TEvent()
    {
    }

    TEvent(unsigned t, unsigned tk)
    {
        tempo = t;
        tick = tk;
        frame = 0;
    }
};

//---------------------------------------------------------
//   TempoList
//---------------------------------------------------------

typedef std::map<unsigned, TEvent*, std::less<unsigned> > TEMPOLIST;
typedef TEMPOLIST::iterator iTEvent;
typedef TEMPOLIST::const_iterator ciTEvent;
typedef TEMPOLIST::reverse_iterator riTEvent;
typedef TEMPOLIST::const_reverse_iterator criTEvent;

class TempoList : public TEMPOLIST
{
    int _tempoSN; // serial no to track tempo changes
    bool useList;
    int _tempo; // tempo if not using tempo list
    int _globalTempo; // %percent 50-200%

    void normalize();
    void add(unsigned tick, int tempo);
    void change(unsigned tick, int newTempo);
    void del(iTEvent);
    void del(unsigned tick);

public:
    TempoList();
    void clear();

    void read(Xml&);
    void write(int, Xml&) const;
    void dump() const;

    int tempo(unsigned tick) const;
    unsigned tick2frame(unsigned tick, unsigned frame, int* sn) const;
    unsigned tick2frame(unsigned tick, int* sn = 0) const;
    unsigned frame2tick(unsigned frame, int* sn = 0) const;
    unsigned frame2tick(unsigned frame, unsigned tick, int* sn) const;
    unsigned deltaTick2frame(unsigned tick1, unsigned tick2, int* sn = 0) const;
    unsigned deltaFrame2tick(unsigned frame1, unsigned frame2, int* sn = 0) const;

    int tempoSN() const
    {
        return _tempoSN;
    }
    void setTempo(unsigned tick, int newTempo);
    void addTempo(unsigned t, int tempo);
    void delTempo(unsigned tick);
	void delTempoRange(unsigned start, unsigned end);
    void changeTempo(unsigned tick, int newTempo);
    bool setMasterFlag(unsigned tick, bool val);

    int globalTempo() const
    {
        return _globalTempo;
    }
    void setGlobalTempo(int val);
};

extern TempoList tempomap;
#endif
