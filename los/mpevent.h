//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: mpevent.h,v 1.8.2.5 2009/11/25 09:09:43 terminator356 Exp $
//
//  (C) Copyright 1999-2002 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __MPEVENT_H__
#define __MPEVENT_H__

#include <set>
#include <list>
#include "evdata.h"
#include "memory.h"
#include "config.h"

//#define MIDI_FIFO_SIZE    2100
//#define MIDI_REC_FIFO_SIZE    160

class Event;
class EvData;
class Track;

//---------------------------------------------------------
//   MEvent
//    baseclass for MidiPlayEvent and MidiRecordEvent
//---------------------------------------------------------

//---------------------------------------------------------
//   MEvent
//---------------------------------------------------------
enum EventSource {
	SystemSource = 0,
	MonitorSource,
	AudioSource
};

class MEvent
{
	EventSource m_source;
	Track *m_track;
    unsigned _time;
    EvData edata;
    unsigned char _port, _channel, _type;
    int _a, _b;
    int _loopNum; // The loop count when the note was recorded.

public:

    MEvent()
    {
        _loopNum = 0;
		m_track = 0;
		m_source = SystemSource;
    }

    MEvent(unsigned tm, int p, int c, int t, int a, int b, Track* trk= 0)
    : m_track(trk), _time(tm), _port(p), _channel(c & 0xf), _type(t), _a(a), _b(b)
    {
        _loopNum = 0;
		m_source = SystemSource;
    }
    MEvent(unsigned t, int p, int type, const unsigned char* data, int len, Track* trk = 0);

    MEvent(unsigned t, int p, int tpe, EvData d, Track* trk = 0) : m_track(trk), _time(t), edata(d), _port(p), _type(tpe)
    {
        _loopNum = 0;
		m_source = SystemSource;
    }
    MEvent(unsigned t, int port, int channel, const Event& e, Track* trk = 0);

    ~MEvent()
    {
    }

    MEvent & operator=(const MEvent& ed)
    {
        _time = ed._time;
        edata = ed.edata;
        _port = ed._port;
        _channel = ed._channel;
        _type = ed._type;
        _a = ed._a;
        _b = ed._b;
        _loopNum = ed._loopNum;
		m_track = ed.m_track;
		m_source = ed.m_source;
        return *this;
    }

    int port() const
    {
        return _port;
    }

    int channel() const
    {
        return _channel;
    }

    int type() const
    {
        return _type;
    }

    int dataA() const
    {
        return _a;
    }

    int dataB() const
    {
        return _b;
    }

    unsigned time() const
    {
        return _time;
    }

    int loopNum() const
    {
        return _loopNum;
    }

    void setPort(int val)
    {
        _port = val;
    }

    void setChannel(int val)
    {
        _channel = val;
    }

    void setType(int val)
    {
        _type = val;
    }

    void setA(int val)
    {
        _a = val;
    }

    void setB(int val)
    {
        _b = val;
    }

    void setTime(unsigned val)
    {
        _time = val;
    }

    void setLoopNum(int n)
    {
        _loopNum = n;
    }

    const EvData& eventData() const
    {
        return edata;
    }

    unsigned char* data() const
    {
        return edata.data;
    }

    int len() const
    {
        return edata.dataLen;
    }

    void setData(const EvData& e)
    {
        edata = e;
    }

    void setData(const unsigned char* p, int len)
    {
        edata.setData(p, len);
    }
    void dump() const;

    bool isNote() const
    {
        return _type == 0x90;
    }

    bool isNoteOff() const
    {
        return (_type == 0x80) || (_type == 0x90 && _b == 0);
    }
    bool operator<(const MEvent&) const;

	EventSource eventSource() const
	{
		return m_source;
	}
	void setEventSource(EventSource es)
	{
		m_source = es;
	}
	Track* track() const
	{
		return m_track;
	}
	void setTrack(Track* t)
	{
		m_track = t;
	}
};

//---------------------------------------------------------
//   MidiRecordEvent
//    allocated and deleted in midiseq thread context
//---------------------------------------------------------

class MidiPlayEvent;

class MidiRecordEvent : public MEvent
{
public:

    MidiRecordEvent() : MEvent()
    {
    }

    MidiRecordEvent(const MEvent& e) : MEvent(e)
    {
    }

    MidiRecordEvent(unsigned tm, int p, int c, int t, int a, int b)
    : MEvent(tm, p, c, t, a, b)
    {
    }

    MidiRecordEvent(unsigned t, int p, int tpe, const unsigned char* data, int len)
    : MEvent(t, p, tpe, data, len)
    {
    }

    MidiRecordEvent(unsigned t, int p, int type, EvData data)
    : MEvent(t, p, type, data)
    {
    }

    ~MidiRecordEvent()
    {
    }
};

//---------------------------------------------------------
//   MidiPlayEvent
//    allocated and deleted in audio thread context
//---------------------------------------------------------

class MidiPlayEvent : public MEvent
{
public:

    MidiPlayEvent() : MEvent()
    {
    }

    MidiPlayEvent(const MEvent& e) : MEvent(e)
    {
    }

    MidiPlayEvent(unsigned tm, int p, int c, int t, int a, int b, Track* trk = 0)
    : MEvent(tm, p, c, t, a, b, trk)
    {
    }

    MidiPlayEvent(unsigned t, int p, int type, const unsigned char* data, int len, Track* trk = 0)
    : MEvent(t, p, type, data, len, trk)
    {
    }

    MidiPlayEvent(unsigned t, int p, int type, EvData data, Track* trk = 0)
    : MEvent(t, p, type, data, trk)
    {
    }

    MidiPlayEvent(unsigned t, int port, int channel, const Event& e, Track* trk = 0)
    : MEvent(t, port, channel, e, trk)
    {
    }

    ~MidiPlayEvent()
    {
    }
};

//---------------------------------------------------------
//   MPEventList
//    memory allocation in audio thread domain
//---------------------------------------------------------

typedef std::multiset<MidiPlayEvent, std::less<MidiPlayEvent>, audioRTalloc<MidiPlayEvent> > MPEL;

struct MPEventList : public MPEL
{

    iterator add(const MidiPlayEvent & ev)
    {
        return MPEL::insert(ev);
    }
};

typedef MPEventList::iterator iMPEvent;
typedef MPEventList::const_iterator ciMPEvent;

//---------------------------------------------------------
//   MidiFifo
//---------------------------------------------------------

class MidiFifo
{
    MidiPlayEvent fifo[MIDI_FIFO_SIZE];
    volatile int size;
    int wIndex;
    int rIndex;

public:

    MidiFifo()
    {
        clear();
    }
    bool put(const MidiPlayEvent& event); // returns true on fifo overflow
    MidiPlayEvent get();
    const MidiPlayEvent& peek(int n = 0);
    void remove();

    bool isEmpty() const
    {
        return size == 0;
    }

    void clear()
    {
        size = 0, wIndex = 0, rIndex = 0;
    }

    int getSize() const
    {
        return size;
    }
};

class MidiRecFifo
{
	MidiPlayEvent fifo[MIDI_REC_FIFO_SIZE];
	volatile int size;
	int wIndex;
	int rIndex;

public:
	MidiRecFifo() { clear(); }
	bool put(const MidiPlayEvent&);
	MidiPlayEvent get();
	const MidiPlayEvent& peek(int n = 0);
	void remove();
	bool isEmpty() const { return size == 0; }
	void clear() { size = 0, wIndex = 0, rIndex = 0; }
	int getSize() const { return size; }
};

#endif

