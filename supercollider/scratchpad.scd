o = Server.default.options;
o.inDevice_("ASIO : Focusrite USB ASIO");
o.outDevice_("ASIO : Focusrite USB ASIO");

s.boot;

SynthDef(\test, { |freq_arg|
	var env, freq, sig0, sig1;
	freq = 100 + (freq_arg * 1200);
	// Generate envelope from "on"
	env = EnvGen.kr(Env.adsr(0.1, 0.5, 0, 0), doneAction: Done.freeSelf);
	// Signal
	sig0 = LPF.ar(BrownNoise.ar(), freq) * env;
	sig1 = Limiter.ar(Decimator.ar(sig0, bits: MouseX.kr(2, 8)), 0.2);
	// Output
	Out.ar(0, Pan2.ar(sig1 * 0.1));
}).add;

(fork {
	16.do {
		t = Synth(\test, [\freq_arg, 0.2]);
		0.5.wait;
		t.free;
	}
})

t = Synth(\test);


t.free;