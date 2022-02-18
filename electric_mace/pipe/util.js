const OSC = require("osc");

const oscPort = new OSC.UDPPort({
  localAddress: "0.0.0.0",
  localPort: 57121,
  metadata: true
});
oscPort.open();

/*
 * Send to SuperCollider via OSC
 * In SuperCollider, we just pipe data into buses which are mapped
 * to the parameters of our synths.
 */
function sendOSC(bus, val_type, val) {
  oscPort.send({
      address: "/bus",
      args: [
          {
              type: "i",
              value: bus
          },
          {
              type: val_type,
              value: val
          }
      ]
  }, "127.0.0.1", 6969);
}

module.exports = { sendOSC };