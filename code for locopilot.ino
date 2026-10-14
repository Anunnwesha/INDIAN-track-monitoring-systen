// ==== PIN DEFINITIONS ====
const int ENA = 9;       // Left Motor PWM
const int IN1 = A2;
const int IN2 = A3;
const int IN3 = A4;
const int IN4 = A5;
const int ENB = 10;      // Right Motor PWM

const int LEFT_IR = 12;   // Left IR Sensor
const int RIGHT_IR = 11;  // Right IR Sensor

const int TRIG = 2;       // Ultrasonic Trigger
const int ECHO = 3;       // Ultrasonic Echo

const int LED = 13;       // LED Indicator

// ==== VARIABLES ====
long duration;
int distance;

// ==== SETUP ====
void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED, OUTPUT);

  Serial.begin(9600);
}

// ==== ULTRASONIC FUNCTION ====
int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2; // cm
  return distance;
}

// ==== MOTOR CONTROL FUNCTIONS ====
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ==== MAIN LOOP ====
void loop() {
  int leftIR = digitalRead(LEFT_IR);
  int rightIR = digitalRead(RIGHT_IR);
  int dist = getDistance();

  Serial.print("Distance: ");
  Serial.println(dist);

  // ==== OBSTACLE DETECTION ====
  if (dist < 20) {   // Obstacle within 20cm
    stopCar();
    digitalWrite(LED, HIGH);   // LED ON when stopped
    delay(500);

    // Move Backward for ~10cm
    backward();
    delay(500);  // 1 sec = ~10cm backward (speed dependent)
    stopCar();
    delay(500);

    return;  // Skip the rest of the loop
  }

  // ==== LINE FOLLOWING ====
  // IR LOW = BLACK (line) | IR HIGH = WHITE (gap/crack)
  if (leftIR == HIGH && rightIR == HIGH) {
    forward();           // Black detected → Move Forward
    digitalWrite(LED, LOW);
  } else {
    stopCar();           // White detected on any sensor → Stop Car
    digitalWrite(LED, HIGH);
  }

  delay(50);  // Small delay for stable reading
}
