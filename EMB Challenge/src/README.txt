Objective:
Embedded system design focuses on gathering useful data, processing that data, and
providing a useful representation of information. Secure communications have always
been a requirement (and a concern) for not only embedded developers, but just about
everyone sharing information. Secure transmission can be accomplished in many
different ways – some software, and some hardware. Although most communications
are complex, simple secure communication schemes can be very useful in providing
“feasibility” and “proof-of-concept”.
The objective of this embedded challenge is to build a handheld messenger 
device designed to send a message, which is 1 of 10 possible enumerated messages
across 5 nodes, i.e. 5 people. Each message will be encoded by a hand
movement sequence that can be measured by the built in accelerometer. The 5 people are assigned a node number from Node 1 to Node 5. 
Create 10 messages, enumerated as message 1 through message 10.

The challenge involves the following procedure:
1. Node 1 will then decide which message to send (1-10).

2. Node 1 will pick up Node 2’s device and “transmit” the message to Node 2 by
performing a hand movement (or sequence of movements) corresponding to the
message.

3. Node 2’s device will then light up the LED corresponding to the detected
message (using one of the 10 neopixels provided)

4. Node 2 will then pick up Node 3’s device and transmit the message using the
procedure in step 2.

5. The process repeats until the correct LED illuminates on Node 5’s device,
meaning the correct message was sent.

6. Now here is the catch. Each Node must have a different message cross
reference list. This means that all Nodes have the same 10 messages and same
10 hand sequences, but in a different enumerated order. For example if one of
the messages is “I need an A”, it may be message 5 for Node 1, message 3 for
Node 2, message 9 for Node 3 etc. But message 5 must be the same hand
gesture for all (same for all messages).

7. Objective – Transmit the message from Node 1 to Node 5

Required Parts:
1. Adafruit Classic Playground with built-in accelerometer
2. Power supply/USB power bank/connected USB cable.

Restrictions:
No other components may be used other than those specified above.
The PlatformIO programming environment must be used.
Allowed to use any drivers/HAL functions available through the IDE