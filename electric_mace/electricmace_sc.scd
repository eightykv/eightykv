thisProcess.openUDPPort(6969);
s.boot;

// Make a bunch of buses
~a_bus = Bus.control(s);
~b_bus = Bus.control(s);
// Turn all buses into an array
~test = [~a_bus, ~b_bus];
(
f = { |msg, time, addr|
	// Message -- first arg is which bus, second arg is which value
	if(msg[0] == '/bus', {
		~test[msg[1]].set(msg[2]);
	});
};
thisProcess.addOSCRecvFunc(f);
);


SynthDef(\windy, { |on, freq_arg = 0, distort_arg = 0|
	var env, sig;
	// Generate envelope from "on"
	env = EnvGen.kr(Env.adsr(2, 0, 1, 2), on);
	// 5 voices, each higher
	sig = Mix.fill(5, {
		|i|
		var vol = 0.2 - (0.02 * i);
		var freq = (40 * i) + (100 * i * freq_arg);
		var sig1 = RLPF.ar(BrownNoise.ar, freq, 0.2, vol);
		var sig2 = (sig1 * (1 - distort_arg)) + (CrossoverDistortion.ar(sig1, 0.8, 0.25) * 0.1 * distort_arg).softclip;
		sig2 * env;
	});
	// Output
	Out.ar(0, Pan2.ar(sig, 0));
}).add;

~windy_synth = Synth(\windy);
~windy_synth.map(\on, ~a_bus.index);
~windy_synth.map(\freq_arg, ~b_bus.index);

// debug stuff
// Turn OSC trace on/off
//OSCFunc.trace(false);
// Remove function f as OSC receive function
//thisProcess.removeOSCRecvFunc(f);
