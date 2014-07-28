Arduino code to read audio input and output to a volume meter.

Audio input should be pre-amplified (e.g. by an LM386)
and connected to an Arduino analog input pin (default A0).

The data for the segments is sent to a digital pin (default 8) serially
using another pin (default 10) as a clock.
You can create a shift register by using a D-Type latch (e.g. 74HC74)
for every segment to drive your LEDs.

The min/max input voltage can be configured, along with some other parameters.
 
