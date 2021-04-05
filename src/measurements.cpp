#include <measurements.h>

const int Measurements::size;

Measurements::Measurements() { }

int Measurements::getCount() {
  return count;
}

bool Measurements::add(__int64_t timeStamp, float temperature) {
  if (count < size ) {
    count++;
    measurementTimes[getCurrentIndex()] = timeStamp;
    temperatures[getCurrentIndex()] = temperature;

    return true;
  }

  return false;
}

void Measurements::clear() {
  count = 0;
}

void Measurements::log() {
  if (getCurrentIndex() >= 0) {
    Serial.printf("[%lu] Writing measurements at index %i\n", millis(), getCurrentIndex());
    Serial.printf("[%lu] Measurement time  : %lld\n", millis(), measurementTimes[getCurrentIndex()]);
    Serial.printf("[%lu] Temperature       : %.3f°C\n", millis(), temperatures[getCurrentIndex()]);
  }
}

String Measurements::createMqttMessage() {
  if (getCurrentIndex() >= 0) {
    String message = String("[");
    char buffer[100];
    for (int i = 0; i <= getCurrentIndex(); i++) {
      sprintf(buffer, "{\"time\":%lld,\"temperature\":%.3f}", measurementTimes[i], temperatures[i]);
      if (i != 0)
        message += ",";
      message += buffer;
    }
    message += "]";

    return message;
  }

  return "[]";
}

int Measurements::getCurrentIndex() {
  return count - 1;
}