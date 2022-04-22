FQBN='arduino:avr:mega'
PORT='/dev/ttyACM0'

arduino-cli compile --fqbn $FQBN
sudo -E arduino-cli upload -p $PORT --fqbn $FQBN
