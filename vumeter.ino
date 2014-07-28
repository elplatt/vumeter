// Volume Meter code for Arduino
// Copyright 2014 Edward L. Platt <ed@elplatt.com>
// License: BSD 3-Clause

long samplePeriod = 100;
long peakFallPeriod = 40000;
long peakLeft = 0;
long fallSpeed = 0;
float fallAcc = 0.4;
long lastPeakMs = 0;
int micPin = 0;
int ledPin = 13;
int clockPin = 10;
int dataPin = 8;
signed int peak = 0;
float maxLevel = 0.1;
float minLevel = 0.04;

void setup () {
  pinMode(ledPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(clockPin, LOW);
  delay(1000);
  Serial.begin(9600);
  Serial.println("Setup");
  lastPeakMs = millis();
}

float sampleAudio () {
  long start = millis();
  long sample;
  long minSample = 1024;
  long maxSample = 0;
  while (millis() - start < samplePeriod) {
    sample = analogRead(micPin);
    if (sample < minSample) {
      minSample = sample;
    }
    if (sample > maxSample) {
      maxSample = sample;
    }
  }
  return (maxSample - minSample) * 5.0/1024.0;
}

void displayLevel(float level) {
  Serial.println(level, DEC);
  int i;
  for (i = 0; i < 8; i++) {
    digitalWrite(clockPin, HIGH);
    digitalWrite(ledPin, LOW);
  }
  // Calculate number of segments to display
  float Delta = maxLevel - minLevel;
  float delta = level - minLevel;
  int on;
  if (delta < 0) {
    on = 0;
  } else {
    on = min(8, int(9.0 * delta / Delta));
    //on = int(9.0 * log(delta) / log(Delta));
  }
  // Update the peak if necessary
  if (on > peak) {
    Serial.println("Setting peak");
    peak = on;
    Serial.println(peak, DEC);
    peakLeft = peakFallPeriod;
    fallSpeed = 0;
  }
  // Write segments top to bottom
  int off = 8 - on;
  for (i = 0; i < off; i++) {
    digitalWrite(ledPin, LOW);
    if (i == 8 - peak) {
      digitalWrite(dataPin, HIGH);
      Serial.println("Peak high");
      Serial.println(peak, DEC);
      Serial.println(8 - peak, DEC);
      Serial.println(i, DEC);
    } else {
      digitalWrite(dataPin, LOW);
    }
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
  }
  for (i = 0; i < on; i ++) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(dataPin, HIGH);
    Serial.println("Level high");
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
  }
}


void updatePeak () {
  long ms = millis();
  // Drop the peak if enough time has passed
  if (fallSpeed * (ms - lastPeakMs) > peakLeft) {
    peakLeft = peakFallPeriod - fallSpeed * (ms - lastPeakMs);
    peak = max(-1, peak - 1);
  } else {
    peakLeft = peakLeft - fallSpeed * (ms - lastPeakMs);
  }
  fallSpeed += fallAcc * (ms - lastPeakMs);
  lastPeakMs = ms;
}

void loop () {
  float level = sampleAudio();
  updatePeak();
  displayLevel(level);
}

