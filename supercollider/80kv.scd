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
          ~at_bus = Bus.control(s);
          ~touch_onoffbus = Bus.control(s);
          ~touch_numericbus = Bus.control(s);
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
                    {Out.ar(~sample_bus, PlayBufCF.ar(1, ~samples[str.asInteger]))}.play;
                  });
                });
              });
            };
          }).play;

          // Gloves:
          ~gloves = SerialPort.new("COM9", 57600);

          ~gr = Routine({
            var byte, str, res;
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
                  ~x_bus.set(res[0]);
                  ~y_bus.set(res[1]);
                  ~z_bus.set(res[2]);
                  ~i_bus.set(res[3]);
                  ~m_bus.set(res[4]);
                  ~r_bus.set(res[5]);
                  ~p_bus.set(res[6]);
                  ~at_bus.set(res[7]);
                  ~touch_onoffbus.set(res[8]);
                  ~touch_numericbus.set(res[9]);
                }
              });
            };
          }).play;
        },

        // Then create SynthDefs
        2, {
          SynthDef(\drumtree, { |distort = 0.5|
            var signal, in_trig, bit_depth, amp;
            bit_depth = 1 - (3 + (distort * 13));
            in_trig = InTrig.kr( ~drum_bus, 5 );
            amp = 0.1 + (0.4 * distort);
            for (0, 4, { |i|
              signal = (PlayBufCF.ar(2, ~drums[i], 1, in_trig[i]) * amp).round(2**(bit_depth));
              Out.ar(0, Pan2.ar(signal, 0));
            })
          }).add;

          SynthDef(\drumtree_sampler, { |distort = 0.5|

            var signal, in_trig, bit_depth, amp;
            bit_depth = 1 - (3 + (distort * 13));
            amp = 0.1 + (0.4 * distort);
            signal = (In.ar(~sample_bus) * amp).round(2**(bit_depth));
            //signal = In.ar(~sample_bus);
            Out.ar(0, Pan2.ar(signal, 0));
          }).add;
        },

        // Finally, create and map synths
        3; {
          ~drum_synth = Synth(\drumtree);
          ~drum_synth.map(\distort, ~fx_bus.index);

          ~sampler_synth = Synth(\drumtree_sampler);
          ~sampler_synth.map(\distort, ~fx_bus.index);
        }
      );
      // Wait half a second between each step to give it time to process
      0.5.wait;
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
  ~at_bus.free;
  ~touch_onoffbus.free;
  ~touch_numericbus.free;
  ~gloves.close;
});
