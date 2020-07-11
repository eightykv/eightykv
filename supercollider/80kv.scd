s.boot;

s.doWhenBooted({
  {
    [0, 1, 2, 3].do {
      |i|
      // Need this to let me stagger the timing of initial code execution
      switch (i,
        0, {
          // First load the buffers and define the busses
          ~drums = [];
          {
            [0, 1, 2, 3, 4].do {
              |i|
              ~drums = ~drums.add(Buffer.read(s, thisProcess.nowExecutingPath.dirname ++ "/sounds/drum" ++ i ++ ".wav"););
              0.125.wait;
            }
          }.fork();

          ~drum_bus = Bus.control(s, 5);
          ~fx_bus = Bus.control(s);
          ~sample_bus = Bus.control(s);
        },
        1, {
          // Next initialize the SerialPort objects and begin the input routines
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
                    ~sample_bus.set(str.asInteger);
                  });
                });
              });
            };
          }).play;

        },
        2, {
          // Then create SynthDefs
          SynthDef(\drumtree, {arg distort = 0;
            var signal, in_trig, bit_depth, amp;
            bit_depth = 1 - (3 + (distort * 13));
            in_trig = InTrig.kr( ~drum_bus, 5 );
            amp = 0.1 + (0.4 * distort);
            for (0, 4, {arg i;
              signal = (PlayBufCF.ar(2, ~drums[i], 1, in_trig[i]) * amp).round(2**(bit_depth));
              Out.ar(0, Pan2.ar(signal, 0));
            })
          }).add;
        },
        3; {
          // Finally, create and map synths
          ~drum_synth = Synth(\drumtree);
          ~drum_synth.map(\distort, ~fx_bus.index);
        }
      );
      // Wait half a second between each step
      0.5.wait;
    }
  }.fork();
});

/*
 * GLOVES
 *
 *
 */
(
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
    });
  };
}).play;
)


s.quit({
  ~dtr.stop;
  ~gr.stop;
  ~drums.do({arg item; item.free;});
  ~drum_bus.free;
  ~fx_bus.free;
  ~sample_bus.free;
  //~gloves.close;
  ~drumtree.close;
  ~drum_synth.free;
});
