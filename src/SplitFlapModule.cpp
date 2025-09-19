#include "SplitFlapModule.h"

// Array of characters, in order, the first item is located on the magnet on the
// character drum
const char SplitFlapModule::StandardChars[37] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                                                 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
                                                 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

const char SplitFlapModule::ExtendedChars[48] = {
    ' ', 'A', 'B', 'C', 'D', 'E',  'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U',  'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', '\'', ':', '?', '!', '.', '-', '/', '$', '@', '#', '%',
};

bool hasErrored = false;

// Default Constructor
SplitFlapModule::SplitFlapModule()
    : address(0), position(0), stepNumber(0), stepsPerRot(0), chars(StandardChars), numChars(37), charSetSize(37) {
    magnetPosition = 710;
}

// Constructor implementation
SplitFlapModule::SplitFlapModule(
    uint8_t I2Caddress, int stepsPerFullRotation, int stepOffset, int magnetPos, int charsetSize
)
    : address(I2Caddress), position(0), stepNumber(0), stepsPerRot(stepsPerFullRotation), charSetSize(charsetSize) {
    magnetPosition = magnetPos + stepOffset;

    chars = (charsetSize == 48) ? ExtendedChars : StandardChars;
    numChars = (charsetSize == 48) ? 48 : 37;
}

void SplitFlapModule::writeIO(uint16_t data) {
    Wire.beginTransmission(address);
    Wire.write(data & 0xFF);        // Send lower byte
    Wire.write((data >> 8) & 0xFF); // Send upper byte

    byte error = Wire.endTransmission();

    if (error > 0 && ! hasErrored) {
        hasErrored = true; // Set the error flag
        Serial.print("Error writing data to module ");
        Serial.print(address);
        Serial.print(", error code: ");
        Serial.println(error); // Error codes:
        // 0 = success
        // 1 = data too long to fit in transmit buffer
        // 2 = received NACK on transmit of address
        // 3 = received NACK on transmit of data
        // 4 = other error
    }
}

// Init Module, Setup IO Board
void SplitFlapModule::init() {
    float stepSize = (float) stepsPerRot / (float) numChars;
    float currentPosition = 0;
    for (int i = 0; i < numChars; i++) {
        charPositions[i] = (int) currentPosition;
        currentPosition += stepSize;
    }

    uint16_t initState = 0b1111111111100001; // Pin 15 (17) as INPUT, Pins 1-4 as OUTPUT
    writeIO(initState);

    stop();                                  // Write all motor coil inputs LOW

    // Give the stepper a few initial steps with delays between them.
    // Purpose:
    //  - Energise each coil in sequence once (4 steps = full driver cycle).
    //  - Ensures the driver’s stepping pattern and the motor’s rotor are aligned.
    //  - Provides a clean "phase sync" at startup so later movements are reliable.
    // After this priming sequence, stop() is called again to de-energise the coils.
    int initDelay = 100;

    delay(initDelay);
    step();
    delay(initDelay);
    step();
    delay(initDelay);
    step();
    delay(initDelay);
    step();
    delay(initDelay);
    stop();
}

int SplitFlapModule::getCharPosition(char inputChar) {
    inputChar = toupper(inputChar);
    for (int i = 0; i < charSetSize; i++) {
        if (chars[i] == inputChar) {
            return charPositions[i];
        }
    }
    return 0; // Character not found, return blank
}

int SplitFlapModule::getPrevCharPosition(char inputChar) {
    inputChar = toupper(inputChar);
    int targetIndex = -1;
    for (int i = 0; i < charSetSize; i++) {
        if (chars[i] == inputChar) {
            targetIndex = i;
            break;
        }
    }
    if (targetIndex == -1) {
        // fall back to space's previous if not found
        targetIndex = 0;
    }
    int prevIndex = (targetIndex - 1 + numChars) % numChars;
    return charPositions[prevIndex];
}

void SplitFlapModule::stop() {
    uint16_t stepState = 0b1111111111100001;
    writeIO(stepState);
}

void SplitFlapModule::start() {
    stepNumber = (stepNumber + 3) % 4; // effectively take one off stepNumber
    step(false);                       // write the "previous" step high again, in case turned off
}

void SplitFlapModule::step(bool updatePosition) {
    uint16_t stepState;
    switch (stepNumber) {
        case 0:
            stepState = 0b1111111111100111;
            writeIO(stepState);
            break;
        case 1:
            stepState = 0b1111111111110011;
            writeIO(stepState);
            break;
        case 2:
            stepState = 0b1111111111111001;
            writeIO(stepState);
            break;
        case 3:
            stepState = 0b1111111111101101;
            writeIO(stepState);
            break;
    }
    if (updatePosition) {
        position = (position + 1) % stepsPerRot;
        stepNumber = (stepNumber + 1) % 4;
    }
}

bool SplitFlapModule::readHallEffectSensor() {
    if (hasErrored) {
        return false;
    }

    uint8_t requestBytes = 2;
    Wire.requestFrom(address, requestBytes);
    // Make sure the data is available
    if (Wire.available() == 2) {
        uint16_t inputState = 0;

        // Read the two bytes and combine them into a 16-bit value
        inputState = Wire.read();         // Read the lower byte
        inputState |= (Wire.read() << 8); // Read the upper byte and shift it left

        // Many PCF8575 inputs are active-low with pull-ups; treat LOW as detection
        return (inputState & (1 << 15)) == 0; // true when bit 15 is LOW (magnet detected)
    }
    return false;
}
