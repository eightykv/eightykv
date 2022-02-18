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
var delay = 1000;
// Metronome object
var metro;
var progression;
var root;
var bars = 0;
var beats = 0;
var max_beats = 4;

const progressions = [
  [0, 1, 4],
  [0, 3, 1, 4],
  [0, 3, 4],
  [0, 4, 5, 3]
];
// Notes to arpeggiate over
const arp = [0, 2, 4, 6];

function generateChordProgression() {
  // Randomly pick a progression
  progression = progressions[Math.floor(Math.random() * 4)];
  // add root
  for (var i = 0; i < progression.length; i++) {
    progression[i] += root;
  }
  console.log(progression);
}

function startMelody(_root) {
  root = _root;
  generateChordProgression();
  metro = setInterval(nextNote, delay);
}

function nextNote() {
  var randInterval = Math.floor(Math.random() * arp.length);
  var note = progression[bars] + arp[randInterval];
  console.log(note);
  // Convert midi note to freq: fm  =  2(m−69)/12(440 Hz)
  var freq = (Math.pow(2, (note - 69)/12)) * 440;
  console.log(freq);
  beats++;
  if (beats == max_beats) {
    beats = 0;
    bars = (bars + 1) % progression.length;
    console.log(bars + ": " + progression[bars]);
  }
}

startMelody(60);