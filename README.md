# air suspension controller


## basic operation
each wheel has a height sensor which provides the following values via resistance measurement:



max ground clearance (high ride height)
upper range
lower range

min ground clearance (low ride height)
upper range
lower range

to change the ride height, solenoid valves have to be opened 

The four height sensors monitor the difference in height between the body of the vehicle and the wheels - one for each wheel. Information is received from each sensor by the electronic control unit (ECU) and is used to maintain, add or release air in each air spring by a series of valves

When lowering the vehicle, the outlet and diaphragm valve, and air spring valves activate, and air is expelled through the air dryer and the exhaust silencer.

When raising the vehicle, the inlet and air spring valves activate, and air is sent into the air springs inflating them


## Arduino Board info

Input and Output

Each of the 54 digital pins on the Arduino 2560 Mega can be used as an input or output, using pinMode(), digitalWrite(), and digitalRead() functions. They operate at 5 volts. Each pin can provide or receive a maximum of 40 mA and has an internal pull-up resistor (disconnected by default) of 20-50 kOhms.

In addition, some pins have specialized functions:

Serial: 0 (RX) and 1 (TX);
Serial 1: 19 (RX) and 18 (TX);
Serial 2: 17 (RX) and 16 (TX);
Serial 3: 15 (RX) and 14 (TX).

Used to receive (RX) and transmit (TX) TTL serial data. Pins 0 and 1 are also connected to the corresponding pins of the ATmega16U2 USB-to-TTL Serial chip.

External Interrupts: 2 (interrupt 0), 3 (interrupt 1), 18 (interrupt 5), 19 (interrupt 4), 20 (interrupt 3), and 21 (interrupt 2). These pins can be configured to trigger an interrupt on a low value, a rising or falling edge, or a change in value. See the attachInterrupt() function for details.

PWM: 2 to 13 and 44 to 46. Provide 8-bit PWM output with the analogWrite() function.
SPI: 50 (MISO), 51 (MOSI), 52 (SCK), 53 (SS). These pins support SPI communication using the SPI library. The SPI pins are also broken out on the ICSP header, which is physically compatible with the Uno, Duemilanove and Diecimila.

LED: 13. There is a built-in LED connected to digital pin 13. When the pin is HIGH value, the LED is on, when the pin is LOW, it’s off.

TWI: 20 (SDA) and 21 (SCL). Support TWI communication using the Wire library. Note that these pins are not in the same location as the TWI pins on the Duemilanove or Diecimila.
The Mega2560 has 16 analog inputs, each of which provide 10 bits of resolution (i.e. 1024 different values). By default they measure from ground to 5 volts, though is it possible to change the upper end of their range using the AREF pin and analogReference() function.

There are a couple of other pins on the board:

AREF. Reference voltage for the analog inputs. Used with analogReference().
Reset. Bring this line LOW to reset the microcontroller. Typically used to add a reset button to shields which block the one on the board.
