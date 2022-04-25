#Fully Qualified Board Name, e.g.
#Arduino Mega:      arduino:avr:mega
#Arduino Nano:      arduino:avr:nano
#Arduino Nano with old bootloader:  arduino:avr:nano:cpu=atmega328old
FQBN='arduino:avr:mega'

#Serial port to program the arduino from
PORT='/dev/ttyACM0'

#Allows user to pass extra cpp compiler flags
CPP_FLAGS=''

arduino-cli compile --fqbn $FQBN --build-property "compiler.cpp.extra_flags=$CPP_FLAGS"
sudo -E arduino-cli upload -p $PORT --fqbn $FQBN
