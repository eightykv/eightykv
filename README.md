# 80KV
This is a repository of my code for my custom technology. It includes both the Arduino code that reads and processes sensor data and the Pure Data patches that translate it into sound, as well as many of the samples I use. It doesn't include any hardware schematics, but the libraries usually indicate which chips are being used.

## Gloves
These were the first pieces I ever built, later combined into one. A BNO055 and a set of flex sensors translate the motion of my hands into data. They communicate wirelessly with a Raspberry Pi running Pure Data, which translates that data into sound.

## Drumtree
The drumtree is a parameterized random rhythm generator. It creates beats according to set of inputs corresponding to drum voices and probabilities, as well as start, stop, tap tempo, and an extremely basic sampler.

## Quadrubrachea
This project builds on the existing gloves, using their data to control a set of servo motors that will form four robotic arms. This is a work in progress.

## Supercollider
This project contains the WIP code changing from Pure Data to SuperCollider. This is the one under the most active development.

## Other stuff?
There may be other random or discontinued projects in the repo, but those three are the ones being maintained.
