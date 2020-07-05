~drumtree = SerialPort.new("COM4", 57600);
~gloves = SerialPort.new("COM9", 57600);

(
~dtr = Routine({
    var byte, str, res;
    inf.do{|i|
        if(~drumtree.read == 10, {
            str = "";
            while({byte = ~drumtree.read; byte != 13 }, {
                str = str ++ byte.asAscii;
            });
            (str).postln;
        });
    };
}).play;
)

(
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

			res.at(0).isInteger.post;
        });
    };
}).play;
)

~dtr.stop;
~gr.stop;