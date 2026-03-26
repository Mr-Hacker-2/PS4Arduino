#include "PS4Arduino.h"

#if defined(USBCON) && !defined(PS4ARDUINO_USB)
  #warning "You need to select [____ as PS4 controller] in the boards tab to be able to communicate with your PlayStation!"
#endif

PS4Arduino PS4controller;

void PS4Arduino::writeToEndpoint(const uint8_t* data, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    UEDATX = data[i];
  }
}

void PS4Arduino::txInterruptCallback() {
  PS4controller.reportCounter = (PS4controller.reportCounter + 1) & 0x3F;
  PS4controller.report.reportCounter = PS4controller.reportCounter;

  PS4controller.timestamp++;
  PS4controller.report.axisTiming = PS4controller.timestamp;

  PS4controller.writeToEndpoint((const uint8_t*)&PS4controller.report,
                                sizeof(PS4controller.report));
  UEINTX &= ~(1 << FIFOCON);
}

void PS4Arduino::controllerOutHandler() {
  uint8_t buf[64];
  int n = USB_Recv(PS4ARDUINO_RX_ENDPOINT, buf,
                   USB_Available(PS4ARDUINO_RX_ENDPOINT));
  (void)n; // avoid unused variable warning for now
}

void PS4Arduino::begin() {
  memset(&report, 0, sizeof(report));

  report.reportID      = 0x01;
  report.leftStickX    = 0x80;
  report.leftStickY    = 0x80;
  report.rightStickX   = 0x80;
  report.rightStickY   = 0x80;
  report.dpad          = 0x08;   // neutral
  report.buttonNorth   = 0x00;
  report.buttonSouth   = 0x00;
  report.buttonEast    = 0x00;
  report.buttonWest    = 0x00;
  report.buttonL1      = 0x00;
  report.buttonL2      = 0x00;
  report.buttonL3      = 0x00;
  report.buttonR1      = 0x00;
  report.buttonR2      = 0x00;
  report.buttonR3      = 0x00;
  report.buttonHome    = 0x00;
  report.buttonTouchpad= 0x00;
  report.reportCounter = reportCounter;
  report.leftTrigger   = 0x00;
  report.rightTrigger  = 0x00;
  report.axisTiming    = timestamp;
  report.battery       = 0x11;
  report.gyrox         = 0x0000;
  report.gyroy         = 0x0000;
  report.gyroz         = 0x0000;
  report.accx          = 0x0000;
  report.accy          = 0x0000;
  report.accz          = 0x2000;

  lastReconnect = millis();

  PS4ArduinoUSB::setSendCallback(txInterruptCallback);
  PS4ArduinoUSB::setRecvCallback(controllerOutHandler);

  noInterrupts();
  UENUM = PS4ARDUINO_TX_ENDPOINT;
  if (UEINTX & (1 << RWAL)) {
    writeToEndpoint((const uint8_t*)&report, sizeof(report));
    UEINTX &= ~(1 << FIFOCON);
  }
  interrupts();

  // Initial "press home" to wake / pair
  delay(1000);
  report.buttonHome = 0x01;

  noInterrupts();
  UENUM = PS4ARDUINO_TX_ENDPOINT;
  if (UEINTX & (1 << RWAL)) {
    writeToEndpoint((const uint8_t*)&report, sizeof(report));
    UEINTX &= ~(1 << FIFOCON);
  }
  interrupts();

  delay(100);
  report.buttonHome = 0x00;
}

void PS4Arduino::maintainConnection() {
  // Use local snapshot of millis() for overflow-safe comparison
  uint32_t now = millis();

  // 480000 ms = 8 minutes; reconnect slightly before PS4 timeout
  if (now - lastReconnect >= 480000UL) {
    // Soft USB detach/attach to refresh connection
    UDCON |= (1 << DETACH);
    delay(100);
    UDCON &= ~(1 << DETACH);

    lastReconnect = now;

    delay(1000);

    // Re-register callbacks after reconnect
    PS4ArduinoUSB::setSendCallback(txInterruptCallback);
    PS4ArduinoUSB::setRecvCallback(controllerOutHandler);

    // Briefly press HOME again to re-establish
    report.buttonHome = 0x01;

    noInterrupts();
    UENUM = PS4ARDUINO_TX_ENDPOINT;
    if (UEINTX & (1 << RWAL)) {
      writeToEndpoint((const uint8_t*)&report, sizeof(report));
      UEINTX &= ~(1 << FIFOCON);
    }
    interrupts();

    delay(100);
    report.buttonHome = 0x00;
  }
}

void PS4Arduino::setDpad(dirEnum direction) {
  switch (direction) {
    case DIR_N:      report.dpad = 0; break;
    case DIR_NE:     report.dpad = 1; break;
    case DIR_E:      report.dpad = 2; break;
    case DIR_SE:     report.dpad = 3; break;
    case DIR_S:      report.dpad = 4; break;
    case DIR_SW:     report.dpad = 5; break;
    case DIR_W:      report.dpad = 6; break;
    case DIR_NW:     report.dpad = 7; break;
    case DIR_CENTER:
    default:         report.dpad = 8; break;
  }
}

void PS4Arduino::setButton(buttonEnum button, bool state) {
  switch (button) {
    case BUTTON_NORTH:     report.buttonNorth    = state; break;
    case BUTTON_EAST:      report.buttonEast     = state; break;
    case BUTTON_SOUTH:     report.buttonSouth    = state; break;
    case BUTTON_WEST:      report.buttonWest     = state; break;
    case BUTTON_SELECT:    report.buttonSelect   = state; break;
    case BUTTON_START:     report.buttonStart    = state; break;
    case BUTTON_HOME:      report.buttonHome     = state; break;
    case BUTTON_L1:        report.buttonL1       = state; break;
    case BUTTON_L2:        report.buttonL2       = state; break;
    case BUTTON_L3:        report.buttonL3       = state; break;
    case BUTTON_R1:        report.buttonR1       = state; break;
    case BUTTON_R2:        report.buttonR2       = state; break;
    case BUTTON_R3:        report.buttonR3       = state; break;
    case BUTTON_TOUCHPAD:  report.buttonTouchpad = state; break;
    default: break;
  }
}

void PS4Arduino::setTrigger(sideEnum trigger, uint8_t value) {
  switch (trigger) {
    case SIDE_LEFT:  report.leftTrigger  = value; break;
    case SIDE_RIGHT: report.rightTrigger = value; break;
    default: break;
  }
}

void PS4Arduino::setJoystick(sideEnum joystick, axisEnum axis, uint8_t value) {
  if (joystick == SIDE_LEFT) {
    if (axis == AXIS_X)      report.leftStickX  = value;
    else if (axis == AXIS_Y) report.leftStickY  = value;
  } else if (joystick == SIDE_RIGHT) {
    if (axis == AXIS_X)      report.rightStickX = value;
    else if (axis == AXIS_Y) report.rightStickY = value;
  }
}
