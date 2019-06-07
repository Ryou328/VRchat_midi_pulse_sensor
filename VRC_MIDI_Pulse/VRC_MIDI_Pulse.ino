#include "MIDIUSB.h"
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

const uint8_t OUTPUT_TYPE = SERIAL_PLOTTER;
const uint8_t PULSE_INPUT = A0;
const int PULSE_BLINK = 13;    // Pin 13 is the on-board LED
const int PULSE_FADE = 5;
const uint16_t THRESHOLD = 550;   // Adjust this number to avoid noise when idle

const uint8_t MIDI_CHA = 12;    //midi channel

PulseSensorPlayground pulseSensor;

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}


void setup() {

  Serial.begin(9600);

  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  while (!pulseSensor.begin()) {
    Serial.print(".");
    delay(500);
  }


}
/*
  uint8_t calcDigit(uint16_t num)
  {
  int digit = 0;
  while (num != 0) {
    num = num / 10;
    digit++;
  }
  return digit;
  }
*/

void sent_midi_data_ON(uint8_t ch, uint8_t num) {
  uint8_t num_data[3];
  const uint8_t cnt = 3;

  // cnt = calcDigit(num);

  num_data[0] = num % 10;
  num_data[1] = num / 10 % 10;
  num_data[2] = num / 100 % 10;

  for (uint8_t i = 0; i < cnt; i++) {
    num_data[i] += 21 + (i * 10);
    noteOn(ch, num_data[i], 64);   // Channel , note, normal velocity
    MidiUSB.flush();
    delay(10);
  }
  Serial.print("MIDI: ");
  Serial.print(num_data[2]);
  Serial.print(",");
  Serial.print(num_data[1]);
  Serial.print(",");
  Serial.println(num_data[0]);
}


void sent_midi_data_OFF(uint8_t ch, uint8_t num) {
  uint8_t num_data[3];
  const uint8_t cnt = 3;

  // cnt = calcDigit(num);

  num_data[0] = num % 10;
  num_data[1] = num / 10 % 10;
  num_data[2] = num / 100 % 10;

  for (uint8_t i = 0; i < cnt; i++) {
    num_data[i] += 21 + (i * 10);
    noteOff(ch, num_data[i], 64);   // Channel , note, normal velocity
    MidiUSB.flush();
    delay(10);
  }

}


void loop() {

  static uint16_t BPM , sum;
  const uint8_t cnt = 10;

  // for (uint8_t i = 0 ; i < cnt ; i++)
  // {
  //sum += pulseSensor.getBeatsPerMinute();
  //sum += random(60, 200);//test
  //delay(100);
  // }
  //BPM = sum / cnt;
  BPM = pulseSensor.getBeatsPerMinute();;
  Serial.print("BPM: ");
  Serial.println(BPM);
  sum = 0;

  sent_midi_data_ON(MIDI_CHA, BPM);
  delay(500);
  sent_midi_data_OFF(MIDI_CHA, BPM);
  delay(500);



}
