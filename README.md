# Working with the CC3200

A small sample using the CC3200 LaunchPad from Texas Instruments.

![CC3200 LaunchPad](images/bare.jpg)

## Overview

In this sample we will work with the CC3200 LaunchPad. Initially, our target will be to integrated LED blinking. Then we will use one of the available sensors to control the blinking frequency. Finally, we will get the blinking frequency from some service accessible via HTTP.

## Requirements

In order to fully cover this tutorial we need the following setup:

* The TI CC3200-LaunchXL board
* One USB micro cable
* Computer with Windows, Linux, or macOS and an USB port
* Installed Energia IDE
* Internet access via WiFi

We can use the editor provided by Energia or any other editor, e.g., Sublime Text, Emacs, or vim.

## Step-By-Step

We start by running the Energia IDE. In case the Energia IDE has not been run (with the CC3200) previously we need to follow some instructions.

### Energia: First-Time Setup

Depending on the OS different instructions need to be followed.

**Windows**

The CC3200 should not be connected, otherwise we should remove it first. The following instructions are useful to setup the drivers for Energia.

1. Download the CC3200 Drivers for Windows from [here](http://energia.nu/files/cc3200_drivers_win.zip).
2. Unzip and run *DPinst.exe* for the 32-bit version or *DPinst64.exe* for 64-bit version of Windows.
3. Follow the installer instructions.
4. Connect the CC3200 LaunchPad. The CC3200 should be automatically recognized.

For Windows 8 or later we may need to disable the signed driver feature, [see this guide](https://learn.sparkfun.com/tutorials/disabling-driver-signature-on-windows-8).

**Linux**

On recent versions of Linux the FTDI kernel driver that is needed for the CC3200 LaunchPad is already installed. The only thing we need to do is to tell Linux to use that driver when we plug in the CC3200 LaunchPad.

**macOS**

The CC3200 should not be connected, otherwise we should remove it first. The following instructions are useful to setup the drivers for Energia.

1. Download the CC3200 Drivers for macOS from [here](http://energia.nu/files/EnergiaFTDIDrivers2.3.zip).
2. Unzip and run *EnergiaFTDIDrivers2.2.18.pkg*.
3. Follow the installer instructions.
4. Reboot the system.
5. Connect the CC3200 LaunchPad. The CC3200 should be automatically recognized.

### Energia Basics

The Energia IDE looks as presented in the next image.

![Energia IDE](images/energia.png)

It contains a code area with a mediocre text editor. The output is shown in the black box on the bottom. The most important file is an Energia project (*x.ino*), which is always contained in a folder with the name of the file (by convention). Even though the Energia project file contains some C-like code it is fully conventional driven, i.e., instead of defining methods and including external files, some important definitions are already made.

The board specific header file and the definitions for `setup` and `loop` have been made already. These two functions will be called from implicitly generated code and are therefore special. `setup` is run during the initialization, `loop` after the initialization. The latter never finishes, as it is invoked over and over again (hence the name `loop`).

For debugging purposes the serial monitor will be crucial.

![Energia Serial Monitor](images/serial.png)

In *File* / *Preferences* we'll find the option to tell Energia we are using an external editor.

### The Code

The board comes with three LEDs that can be used in such samples.

* `RED_LED` (29)
* `GREEN_LED` (10)
* `YELLOW_LED` (9)

All the hardware addresses and definitions can be found in *hardware/cc3200/variants/launchpad/pins_energia.h*. Initially, our code may be as simple as follows:

```C
void setup() {
  Serial.begin(9600);
  Serial.println("Entering setup!");
}

void loop() {
  Serial.println("Next loop iteration!");
  delay(5000);
}
```

This code instructs the LaunchPad to initialize the serial port for sending messages (at 9600 boud). Afterwards, we are printing "Entering setup!" once and "Next loop iteration!" every five seconds. The `delay` function puts the device to sleep for the specified amount of milliseconds.

The previously mentioned three LEDs may now be used as follows (instead of their numeric values we fall back to the named macros - its just much easier to read):

```C
void setup() {
  Serial.begin(9600);
  Serial.println("Entering setup!");  
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void loop() {
  Serial.println("Next loop iteration!");
  digitalWrite(RED_LED, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED, HIGH);
  delay(500);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED, LOW);
  delay(1000);
  digitalWrite(YELLOW_LED, LOW);
  delay(500);
}
```

The library that is used by Energia needs to know the type of the pins that are used. Therefore, we have to setup the LEDs in the beginning to be initialized with the right pin mode. For all of them we use the `OUTPUT` setting (resolving to 1, whereas `INPUT` would resolve to 0). The `digitalWrite` function is then used to write a value to a given address, e.g., 1 (named `HIGH`) to the address of the `RED_LED` or 0 (named `LOW`) to the address of the `GREEN_LED`.

What happens here is that we get the following sequence:

* RED (1s)
* RED/GREEN (0.5s)
* GREEN/YELLOW (1s)
* YELLOW (1s)
* (dark) (0.5s)

Now its time to the connect the LaunchPad and upload it!

### Connecting the LaunchPad

The easiest way to get it on the LaunchPad is to press `CTRL + M` in Energia. This will compile the binaries, upload the code, open the serial monitor, and run the application. However, by design the LaunchPad does not come with a very programmer friendly method of dealing with the reprogramming. It is required to set different jumpers to go from run-mode to program-mode and back again.

The following image shows how this can be circumvented using the J8 (TOP) connector together with the SOP2 (BOTTOM).

![CC3200 Jumpers for Quick Execution](images/jumper.png)

The following instructions give some more detail:

1. We start by removing both jumpers J8 and SOP2 (if installed).
2. With the USB connector facing up, we connect one side of the jumper wire to the top of J8 and the other side to the bottom of SOP2.

Now when we run the code from Energia via `CTRL + M` we will see the serial monitor popping up (be sure to have the correct boud rate configured!) and the LEDs start blinking.

At this point in time our setup looks similar to the next image.

![CC3200 blinking LEDs](images/leds.jpg)

So far so good. The next step is to get a little bit more interactive. We could either use one of the buttons,

* `PUSH1` (3)
* `PUSH2` (11)

or some information from an integrated sensor. For the buttons we would use `digitalRead` to get the state (`HIGH` or `LOW`). This could be done via polling.

### Using the Accelerometer

The board comes with some sensors. There is the temperature sensor (called `tmp006`), which is available at the address 0x41, and an accelerometer (`bma222`) at 0x18. In this sample we will use the accelerometer as it is far easier to use for demo purposes (the latency and accuracy for manipulating temperatures is quite difficult to manage).

**Warning** As there is an overlap in the address register we can't use the yellow and green LED together with the accelerometer. Consequently, we will only use the red LED from now on.

For using the accelerometer we will include the *Wire.h*. Also we should put the code we write to read from the accelerometer in a new file. We will call this *accelerometer.cpp* with the header *accelerometer.h*. Initially, our code looks as follows:

```C
#include <Wire.h>
#include "accelerometer.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Entering setup!");  
  Wire.begin();
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  Serial.println("Next loop iteration!");
  AccData acc = readAccelerometer();
  Serial.print(acc.x);
  Serial.print(", ");
  Serial.print(acc.y);
  Serial.print(", ");
  Serial.println(acc.z);
  digitalWrite(RED_LED, HIGH);
  delay(1000);
}
```

The `readAccelerometer` function comes from our header. In the setup we need to initialize the Wire library. In each iteration we read the acceleromter and print the values to the screen. Running this in Energie should yield output as shown below.

![CC3200 Accelerometer Output](images/accelerometer.png)

The `AccData` structure is defined in our header. The code here looks as follows.

```C
#pragma once
#include <stdint.h>

struct AccData {
  int8_t x; 
  int8_t y;
  int8_t z;
};

AccData readAccelerometer();
```

Quite straightforward. The source file is much more interesting.

```C
#include "accelerometer.h"

// ...

int8_t readSingleAxis(uint8_t axis);

AccData readAccelerometer() {
  AccData data;
  data.x = readSingleAxis(0x03);
  data.y = readSingleAxis(0x05);
  data.z = readSingleAxis(0x07);
  return data;
}
```

We read the registers for each of the components and return the full result. The question is now how the reading for a single component is defined. The rest is displayed below.

```C
#include <Energia.h>
#include <Wire.h>

void initializeI2C(uint8_t base_address, uint8_t register_address) {
  Wire.beginTransmission(base_address);
  Wire.write(register_address);
  Wire.endTransmission();
}

uint8_t readI2C(uint8_t base_address, uint8_t register_address) {
  initializeI2C(base_address, register_address);
  Wire.requestFrom(base_address, OUTPUT);

  while (Wire.available() < HIGH);

  return Wire.read();
}

int8_t readSingleAxis(uint8_t axis) {
  return readI2C(0x18, axis);
}
```

Each read operation starts an I2C transmission at the given address. We remember that the address for the accelerometer was 0x18. The access is given by the register specified in the call. The whole flow is only representing the expected I2C timing diagram.

Having written this part we can adjust the code in the `loop` function to show the red light in case of a "falling" board. From our elementary physics course in school we remember that a falling body is essentially free, i.e., the acceleration in `z` direction will be zero (as compared to `1` in units of *g* for a body standing on the earth's surface).

As we measure a value of approx. `65` in z-direction by default we may normalize to this value. We should show a red light for values below 0.4 *g*. Our modified code looks as given below.

```C
#include <Wire.h>
#include "accelerometer.h"

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  AccData acc = readAccelerometer();
  Serial.println(acc.z);

  if (acc.z > 26) {
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(RED_LED, HIGH);  
  }
}
``` 

Potentially, we want to the warning to stay active for at least some time - in this case we may modify the code to look as follows.

```C
if (acc.z > 26) {
  digitalWrite(RED_LED, LOW);
} else {
  digitalWrite(RED_LED, HIGH);  
  delay(1000);
}
```

This will activate the LED for at least 1 second.

### HTTP Requests

Until this point we already archieved quite a lot. We started with a simple kind of "Hello World" and reached a stage where we are able to gather sensor information and trigger outputs based on the collected information. Now we want to go one step further.

We can use [random.org](https://random.org) to make a request to its API, e.g., [https://www.random.org/integers/?num=1&min=1&max=100&col=1&base=10&format=plain&rnd=new](https://www.random.org/integers/?num=1&min=1&max=100&col=1&base=10&format=plain&rnd=new). The resulting number will then be displayed using a blinking LED. A five-second pause (LED off) is inserted after each HTTP call.

Before we can start with an HTTP call we need to establish a network connection. Luckily, the CC3200 LaunchPad comes with a WiFi chip on-board.

Similar to the integration of the accelerometer we put the code in a new file *wifi.cpp*. The header is quite simple. Here we only declare a single function.

```C
#pragma once

void connectWifi(char* ssid, char* password);
```

The source file contains quite some debug printing, which is useful to identify what's going on in the setup process. To make it fully work we use the WiFi library that comes with Energia.

```C
#include "wifi.h"
#include <Energia.h>
#include <WiFi.h>

void connectWifi(char* ssid, char* password) {	
  Serial.print("Connecting to WIFI network ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println(" connected!");
  Serial.print("Waiting for an IP address ");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println(" received!");
}
```

The steps included here are simple. We start by connecting to the WiFi network. We then wait until the connection is established. Finally, we also wait until the router assigned us some IP. At this point in time the WiFi connection is established and ready to be used.

How to we now make an HTTP request? Well, it turns out that this is not such an easy task! We'll start with a request to [httpbin.org](http://httpbin.org/) before we deal with [random.org](https://random.org).

In the beginning our API is as simple as the following declaration:

```C
bool httpGetRequest(char* hostname, char* path);
```

We just pass in a hostname, e.g., *httpbin.org*, and a path. In our case we choose `/bytes/4` to obtain 4 random bytes. The source code for the handling function looks close to the code shown below.

```C
#include <Energia.h>
#include <WiFi.h>

bool httpGetRequest(char* host, char* path) {
  String hostname = String(host);
  String head_post = "GET " + String(path) + " HTTP/1.1";
  String head_host = "Host: " + hostname;
  String request = head_post + "\n" + 
                   head_host + "\n\n";

  char receive_msg_buffer[1024];
  uint32_t host_ip;
  bool success = false;

  SlTimeval_t timeout { .tv_sec = 45, .tv_usec = 0 };
  
  if (sl_NetAppDnsGetHostByName((signed char*)hostname.c_str(), hostname.length(), &host_ip, SL_AF_INET)) {
    return false;
  }

  SlSockAddrIn_t socket_address {
    .sin_family = SL_AF_INET, .sin_port = sl_Htons(80), .sin_addr = { .s_addr = sl_Htonl(host_ip) }
  };

  uint16_t socket_handle = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, IPPROTO_TCP);

  if (sl_SetSockOpt(socket_handle, SL_SOL_SOCKET, SL_SO_RCVTIMEO, (const void*)&timeout, sizeof(timeout)) >= 0 &&
      sl_Connect(socket_handle, (SlSockAddr_t*)&socket_address, sizeof(SlSockAddrIn_t)) >= 0 &&
      sl_Send(socket_handle, request.c_str(), request.length(), 0) >= 0 &&
      sl_Recv(socket_handle, receive_msg_buffer, sizeof(receive_msg_buffer), 0) >= 0) {
    Serial.println(receive_msg_buffer);
    success = true;  
  }

  sl_Close(socket_handle);
  return success;
}
```

First we define the request message to send. Note that the double newline at the end is crucial; it is indicating that the header ends here. Otherwise we'll receive a timeout as the server waits for more. Then we retrieve the IP address of the host to connect to. Then we set up the connection socket as TCP/IP on port 80 (standard HTTP connection). The steps to follow are then described as

* setting socket options; in this case we only set the timeout to 45s,
* connecting to the socket using the previously defined options
* sending the content: we transmit the request, which is given by the headers only, and finally
* receiving the answer.

The response will be available as a single string - there is no distinction between headers and content body by default. We need to do the parsing.

Once we deploy the code we should see something similar to the following:

```plain
HTTP/1.1 200 OK
Server: nginx
Date: Sat, 18 Jun 2016 07:24:31 GMT
Content-Type: application/octet-stream
Content-Length: 4
Connection: keep-alive
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

L=r
```

If we did not receive anything, we should do some debugging to find the origin of the problem.

### Debugging the LaunchPad

Debugging software on embedded systems is difficult. Most of the tooling and techniques we've learned to love are not available. Essentially, we are back to the stone age of programming. We've already seen that the `Serial` class represents a useful utility to gain some knowledge about what's actually going on.

(tbd)

### HTTPS with the TPM

(tbd)

## Conclusions

The LaunchPad is a great device to quickly get going in the embedded world. It comes with a lot of interesting possibilities, such as a TPM or a WiFi chip. The available booster packs and open-design make it an ideal companion for rapid prototyping or IoT sketches in general.

## References

* [Energia IDE](http://energia.nu)
* [Energia CC3200 Guide](http://energia.nu/cc3200guide/)
* [SimpleLink Wi-Fi CC3200 LaunchPad Datasheet](http://www.ti.com/tool/cc3200-launchxl)
* [Sample Project: BabyZen](http://www.codeproject.com/Articles/891868/BabyZen-IoT-with-Azure)
* [CC3200 SimpleLink LaunchPad User Guide](http://www.ti.com/lit/pdf/swru397)
* [CC3210 SimpleLink Wireless MCU Specification](http://www.ti.com/lit/gpn/cc3200mod)
