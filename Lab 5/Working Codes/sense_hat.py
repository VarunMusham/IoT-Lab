from sense_emu import SenseHat

sense = SenseHat()

temp = sense.get_temperature()
humidity = sense.get_humidity()

print(f"Emulated Sense HAT Temperature: {temp:.2f}°C")
print(f"Emulated Sense HAT Humidity: {humidity:.2f}%")
