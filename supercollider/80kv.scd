/*ServerOptions.devices
o = Server.default.options;
o.inDevice_("Focusrite USB ASIO");
o.outDevice_("Focusrite USB ASIO");*/

s.boot;

s.doWhenBooted({
  {
    (0..3).do {
      |i|
      // Need this to let me stagger the timing of initial code execution
      switch (i,
        // First load the buffers and define the busses
        0, {
          ~drums = [];
          {
            (0..4).do {
              |i|
              ~drums = ~drums.add(Buffer.read(s, thisProcess.nowExecutingPath.dirname ++ "/sounds/drum" ++ i ++ ".wav"););
              0.125.wait;
            }
          }.fork();

          ~samples = [];
          {
            (0..9).do {
              |i|
              // Load up to 10 samples
                ~samples = ~samples.add(Buffer.read(s, thisProcess.nowExecutingPath.dirname ++ "/sounds/sample" ++ i ++ ".wav"););
              0.125.wait;
            }
          }.fork();

          ~current_sample = ~samples[0];

          // Drumtree busses
          ~drum_bus = Bus.control(s, 5);
          ~fx_bus = Bus.control(s);
          ~sample_bus = Bus.audio(s);

          // Gloves busses
          ~x_bus = Bus.control(s);
          ~y_bus = Bus.control(s);
          ~z_bus = Bus.control(s);
          ~i_bus = Bus.control(s);
          ~m_bus = Bus.control(s);
          ~r_bus = Bus.control(s);
          ~p_bus = Bus.control(s);
          ~rp_bus = Bus.control(s);
          ~at_bus = Bus.control(s);
          ~atr_bus = Bus.control(s);
          ~natr_bus = Bus.control(s);
          ~touch_onoff_bus = Bus.control(s);
          ~touch_numeric_bus = Bus.control(s);

          ~ix_store_bus = Bus.control(s);
          ~iy_store_bus = Bus.control(s);
          ~iz_store_bus = Bus.control(s);
          ~mx_store_bus = Bus.control(s);
          ~my_store_bus = Bus.control(s);
          ~mz_store_bus = Bus.control(s);
          ~rx_store_bus = Bus.control(s);
          ~ry_store_bus = Bus.control(s);
          ~rz_store_bus = Bus.control(s);
        },

        // Next initialize the SerialPort objects and begin the input routines
        1, {
          // Drumtree: Three output busses: a 5-channel one containing the data from each
          // "limb", and two one-channel ones for the FX and samplers
          ~drumtree = SerialPort.new("COM4", 57600);

          ~dtr = Routine({
            var byte, str, drum_status, fx;
            inf.do{|i|
              if(~drumtree.read == 10, {
                str = "";
                while({byte = ~drumtree.read; byte != 13 }, {
                  str = str ++ byte.asAscii;
                });

                if (str.size >= 9, {
                  // If this is >= 9, it's a standard message
                  drum_status = str.split($ );

                  drum_status.do({arg item, count;
                    drum_status.put(count, item.asInteger);
                  });
                  ~drum_bus.setn(drum_status);
                },
                {
                  // Otherwise, this is either FX or a sample
                  if (str.find("fx") != nil, {
                    fx = str.split($ )[1].asFloat;
                    ~fx_bus.set(fx);
                  },
                  {
                    ("sample" + str.asInteger).postln;
                    ~current_sample = ~samples[str.asInteger];
                    if (~chop_sampler != nil) {
                      ~chop_sampler.set(\buf, ~current_sample.bufnum);
                      {Out.ar(~sample_bus, PlayBufCF.ar(1, ~current_sample))}.play;
                    }
                  });
                });
              });
            };
          }).play;

          // Gloves:
          ~gloves = SerialPort.new("COM9", 57600);

          ~gr = Routine({
            var byte, str, res, x, y, z;
            inf.do{|i|
              if(~gloves.read == 10, {
                str = "";
                while({byte = ~gloves.read; byte != 13 }, {
                  str = str ++ byte.asAscii;
                });

                res = str.split($ );

                res.do({arg item, count;
                  res.put(count, item.asInteger);
                });


                // Set the busses
                if (res.size >= 10) {
                  // Process X. Currently 0-360; modify to scale from 0 to 1, where 180 = 1 and then it goes back down
                  x = res[0];
                  if (x > 180, {
                    x = 180 - (x % 180);
                  });
                  x = x / 180;

                  // Process Y. Set to absolute value and scale from 0 to 1
                  y = abs(res[1]) / 180;

                  // Process Z.
                  z = res[2];
                  if (z > 0, {
                    z = 0;
                  }, {
                    z = MinItem([abs(z), 90]);
                  });
                  z = z / 90;

                  ~x_bus.set(x);
                  ~y_bus.set(y);
                  ~z_bus.set(z);

                  // If AT is on, store XYZ for each finger
                  ~at_bus.get({|at_val|
                    if (at_val == 1, {
                      if (res[3] == 1, {
                        ~ix_store_bus.set(x);
                        ~iy_store_bus.set(y);
                        ~iz_store_bus.set(z);
                      });
                      if (res[4] == 1, {
                        ~mx_store_bus.set(x);
                        ~my_store_bus.set(y);
                        ~mz_store_bus.set(z);
                      });
                      if (res[5] == 1, {
                        ~rx_store_bus.set(x);
                        ~ry_store_bus.set(y);
                        ~rz_store_bus.set(z);
                      });
                    });
                  });

                  // ATR and NATR - Ring with/without AT
                  if (res[5] == 1 && res[7] == 1, {
                    ~atr_bus.set(1);
                    ~natr_bus.set(0);
                  }, {
                    ~atr_bus.set(0);
                    if (res[5] == 1 && res[7] == 0, {
                      ~natr_bus.set(1);
                    }, {
                      ~natr_bus.set(0);
                    });
                  });

                  ~i_bus.set(res[3]);
                  ~m_bus.set(res[4]);
                  ~r_bus.set(res[5]);
                  ~p_bus.set(res[6]);
                  ~rp_bus.set(res[5] & res[6]);
                  ~at_bus.set(res[7]);
                  ~touch_onoff_bus.set(res[8]);
                  ~touch_numeric_bus.set(res[9]);
                }
              });
            };
          }).play;
        },

        // Then create SynthDefs
        2, {
          // Drumtree synth, plays drums with or without distortion
          SynthDef(\drumtree, { |distort = 1|
            var signal, in_trig, bit_depth, amp;
            bit_depth = 1 - (3 + (distort * 13));
            in_trig = InTrig.kr( ~drum_bus, 5 );
            amp = 0.2 + (0.3 * distort);
            for (0, 4, { |i|
              signal = (PlayBufCF.ar(2, ~drums[i], 1, in_trig[i]) * amp).round(2**(bit_depth));
              Out.ar(0, Pan2.ar(signal, 0));
            })
          }).add;

          // Drumtree sampler, plays samples with or without distortion
          SynthDef(\drumtree_sampler, { |distort = 1|
            var signal, in_trig, bit_depth, amp;
            bit_depth = 1 - (3 + (distort * 13));
            amp = 0.1 + (0.4 * distort);
            signal = (In.ar(~sample_bus) * amp).round(2**(bit_depth));
            Out.ar(0, Pan2.ar(signal, 0));
          }).add;

          // Nice windy noise with optional distortion
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

          // Staticky crackle
          SynthDef(\static, { |on, freq_arg, scale_arg, static_arg, noise_arg|
            var env, freq, scale, sig;
            // Scale freq between 200 and 1000
            freq = 200 + (freq_arg * 800);
            // Scale between -1 and 1
            scale = (scale_arg * 2) - 1;
            // Generate envelope from "on"
            env = EnvGen.kr(Env.adsr(0.8, 0, 1, 2.5), on);
            // Signal
            sig = LPF.ar(((Crackle.ar(1.3 + (0.2 * static_arg), 0.75) + (BrownNoise.ar * noise_arg)) * 3).scaleneg(scale).softclip, freq) * env;
            // Output
            Out.ar(0, [sig, sig]);
          }).add;

          // To fuck with current sample
          // on: 0 or 1
          // start_arg, end_arg, retrig_arg: scaled 0.0 through 1.0
          SynthDef(\chop_sampler, { |on = 0, start_arg = 0, end_arg = 0, retrig_arg = 0, buf|
            var env, start, end, retrig, sig;
            // Start position - scrub through current sample
            start = BufFrames.kr(buf) * (start_arg * 0.9);
            // End position - offset from start position
            end = BufFrames.kr(buf);// * end_arg;// (1 - end_arg));
            // Retrigger - Happens semirandomly through Dust
            retrig = retrig_arg * 8.0;
            // Generate envelope from "on"
            env = EnvGen.kr(Env.adsr(0.6, 0, 1, 1), on);
            // Signal
            sig = BufRd.ar(1, buf, Phasor.ar(Dust.ar(retrig), 1 - end_arg, start, end, start), 0) * env;
            // Output
            Out.ar(0, Pan2.ar(sig * 0.5, 0));
          }).add;

          // wavetable
          ~cheb = Buffer.alloc(s, 512, 1, { | buf | buf.chebyMsg([1,0,1,1,0,1])});
          // Chorusing pad sound
          SynthDef(\chorus, { | on = 0, freq_arg = 0, rev1_arg = 0, rev2_arg = 0, mix_arg = 0 |
            // Chorus oscillator
            var sig0 = RLPF.ar(COsc.ar(~cheb.bufnum, 80, freq_arg * 2, 0.25) + COsc.ar(~cheb.bufnum, 100, freq_arg * 30, 0.25), 300, 0.05);
            // Nonlinear filters with reverb
            var sig1 = FreeVerb.ar( NLFiltC.ar(
              sig0,
              LFNoise2.kr(1).range(0.3, 0.5),
              0.2,
              0.7,
              0.4,
              LFCub.kr(0.2, [0, 0.5 * pi], 100, 400)
            ), 0.5, rev1_arg, rev2_arg);
            // Envelope as always (slower ramp up/down
            var env = EnvGen.kr(Env.adsr(0.6, 0, 1, 1), on);
            // Mix between filtered and unfiltered signal
            var sig2 = FreeVerb.ar( (mix_arg * sig1 ) + ((1 - mix_arg) * sig0 ), 0.5, rev1_arg, rev2_arg ) * env;
            // Output
            Out.ar(0, Pan2.ar(sig2 * 0.2));
          }).add;
        },

        // Finally, create and map synths
        3; {
          ~drum_synth = Synth(\drumtree);
          ~drum_synth.map(\distort, ~fx_bus.index);

          ~sampler_synth = Synth(\drumtree_sampler);
          ~sampler_synth.map(\distort, ~fx_bus.index);

          ~windy_synth = Synth(\windy);
          ~windy_synth.map(\on, ~i_bus.index);
          ~windy_synth.map(\distort_arg, ~iz_store_bus.index);
          ~windy_synth.map(\freq_arg, ~y_bus.index);

          ~static_synth = Synth(\static);
          ~static_synth.map(\on, ~m_bus.index);
          ~static_synth.map(\freq_arg, ~x_bus.index);
          ~static_synth.map(\scale_arg, ~z_bus.index);
          ~static_synth.map(\static_arg, ~z_bus.index);
          ~static_synth.map(\noise_arg, ~y_bus.index);

          ~chop_sampler = Synth(\chop_sampler, [\buf, ~current_sample.bufnum]);
          ~chop_sampler.map(\on, ~atr_bus.index);
          ~chop_sampler.map(\start_arg, ~x_bus.index);
          ~chop_sampler.map(\end_arg, ~y_bus.index);
          ~chop_sampler.map(\retrig_arg, ~z_bus.index);

          ~chorus_synth = Synth(\chorus);
          ~chorus_synth.map(\on, ~natr_bus.index);
          ~chorus_synth.map(\freq_arg, ~x_bus.index);
          ~chorus_synth.map(\rev1_arg, ~z_bus.index);
          ~chorus_synth.map(\rev2_arg, ~z_bus.index);
          ~chorus_synth.map(\mix_arg, ~y_bus.index);

        }
      );
      // Wait half a second between each step to give it time to process
      1.wait;
    }
  }.fork();
});

s.quit({
  ~dtr.stop;
  ~drums.do({arg item; item.free;});
  ~drum_bus.free;
  ~fx_bus.free;
  ~sample_bus.free;
  ~drum_synth.free;
  ~sampler_synth.free;
  ~drumtree.close;
  ~gr.stop;
  ~x_bus.free;
  ~y_bus.free;
  ~z_bus.free;
  ~i_bus.free;
  ~m_bus.free;
  ~r_bus.free;
  ~p_bus.free;
  ~rp_bus.free;
  ~at_bus.free;
  ~atr_bus.free;
  ~natr_bus.free;
  ~touch_onoff_bus.free;
  ~touch_numeric_bus.free;
  ~ix_store_bus.free;
  ~iy_store_bus.free;
  ~iz_store_bus.free;
  ~mx_store_bus.free;
  ~my_store_bus.free;
  ~mz_store_bus.free;
  ~rx_store_bus.free;
  ~ry_store_bus.free;
  ~rz_store_bus.free;
  ~windy_synth.free;
  ~static_synth.free;
  ~chop_sampler.free;
  ~chorus_synth.free;
  ~gloves.close;
});
