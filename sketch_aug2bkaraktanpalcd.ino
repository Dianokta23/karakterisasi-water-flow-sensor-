#include <Wire.h>

#define SENSOR_PIN D5

volatile int pulseCount = 0;
float flowRate = 0;
float lastFlowRate = 0;
unsigned long lastTime = 0;
unsigned long currentTime = 0;
float volume = 0;
unsigned long lastPulseTime = 0; // Waktu terakhir pulsa terdeteksi
const unsigned long displayTimeout = 20000; // 20 detik

void IRAM_ATTR pulseCounter() {
  pulseCount++;
  lastPulseTime = millis(); // Memperbarui waktu terakhir pulsa terdeteksi
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
  lastTime = millis();
  lastPulseTime = millis(); // Inisialisasi waktu terakhir pulsa terdeteksi
}

void loop() {
  currentTime = millis();
  
  if (currentTime - lastTime >= 1000) { // Setiap 1 detik
    detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
    
    float frequencyHz = pulseCount; // Menghitung frekuensi dalam Hz
    
    if (pulseCount > 0) {
      // Menghitung flow rate dalam liter/menit
      flowRate = (pulseCount / 7.5); // Flow rate dalam L/min
      // Menghitung volume dalam liter
      volume += (flowRate / 60.0); // Volume dalam liter (karena dihitung setiap detik)
      
      // Menyimpan nilai flow rate terakhir yang terdeteksi
      lastFlowRate = flowRate;
    } else if (currentTime - lastPulseTime <= displayTimeout) {
      // Jika tidak ada pulsa yang terdeteksi tapi masih dalam batas waktu timeout
      flowRate = lastFlowRate;
      frequencyHz = lastFlowRate * 7.5; // Menghitung frekuensi berdasarkan flow rate terakhir
    } else {
      // Jika tidak ada pulsa dan sudah melewati batas waktu timeout
      flowRate = 0;
      lastFlowRate = 0; // Reset last flow rate setelah timeout
      frequencyHz = 0;  // Reset frekuensi setelah timeout
    }
    
    // Menampilkan hasil pada Serial Monitor
    Serial.println("============================");
    Serial.println("Water Flow Measurement");
    Serial.println("============================");
    
    Serial.print("Pulse Frequency: ");
    Serial.print(frequencyHz);
    Serial.println(" Hz");
    
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
    
    Serial.print("Volume: ");
    Serial.print(volume);
    Serial.println(" L");
    
    Serial.println("============================\n");
    
    // Reset pulseCount untuk perhitungan berikutnya
    pulseCount = 0;
    // Memperbarui lastTime untuk interval berikutnya
    lastTime = currentTime;
    
    // Mengaktifkan kembali interrupt
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
  }
}
