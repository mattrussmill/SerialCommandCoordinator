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
#ifndef SERIALCOMMANDCOORDINATOR_h
#define SERIALCOMMANDCOORDINATOR_h

#include "Arduino.h"

class SerialCommandCoordinator
{
  public:

    SerialCommandCoordinator(Stream &device);
    SerialCommandCoordinator(Stream *device);
    virtual ~SerialCommandCoordinator();

    // Checks if there is data available in the serial recieve buffer. If
    // there is, it copies the data byte by byte into _inputBuffer until the 
    // predefined _endMarker is reached (preset to \n) and sets _inputValid to
    // true. If the serial stream would overflow _inputBufferSize, the
    // data is truncated (to _inputBufferSize - 2), and a null terminator is
    // appended at the end of the _inputBuffer (_inputBufferSize - 1) instead 
    // of where _endMarker defines. The remaining data in the serial recieve
    // buffer is emptied and _inputValid is set to false. Returns _inputValid
    // if new input data is available, else returns false. 
    bool recieveInput();

    // First calls recieveInput. If receiveInput is successful, _inputBuffer
    // contains a valid string, attempts to set the selected command with
    // setSelectedFunction(). If successful, returns true and the selected
    // command can be run via runSelectedCommand(). If the command is not
    // recognized, no command will be pre-selected and returns false.
    bool recieveCommandInput();

    // Given a null terminated string and function address, attempts to register
    // a command with its intended routine. It fails and returns false if the 
    // command is already in the list, the list is full, nullptr is an argument,
    // or allocation of memory to store command fails. Returns true on success.
    bool registerCommand(const char *command, const void (*function)(void));

    // If the last call to recieveCommandInput() is successful, will run the most
    // recently selected function matching a valid command from the _commandList.
    void runSelectedCommand();

    // Prints all commands currently registered in the _commandList.
    void printCommandList();

    // Prints the current value stored in the _inputBuffer.
    void printInputBuffer();

    // Sets the appropriate delay time for reading long strings of text from the input _device.
    void setBaudRate(long baudRate);

    // Returns a pointer to the _inputBuffer for use outside of the class.
    const char* getSerialBuffer();
    
     // Stream test function. Prints a single line to the stream reference for testing purposes.
    void testStream();               

  private:
    typedef void(*func_ptr_t)(void);  // Type definition for function pointer (for readability).

    // Initializing code to be shared between all constructors
    void init();

    // Sets the function to be selected
    bool setSelectedFunction();

    Stream *_device = nullptr;        // Address to input stream.
    uint8_t _deviceDelay = 67;        // *Minimum delay in ms, at the provided baud rate, to fill the _device buffer
    char _endMarker = '\n';           // Designated end marker for input stream.

    bool _inputValid = false;         // State of input buffer fitting entirely within the _inputBuffer.
    uint8_t _inputBufferSize = 32;    // Size of the _inputBuffer to be allocated.
    uint8_t _inputDelay = 34;         // *Minimum delay in ms, at the provided baud rate, to fill the _inputBuffer
    char *_inputBuffer = nullptr;     // Input buffer address for stream input.

    uint8_t _commandListSize = 8;           // Shared index for commands and functions.
    char **_commandList = nullptr;          // List of addresses for registered commands.
    func_ptr_t *_functionList = nullptr;    // List of addresses for registered functions.
    func_ptr_t _functionSelected = nullptr; // Selected function to be run with runSelectedCommand().

    // * Initial delay value is based on 9600 baud rate using the following conversion:
    //    => 1 / ( (baud rate bytes / sec) / buffer-size bytes) 
    //      => assuming 1 byte is 10 bits (1 start & 1 stop bit)
    //    => _inputDelay  = 1000 / ( (960 bytes / sec) / _inputBufferSize() bytes ) (e.g. based on 32 byte buffer size)
    //    => _deviceDelay = 1000 / ( (960 bytes / sec) / SERIAL_BUFFER_SIZE bytes ) (e.g. hardcoded as 64 in HardwareSerial.cpp for Arduino ecosystem)

};

#endif /* SERIALCOMMANDCOORDINATOR_h */
