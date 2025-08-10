// RootDictLinkDef.h is a part of the PYTHIA event generator.
// Copyright (C) 2025 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details
// ROOT is (c) the ROOT team. See https://root.cern.ch/.

// Authors: Christian Bierlich <christian.bierlich@fysik.lu.se>

// Header used to generate a ROOT 6 dictionary for the custom classes
// RootTrack and RootEvent used by main144.cc, and defined in RootEvent.h.
// The dictionary does not require a Pythia installation, and can therefore
// be generated/used on any machine with ROOT installed.

#ifdef __CLING__
// or use __CINT__ if you're still on ROOT 5.

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class RootTrack+;
#pragma link C++ class RootEvent+;

#endif