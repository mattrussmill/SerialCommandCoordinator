/**
 *
 * SerialCommandCoordinator library for Arduino
 * v0.0.1
 * https://github.com/mattrussmill/SerialCommandCoordinator
 *
 * MIT License
 * (c) 2023 Matthew Miller
 *
**/
#include <Arduino.h>
#include <math.h>
#include "SerialCommandCoordinator.h"

SerialCommandCoordinator::SerialCommandCoordinator(Stream &device) : _device(&device) {
  init();
}

SerialCommandCoordinator::SerialCommandCoordinator(Stream *device) : _device(device) {
  init();
}

SerialCommandCoordinator::~SerialCommandCoordinator() {
  if (_inputBuffer) {
    free(_inputBuffer);
  }
  
  // currently manages all command string memory
  if (_commandList) {
    for (int i = 0; i < _commandListSize; i++) {
      if (_commandList[i]) {
        free(_commandList[i]);
      }
    }
    free(_commandList);
  }

  if (_functionList) {
    free(_functionList);
  }
}

bool SerialCommandCoordinator::receiveInput() {
  uint8_t ndx = 0;
  bool newInput = false;
  char rc;
  if (_device->available() > 0){
    delay(_inputDelay);
    while (_device->available() > 0 && newInput == false) {
      rc = _device->read();

      if (rc != _endMarker && ndx < _inputBufferSize - 1) {
        _inputBuffer[ndx] = rc;
        ndx++;

      } else {
        _inputBuffer[ndx] = '\0'; // terminate the string
        newInput = true;

        if (rc != _endMarker && ndx >= _inputBufferSize - 1) {
          _inputValid = false; // input string too large for buffer

          while (_device->available() > 0 ) { 
            while (_device->available() > 0 ) {
              _device->read(); // clear the remaining serial buffer
            }
            delay(_deviceDelay);
          }

        } else {
          _inputValid = true;
        }
      }
    }
  }

  return newInput && _inputValid;
}

bool SerialCommandCoordinator::receiveCommandInput() {
  if (receiveInput()) {
    return setSelectedFunction();
  }
  return false;
}

void SerialCommandCoordinator::printInputBuffer() {
  _device->println(_inputBuffer);
}

void SerialCommandCoordinator::setBaudRate(long baudRate) {
  if (baudRate <= 0) {
    return;
  }
  _inputDelay = ceil(1000.0 / ((double)(baudRate / 10) / (double)_inputBufferSize)) ; // 1000 for ms conversion
  _deviceDelay = ceil(1000.0 / ((double)(baudRate / 10) / 64.0)); // 64 is Arduino standard serial buffer size
}

bool SerialCommandCoordinator::registerCommand(const char *command, const void (*function)(void)) {
  if (command == nullptr || function == nullptr) {
    return false;
  }
  
  // find next empty spot in list
  int ndx = 0;
  while (ndx < _commandListSize) {
    // find next empty spot in list
    if (_commandList[ndx] == nullptr) {
      break;

    // command already in list
    } else if (strcmp(_commandList[ndx], command) == 0) {
      return false;
    }
    ndx++;
  }

  // Command buffer full, cannot register command;
  if (ndx >= _commandListSize) {
    return false; 
  }

  // Store command and function address
  int size = strlen(command) + 1;
  _commandList[ndx] = (char*) malloc(size * sizeof(char));
  if (_commandList[ndx] == nullptr) {
    return false;
  }
  strcpy(_commandList[ndx], command);
  _functionList[ndx] = function;
  return true;

}

void SerialCommandCoordinator::runSelectedCommand() {
  if (!_inputValid || _functionSelected == nullptr) {
    return;
  }
  (*_functionSelected)();
}

void SerialCommandCoordinator::printCommandList() {
  for (int i = 0; i < _commandListSize; i++) {
    if (_commandList[i] != nullptr) {
      _device->println(_commandList[i]);
    }
  }
}

void SerialCommandCoordinator::testStream() {
  _device->println("Hello World!"); 
}

void SerialCommandCoordinator::init() {
  _inputBuffer = (char*) calloc(_inputBufferSize, sizeof(char));
  _commandList = (char**) calloc(_commandListSize, sizeof(char*));
  _functionList = (func_ptr_t*) calloc(_commandListSize, sizeof(func_ptr_t*));

  if (_inputBuffer == nullptr || _commandList == nullptr || _functionList == nullptr) {
    abort();
  }
}

bool SerialCommandCoordinator::setSelectedFunction() {
  int ndx = 0;
  _functionSelected = nullptr;

  while (ndx < _commandListSize && _inputValid) {

    // registered functions can't be removed, nullptr is end of list = not found
    if (_commandList[ndx] == nullptr) {
      return false;
    }

    // found function
    if (strcmp(_inputBuffer, _commandList[ndx]) == 0) {
      _functionSelected = _functionList[ndx];
      return true;
    }
    ndx++;    
  }

  // not found in list
  return false;
}
