//ServerOptions.devices
o = Server.default.options;
o.inDevice_("ASIO : Focusrite USB ASIO");
o.outDevice_("ASIO : Focusrite USB ASIO");
~touch = SerialPort.new("COM6", 115200);

s.boot;


~tr = Routine({
	var byte, str, drum_status, fx;
	inf.do{|i|
		if(~touch.read == 10, {
			str = "";
			while({byte = ~touch.read; byte != 13 }, {
				str = str ++ byte.asAscii;
			});
			str.postln;

			if (str == "0 touched", {
				~a_bus.set(1);
				"on".postln;
			}, {
				if (str == "0 released", {
					~a_bus.set(0);
					"off".postln;
				})
			});
		});
	};
}).play;


~a_bus = Bus.control(s);
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

~tr.stop;