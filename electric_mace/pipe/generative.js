const util = require("./util");

// Intervals of the minor scale
const minor_scale = [0, 2, 3, 5, 7, 8, 10];
/* Common minor jazz chord progressions:
 * 1 2 5
 * 1 4 2 5
 * 
 * Common substitutions:
 * replace 1 with 3
 * 
 * Common blues progressions:
 * 1 4 5
 * 1 5 6 4
 */

// BPM in ms
var bpm = 100;
// Metronome object
var metro;
var metro1;
// Stored chord progression
var progression;
// Root note e.g. 60 for C4
var root;
// Which beat/bar we're in
var bars = 0;
var beats = 0;
// How many beats per bar (4 for whole notes, 8 for half notes, etc)
var max_beats = 8;

const progressions = [
  [0, 1, 4, 0],
  [0, 3, 1, 4],
  [0, 3, 4, 0],
  [0, 4, 5, 3]
];
// Notes to arpeggiate over
const arp = [0, 2, 4, 6];

function generateChordProgression() {
  // Randomly pick a progression
  var which = Math.floor(Math.random() * 4);
  progression = [];
  // add root
  for (var i = 0; i < progressions[which].length; i++) {
    progression.push(progressions[which][i] + root);
  }
  console.log(progression);
}

function startMelody(_root) {
  root = _root;
  generateChordProgression();
  // TEMP
  util.sendOSC(0, "i", 1);
  metro = setInterval(() => { nextNote(0) }, bpmtoms(bpm));
  setTimeout(() => {
    metro1 = setInterval(() => { nextNote(1) }, bpmtoms(bpm));
  }, bpmtoms(bpm) * 0.6);
}

function nextNote(flag) {
  var note = progression[bars] - minor_scale[arp[(beats + 2) % arp.length]];
  
  console.log(note);
  // Convert midi note to frequency. From here: https://newt.phys.unsw.edu.au/jw/notes.html
  var freq = (Math.pow(2, (note - 69)/12)) * 440;
  util.sendOSC(1 + flag, "f", freq);
  beats++;
  if (beats == max_beats) {
    beats = 0;
    bars = (bars + 1) % progression.length;
    if (bars == 0 && Math.random() < 0.25) {
      if (Math.random() > 0.5) {
        root += Math.floor(Math.random() * 11);
      }
      else {
        root += Math.floor(Math.random() * 11);
      }
      generateChordProgression();
    }
    console.log(bars + ": " + progression[bars]);
  }
}

function bpmtoms(bpm) {
  return 60000 / bpm;
}

// Examples
module.exports = { startMelody };