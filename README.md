# SerialCommandCoordinator

## Origin
I was working on a control system prototype and wanted a more interacve way to jump between diagnostic modes when working with an Arduino board, and its connected peripherals, than maintaining multiple sketch versions and flashing the board each time I needed to run it in a different mode. The solution had to have a compact enough memory footprint to be useful on the ATmega328P, yet verbose enough that someone could connect to it via a serial port to run these diagnostics. I couldn't find an open source library I felt was both efficient and simple enough so I wrote my own.

## How it Works
Given an initialized serial stream in the Arduino ecosystem that inherits from Stream (e.g. HardwareSerial or SoftwareSerial), the SerialCommandCoordinator maps a registered serial command, in the form of null terminated character array, recieved from said stream to functions via a function's memory address. These values are stored and accessed via two, parallel, unsorted arrays allocated on the heap. Since the number of commands will likely be small, performing a search of an unsorted array for mode switching is sufficient as memory footprint is valued over latency.


## How to Use It
### Main Workflow
The main workflow for using the SerialCommandCoordinator is as follows:
1. Initialize the serial stream and SerialCommandCoordinator object
2. Register a command with function
3. Wait for input and run the command if matching input arrives
``` C++
void performLampTest() {
  // code to turn on and off lamp
}

void setup() {
  Serial.begin(9600);
  SerialCommandCoordinator scc(Serial);
  scc.registerCommand("lampTest", &performLampTest);
}

void loop {
  if (scc.recieveCommandInput()) {
    scc.runSelectedCommand();
  }
}
```
Now, (e.g. using the Arduino IDE and Serial Monitor), when you enter the text **lampTest** in the Serial Monitor, the function ```performLampTest()``` will execute each time the member function ```runSelectedCommand()``` is called. In the example above this will only occur once each time the registered command **lampTest** is entered.

### Setting an Exit Case
//TODO talk about 'q' case

### Considerations
 TODO - talk about initialization: They are also only allocated on the heap once at initialization to prevent memory fragmentation
 TODO - talk about baud rate
 TODO - talk about limits on num registered commands
 TODO - talk about memory freed on destruction
