const { SerialPort, ReadlineParser } = require("serialport");
const generative = require("./generative");

const port = new SerialPort({ path: "COM6", baudRate: 115200 });
const parser = new ReadlineParser();
const uart_in = port.pipe(parser);

// Read data from Arduino
function process_uart(data) {
    // Process the data
    // Where is it from? What does it contain?
    // Post-processing -- 
    console.log(data);
}
uart_in.on("data", process_uart);

generative.startMelody(45);
