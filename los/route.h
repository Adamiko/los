//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: route.h,v 1.5.2.1 2008/05/21 00:28:52 terminator356 Exp $
//
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
#include <map>
#include <Qt>
#include <QList>

#include "globaldefs.h"

class QString;
class Track;
class MidiDevice;
class Xml;

//---------------------------------------------------------
//   Route
//---------------------------------------------------------

struct Route
{

    enum
    {
        TRACK_ROUTE = 0, JACK_ROUTE = 1, MIDI_DEVICE_ROUTE = 2, MIDI_PORT_ROUTE = 3
    }; // p3.3.49

    union
    {
        Track* track;
        MidiDevice* device;
        void* jackPort;
    };

    int midiPort; // p3.3.49 Midi port number. Best not to put this in the union to avoid problems?
	qint64 midiPortId;
	qint64 trackId;

    // Starting source channel (of the owner of this route). Normally zero for mono or stereo tracks, higher for multi-channel tracks.
    // NOTICE: channel is now a bit-wise channel mask, for MidiPort <-> MidiTrack routes.
    // This saves many routes: Instead of one route per channel as before, there can now be only one single route with a channel mask,
    //  for each MidiPort <-> MidiTrack combination.
    int channel;
    // Number of (audio) channels being routed.
    int channels;

    // Allow for multi-channel syntis to feed to/from regular tracks, and to feed one to another.
    // If a synti is feeding to/from a regular track, remoteChannel is the 'starting' channel of this multi-channel synti.
    // If a synti is feeding to/from another synti, this is not used and individual channels are routed using channel instead.
    int remoteChannel;

    unsigned char type; // 0 - track, 1 - jackPort, 2 - midi device, 3 - midi port

    Route(void* t, int ch = -1);
    Route(Track* t, int ch = -1, int chans = -1);
	Route(qint64 tid, int ch = -1, int chans = -1);
    Route(MidiDevice* d, int ch);
    Route(int port, int ch);
    Route(qint64 port, int ch);
    Route(const QString&, bool dst, int ch, int rtype = -1);
    Route();

    QString name() const;
    bool operator==(const Route & a) const;

    bool isValid() const
    {
        return ((type == TRACK_ROUTE) && (trackId > 0)) || ((type == JACK_ROUTE) && (jackPort != 0)) ||
                ((type == MIDI_DEVICE_ROUTE) && (device != 0)) ||
                ((type == MIDI_PORT_ROUTE) && (midiPortId > 0)/* && (midiPort < MIDI_PORTS)*/); // p3.3.49
    }
    void read(Xml & xml);
    void dump() const;
};


//---------------------------------------------------------
//   RouteList
//---------------------------------------------------------

//struct RouteList : public std::vector<Route>
struct RouteList : public QList<Route>
{
    void removeRoute(const Route & r);
};

typedef RouteList::iterator iRoute;
typedef RouteList::const_iterator ciRoute;

extern void addRoute(Route, Route);
extern void removeRoute(Route, Route);
extern void removeAllRoutes(Route, Route); // p3.3.55
extern Route name2route(const QString&, bool dst, int rtype = -1);
extern bool checkRoute(const QString&, const QString&);

//---------------------------------------------------------
//   RouteMenuMap
//---------------------------------------------------------

typedef std::map<int, Route, std::less<int> >::iterator iRouteMenuMap;
typedef std::map<int, Route, std::less<int> >::const_iterator ciRouteMenuMap;
typedef std::map<int, Route, std::less<int> > RouteMenuMap;
typedef std::pair<int, Route> pRouteMenuMap;
typedef std::pair<iRouteMenuMap, bool > rpRouteMenuMap;

#endif

