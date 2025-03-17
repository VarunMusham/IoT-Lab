#!/usr/bin/env python3 
from sense_emu import SenseHat 
sense = SenseHat() 
temp = sense.get_temperature() 
humidity = sense.get_humidity() 
print(f"{temp:.2f},{humidity:.2f}")  # Outputs "temperature,humidity"