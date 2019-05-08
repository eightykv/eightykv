#include "Samples.h"

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, SAMPLESMIDI, midi::DefaultSettings);

Samples::Samples() {
  baseMidiNote = 24;
  midiNote = baseMidiNote;
}

void Samples::sendSample() {
  SAMPLESMIDI.sendNoteOff(midiNote, 127, 8);
  
  midiNote = midiNote + 1;
  SAMPLESMIDI.sendNoteOn(midiNote, 127, 8);
}

void Samples::goBack() {
  midiNote = midiNote - 1;
  // Never let it go lower than the base note
  if (midiNote < baseMidiNote) {
    midiNote = baseMidiNote;
  }
}

void Samples::reset() {
  midiNote = baseMidiNote;
}
