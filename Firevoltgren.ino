#include <BluetoothSerial.h>
#include <ESP32Servo.h>

// Main Motor Driver Pins (for movement)
#define MOTOR_LEFT_FORWARD 12
#define MOTOR_LEFT_BACKWARD 13
#define MOTOR_RIGHT_FORWARD 14
#define MOTOR_RIGHT_BACKWARD 15

// Additional Motor Driver Pins (for DC motors replacing servos 1 & 2)
#define DC_MOTOR1_FORWARD 16    // New pin for DC motor 1 forward
#define DC_MOTOR1_BACKWARD 17   // New pin for DC motor 1 backward
#define DC_MOTOR2_FORWARD 18    // New pin for DC motor 2 forward
#define DC_MOTOR2_BACKWARD 19   // New pin for DC motor 2 backward

// Slow motor speed (0-255)
#define SLOW_MOTOR_SPEED 500    // Very slow speed (about 16% of full speed)

// Servo Pin (only servo3 remains)
#define SERVO3_PIN 27

// Relay Pins
#define RELAY1_PIN 32
#define RELAY2_PIN 33
#define RELAY3_PIN 34

// Bluetooth Serial Object
BluetoothSerial SerialBT;

// Only one servo object now
Servo servo3;

void setup() {
  // Main Motor Driver pin setup
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  
  // Additional Motor Driver pin setup
  pinMode(DC_MOTOR1_FORWARD, OUTPUT);
  pinMode(DC_MOTOR1_BACKWARD, OUTPUT);
  pinMode(DC_MOTOR2_FORWARD, OUTPUT);
  pinMode(DC_MOTOR2_BACKWARD, OUTPUT);
  
  // Initialize all motor pins to LOW
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  digitalWrite(DC_MOTOR1_FORWARD, LOW);
  digitalWrite(DC_MOTOR1_BACKWARD, LOW);
  digitalWrite(DC_MOTOR2_FORWARD, LOW);
  digitalWrite(DC_MOTOR2_BACKWARD, LOW);
  
  // Relay pin setup
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  
  // Initialize relays to OFF state
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  
  // Allocate PWM timer for the remaining servo
  ESP32PWM::allocateTimer(0);
  
  // Attach servo to pin
  servo3.attach(SERVO3_PIN);
  
  // Initialize servo to default position
  servo3.write(0);    // Start at 0 degrees
  
  // Bluetooth setup
  SerialBT.begin("RobotController"); // Name of Bluetooth device
  Serial.begin(115200); // Serial monitor for debugging
  Serial.println("Robot Controller Ready");
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();
    
    switch(command) {
      // Movement Commands
      case 'F': moveForward(); break;    // Forward
      case 'B': moveBackward(); break;   // Backward
      case 'L': turnLeft(); break;       // Left
      case 'R': turnRight(); break;      // Right
      case 'S': stopMoving(); break;     // Stop
      
      // DC motor rotation commands
      case 'X': rotateDCMotorsOpposite(); break;        // Rotate DC motors in opposite directions slowly
      case 'Y': rotateDCMotorsOppositeReversed(); break; // Rotate DC motors in opposite directions (reversed) slowly
      
      // New maximum speed DC motor commands
      case 'M': rotateDCMotorsOppositeMax(); break;     // Rotate DC motors in opposite directions at max speed
      case 'N': rotateDCMotorsOppositeReversedMax(); break; // Rotate DC motors in opposite directions (reversed) at max speed
      
      // Relay Control Commands
      case 'A': relay1On(); break;         // Turn Relay 1 ON
      case 'a': relay1Off(); break;        // Turn Relay 1 OFF
      case 'C': relay2On(); break;         // Turn Relay 2 ON
      case 'c': relay2Off(); break;        // Turn Relay 2 OFF
      case 'E': relay3On(); break;         // Turn Relay 3 ON
      case 'e': relay3Off(); break;        // Turn Relay 3 OFF
    }
  }
}

// Rotate DC motors in opposite directions at maximum speed for 5 seconds
void rotateDCMotorsOppositeMax() {
  Serial.println("Rotating DC motors in opposite directions at MAX SPEED for 5 seconds");
  
  // DC Motor 1 forward, DC Motor 2 backward at full speed
  digitalWrite(DC_MOTOR1_FORWARD, HIGH);
  digitalWrite(DC_MOTOR1_BACKWARD, LOW);
  digitalWrite(DC_MOTOR2_FORWARD, LOW);
  digitalWrite(DC_MOTOR2_BACKWARD, HIGH);
  
  // Run for 5 seconds
  delay(5000);
  
  // Stop DC motors
  stopDCMotors();
}

// Rotate DC motors in opposite directions (reversed) at maximum speed for 5 seconds
void rotateDCMotorsOppositeReversedMax() {
  Serial.println("Rotating DC motors in opposite directions (reversed) at MAX SPEED for 5 seconds");
  
  // DC Motor 1 backward, DC Motor 2 forward at full speed
  digitalWrite(DC_MOTOR1_FORWARD, LOW);
  digitalWrite(DC_MOTOR1_BACKWARD, HIGH);
  digitalWrite(DC_MOTOR2_FORWARD, HIGH);
  digitalWrite(DC_MOTOR2_BACKWARD, LOW);
  
  // Run for 5 seconds
  delay(5000);
  
  // Stop DC motors
  stopDCMotors();
}

// Method 1: Use a time-based approach to simulate slow motion
void rotateDCMotorsOpposite() {
  Serial.println("Rotating DC motors in opposite directions SLOWLY for 5 seconds");
  
  // Run in short bursts with pauses between them to achieve slow motion
  long startTime = millis();
  while (millis() - startTime < 5000) {  // Run for 5 seconds total
    // DC Motor 1 forward, DC Motor 2 backward
    digitalWrite(DC_MOTOR1_FORWARD, HIGH);
    digitalWrite(DC_MOTOR1_BACKWARD, LOW);
    digitalWrite(DC_MOTOR2_FORWARD, LOW);
    digitalWrite(DC_MOTOR2_BACKWARD, HIGH);
    
    delay(5);  // Run for 5ms
    
    // Pause motors
    digitalWrite(DC_MOTOR1_FORWARD, LOW);
    digitalWrite(DC_MOTOR1_BACKWARD, LOW);
    digitalWrite(DC_MOTOR2_FORWARD, LOW);
    digitalWrite(DC_MOTOR2_BACKWARD, LOW);
    
    delay(20);  // Pause for 20ms (adjust this for different speeds)
  }
  
  // Stop DC motors
  stopDCMotors();
}

// Rotate DC motors in opposite directions (reversed) slowly for 5 seconds
void rotateDCMotorsOppositeReversed() {
  Serial.println("Rotating DC motors in opposite directions (reversed) SLOWLY for 5 seconds");
  
  // Run in short bursts with pauses between them to achieve slow motion
  long startTime = millis();
  while (millis() - startTime < 5000) {  // Run for 5 seconds total
    // DC Motor 1 backward, DC Motor 2 forward
    digitalWrite(DC_MOTOR1_FORWARD, LOW);
    digitalWrite(DC_MOTOR1_BACKWARD, HIGH);
    digitalWrite(DC_MOTOR2_FORWARD, HIGH);
    digitalWrite(DC_MOTOR2_BACKWARD, LOW);
    
    delay(5);  // Run for 5ms
    
    // Pause motors
    digitalWrite(DC_MOTOR1_FORWARD, LOW);
    digitalWrite(DC_MOTOR1_BACKWARD, LOW);
    digitalWrite(DC_MOTOR2_FORWARD, LOW);
    digitalWrite(DC_MOTOR2_BACKWARD, LOW);
    
    delay(20);  // Pause for 20ms (adjust this for different speeds)
  }
  
  // Stop DC motors
  stopDCMotors();
}

// Stop DC motors
void stopDCMotors() {
  digitalWrite(DC_MOTOR1_FORWARD, LOW);
  digitalWrite(DC_MOTOR1_BACKWARD, LOW);
  digitalWrite(DC_MOTOR2_FORWARD, LOW);
  digitalWrite(DC_MOTOR2_BACKWARD, LOW);
  Serial.println("DC motors stopped");
}

// Main Motor Movement Functions
void moveForward() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("FORWARD");
}

void moveBackward() {
  digitalWrite(MOTOR_LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACKWARD, HIGH);
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  Serial.println("BACKWARD");
}

void turnLeft() {
  digitalWrite(MOTOR_LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("TURN LEFT");
}

void turnRight() {
  digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_RIGHT_BACKWARD, HIGH);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  Serial.println("TURN RIGHT");
}

void stopMoving() {
  digitalWrite(MOTOR_LEFT_FORWARD, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
  Serial.println("STOP");
}
  
// Relay Control Functions
void relay1On() {
  digitalWrite(RELAY1_PIN, HIGH);
  Serial.println("RELAY 1 ON");
}

void relay1Off() {
  digitalWrite(RELAY1_PIN, LOW);
  Serial.println("RELAY 1 OFF");
}

void relay2On() {
  digitalWrite(RELAY2_PIN, HIGH);
  Serial.println("RELAY 2 ON");
}

void relay2Off() {
  digitalWrite(RELAY2_PIN, LOW);
  Serial.println("RELAY 2 OFF");
}

void relay3On() {
  digitalWrite(RELAY3_PIN, HIGH);
  Serial.println("RELAY 3 ON");
}

void relay3Off() {
  digitalWrite(RELAY3_PIN, LOW);
  Serial.println("RELAY 3 OFF");
}
