# Network Diagnostic Tool Protocol (NDTP Version 3.7.0) #

## Abstract ##

The Network Diagnostic Tool (NDT) is a client/server program that provides network configuration and performance testing to a user's computer. It uses a well-defined protocol (Network Diagnostic Tool Protocol - NDTP) to provide a reliable communication link between client and server. This protocol documentation is sufficient to allow outside parties to write compatible NDT clients without consulting the NDT source code. This documentation contains a state transition diagrams detailing the various states that the components (both server and client) can be in, and the valid messages that can or will be transmitted or received in those states.

## Table of Contents ##



## Introduction ##

Several studies have shown that the majority of network performance problems occur in or near the users’ desktop/laptop computer.  These problems include, but are not limited to, duplex mismatch conditions on Ethernet/FastEthernet links, incorrectly set TCP buffers in the local network infrastructure (e.g. first hop switch/router, hubs, etc.) and bad or dirty cables. NDT seeks to determine why a network connection exhibits certain performance characteristics. In addition to measuring the familiar upload and download speeds of a user's connection, NDT also performs tests that can assess factors such as latency, packet loss, congestion, bad cables, out of order delivery and bottlenecks on the end-to-end path from client to server (on either side of the connection).

The NDT popularity has grown in recent years, which led to an increased interest from the developers community around the world. People started to integrate the NDT with their systems using a different approaches, like adding a Web Developer's API to a Web based (java applet) client and writing a new mobile (Android based) client.

The NDTP allows outside parties to easily extend both the NDT server and client parts and write compatible clients in new languages that will be able to communicate with the publicly available Measurement Lab servers.

In this document, the key words "MUST", "REQUIRED", "SHOULD", "RECOMMENDED", and "MAY" are to be interpreted as described in [RFC2119](http://www.ietf.org/rfc/rfc2119.txt).

### Relationship of Tests and Control Protocols ###

The NDTP actually consists of a two inter-related sets of protocols: NDTP-Control and NDTP-Tests. NDTP-Control is used to initiate, start, and stop test sessions and to fetch their results, whereas NDTP-Tests is a set of smaller sub-protocols concentrated on testing a specific network performance problems.

The NDTP-Tests consists of a five smaller sub-protocols: Middlebox test, Simple firewall test, C2S (Client to Server) throughput test, S2C (Server to Client) throughput test and META test. These tests are completely independent from each other and can be run in any order or even skipped. Moreover, a new NDTP-Tests protocols can be easily created and integrated into the existing tests ecosystem.

The NDTP-Control is designed to support the negotiation of test sessions and results retrieval in a straightforward manner. At the session initiation, there is a negotiation of the test suite and the session start time. Moreover, the server version is being verified to discover the possibility of incompatibilities. Additionally, the NDTP-Control defines an exact message format, which greatly increase the general protocol reliability and allows other NDTP-Tests protocols to communicate in a convenient and a more structured fashion.

The NDTP-Control protocol can effectively be used to implement not only a new types of clients, but also can be treated as a generic framework to create and combine with each other a completely new NDTP-Tests protocols.

### Logical Model ###

Several different roles on both NDTP-Control and NDTP-Tests levels can be distinguished. Specifically, the following ones should be highlighted:

| **Role** | **Description** |
|:---------|:----------------|
| Session-Sender | The sending endpoint of the NDTP-Tests session. |
| Session-Receiver | The receiving endpoint of the NDTP-Tests session. |
| Server | An end system that manages the NDTP-Control and NDTP-Tests sessions, performs the specific tests needed to determine what problems, if any, exist. The server process then analyzes the test results and returns these results to the client. |
| Client | An end system that initiates requests for the NDTP-Tests sessions, triggers the start of a set of sessions and receives the tests results. |

Different logical roles can be played by the same host. For example the Server machine is a Session-Sender in the S2C throughput test, while it is a Session-Receiver in the C2S throughput test.

Please look at the figures describing the two main NDTP-Tests protocols:

  * C2S throughput test

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/1.%20C2S%20throughput%20test.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/1.%20C2S%20throughput%20test.png)

  * S2C throughput test

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/2.%20S2C%20throughput%20test.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/2.%20S2C%20throughput%20test.png)

## Protocol Overview ##

As described above, the NDTP consists of a two inter-related sets of protocols: NDTP-Control and NDTP-Tests. Both of them are layered over TCP. The former is used to initiate and control the test sessions and to fetch their results. The latter set of protocols is used to perform the specific tests.

The initiator of the test session establishes a TCP connection to a well-known port, 3001, on the target point and this connection remains open for the duration of the NDTP-Tests sessions. The NDT server SHOULD listen to this well-known port.

The NDTP-Control messages are transmitted before the NDTP-Tests sessions are actually started, after they are completed and between particular tests from the performed test suite. Moreover, a specific tests also use the NDTP-Control messages to synchronise the test sessions, exchange a configuration data and additional information.

### Protocol Encodings ###

All multi-byte quantities defined in this document are represented as unsigned integers in a network byte order unless specified otherwise. All single byte charactor quantities are US-ASCII encoded unless specified otherwise. C style character syntax is used in this document to represent US-ASCII encodings for values. i.e. '2' represents US-ASCII value for the charactor numeral two which is hex 32 or decimal 50 and not the integer value two. All multiple byte **string** quantities are US-ASCII encoded unless otherwise specified. The term octet is used when describing bytes in the wire-format of the messages to be more consistent with RFC style language.

### Protocol messages format ###
As of version 3.7.0 messages are transmitted using JSON format. However to support older clients which does not use this, NDT server first recognizes if client supports such encoding (that is if client sent MSG\_EXTENDED\_LOGIN message) and if not, then messages aren't converted to JSON.
JSON messages use map object so that specific values can be obtained using specific key (<font color='red'>IMPORTANT: if not specified otherwise then its default value is "msg"</font>).

## NDTP-Control ##

The type of each NDTP-Control message can be found after reading the first octet. The length of each NDTP-Control message can be computed upon reading its fixed-size part. No message is shorter than 3 octets.

An implementation SHOULD expunge unused state to prevent denial-of-service attacks, or unbounded memory usage, on the server.  For example, if the full control message is not received within some number of seconds after it is expected, the TCP connection associated with the NDTP-Control session SHOULD be dropped. A value of no more than 60 seconds is RECOMMENDED.

### Message structure ###

Each of the NDTP-Control messages always has the following format:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/3.%20Each%20of%20the%20NDTP-Control%20messages%20always%20has%20the%20following%20format.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/3.%20Each%20of%20the%20NDTP-Control%20messages%20always%20has%20the%20following%20format.png)

### Message types ###

The NDTP-Control currently defines the following types of messages:

| **Symbolic name** | **Byte integer value** | **Description** |
|:------------------|:-----------------------|:----------------|
| COMM\_FAILURE | 0 | Notification about communication link failure. |
| SRV\_QUEUE | 1 | Message used to the Clients' queue management |
| MSG\_LOGIN | 2 | Used during initialization to negotiate the NDT version and the test suite. <font color='red'>As of version 3.7.0 this type of message is deprecated and kept only for backward compatibility, newer clients should use MSG_EXTENDED_LOGIN message instead, see below)</font> |
| TEST\_PREPARE | 3 | Used to send all information needed for a particular test (i.e. port numbers, test time, etc.) |
| TEST\_START | 4 | Message used to start a specific test |
| TEST\_MSG | 5 | Used during tests to communicate between the Client and the Server |
| TEST\_FINALIZE | 6 | Message used to end a specific test |
| MSG\_ERROR | 7 | Used to send an error messages and notifications about invalid states of the test session |
| MSG\_RESULTS | 8 | Contains a final test sessions' results |
| MSG\_LOGOUT | 9 | This is the last message send by the Server after MSG\_RESULTS messages. The Server SHOULD close the NDTP-Control connection after sending this message. |
| MSG\_WAITING | 10 | Message from the Client to notify the Server that the Client is still alive |
| MSG\_EXTENDED\_LOGIN | 11 | Extended version of MSG\_LOGIN message (contains additional client version info and indicates that server and client will use JSON format for message encoding) |

### Protocol description ###

#### Logging in to the server with a test suite request ####

Before the Client can start the test session, it has to establish a connection to the Server.

First, the Client opens a TCP connection to the Server on a well-known port 3001 and sends an EXTENDED\_LOGIN message:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/EXTENDED_LOGIN_MESSAGE.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/EXTENDED_LOGIN_MESSAGE.png)

Where TESTS is a bitwise OR of the tests' ids that should be run in a requested test-suite and subsequent bytes are client version number in US-ASCII. The Tests' ids are defined in the [NDTP-Tests](NDTProtocol#NDTP-Tests.md) section.

All Clients MUST always request the TEST\_STATUS (16 (1L << 4)) test in order to notify the Server that they support the MSG\_WAITING messages introduced in the NDT v3.5.5.

#### Kick off message to disconnect old clients ####

In the same time the Server MUST send the specially crafted 13 octets long data that kicks off the old Clients. This message does not follow the NDTP-Control protocol message format and MUST be sent verbatim as a string of US-ASCII bytes:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/5.%2013%20octets%20long%20data%20that%20kicks%20off%20the%20old%20Clients%20%28%22123456%20654321%22%29.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/5.%2013%20octets%20long%20data%20that%20kicks%20off%20the%20old%20Clients%20%28%22123456%20654321%22%29.png)

All non-compatible Clients MUST drop the connection after receiving the above message. This message is different from all other message types, because it was designed to exploit the old Clients behavior, that tried to treat these string encoded numbers as the port numbers.

#### Queuing clients ####

Next, the NDT Server MUST send a one or more SRV\_QUEUE messages to inform the Client whether or not it needs to wait, or can start performing tests. These messages MUST contain one of the following string values:

| **SRV\_QUEUE value** | **Description** |
|:---------------------|:----------------|
| "0" | The test session will start now |
| "9977" | Server Fault: test session will be terminated for unknown reason. Client MUST drop the connection after receiving this message. |
| "9988" | If this is a first message from the Server, then it means that the Server is busy. In other cases it means the Server Fault. the Client MUST drop the connection after receiving this message. |
| "9990" | This is a check from the Server to verify if the queued Client is alive. Available from v3.5.5. The Client MUST respond to this message by sending an empty 'MSG\_WAITING' message. The Server will only send these messages to the queued Clients that requested TEST\_STATUS test. |
| "N" | Where N is any other value. It means the estimated number of minutes that the Client will wait for its test to begin. |

When the NDT Server decides that the Client can start its test session, it sends the SRV\_QUEUE message with value **'0'**:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/6.%20sends%20SRV_QUEUE%20message%20with%20value%20%270%27.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/6.%20sends%20SRV_QUEUE%20message%20with%20value%20%270%27.png)

The SRV\_QUEUE messages are only sent to the queued Clients.

#### Verifying version compatibility ####

At the beginning of the test session the Server and the Client MUST verify their version compatibility.

The Server MUST send a MSG\_LOGIN message containing its version identifier in the following form:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/7.%20MSG_LOGIN%20message%20containing%20its%20version%20identifier%20in%20the%20following%20form.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/7.%20MSG_LOGIN%20message%20containing%20its%20version%20identifier%20in%20the%20following%20form.png)

where VERSION is the current NDT version encoded as ascii string, for example **"3.6.4"**.

It is RECOMMENDED, that the Client display a warning message in case of version difference. Moreover, the Client MAY also drop a connection in case of version differences.

The current version of the protocol is backwards compatible to version 3.3.12. Clients detecting a version earlier than this SHOULD drop the connection.

#### Negotiating test suite ####

In the next step the NDT Server MUST send a MSG\_LOGIN message containing a list of tests' ids, encoded as ascii string, that will be performed. This list of tests's ids MUST NOT contain any tests that were not requested by the Client in the first MSG\_LOGIN message. The Server's MSG\_LOGIN message looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/8.%20encoded%20as%20ascii%20string,%20that%20will%20be%20performed.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/8.%20encoded%20as%20ascii%20string,%20that%20will%20be%20performed.png)

This list is a string containing space separated numbers that describes the particular tests. The test suite containing all five tests (Middlebox test, Simple firewall test, C2S throughput test, S2C throughput test and META test) will be encoded in the following message:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/ndt_8b.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/ndt_8b.png)

The Client MUST drop the connection when it receives unknown test id, because such situation means an error on the Server's part.

The next step is to run the negotiated test suite. The tests MUST be performed in the order received from the Server. Any specific test related configuration (like port numbers, test duration, etc.) MUST be negotiated by the particular NDTP-Tests sub-protocol.

#### Results retrieval ####

After the last accomplished NDTP-Tests test (the particular NDTP-Tests sub-protocols are described in the [NDTP-Tests](NDTProtocol#NDTP-Tests.md) section) the Server MUST send all gathered results to the Client. These results MUST be sent in a set of MSG\_RESULTS messages:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/9.%20These%20results%20MUST%20be%20sent%20in%20a%20set%20of%20MSG_RESULTS%20messages.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/9.%20These%20results%20MUST%20be%20sent%20in%20a%20set%20of%20MSG_RESULTS%20messages.png)

All standard results MUST be encoded as a human-readable strings to allow the Client to display them directly on the screen without any modifications.

At the end the Server MUST close the whole test session by sending an empty MSG\_LOGOUT message and closing connection with the Client.

The whole test session overview can be seen on the following sequence diagram. Both the Server and the Client MUST follow the described sequence:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/10.%20Both%20Server%20and%20Client%20MUST%20follow%20the%20described%20sequence.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/10.%20Both%20Server%20and%20Client%20MUST%20follow%20the%20described%20sequence.png)

## NDTP-Tests ##

The NDTP-Tests currently defines the following types of tests:

| **Symbolic name** | **Integer value** | **Description** |
|:------------------|:------------------|:----------------|
| TEST\_MID | 1 (1L << 0) | Middlebox test - a short (5 seconds) throughput test from the Server to the Client with a limited CWND and pre-defined MSS value |
| TEST\_C2S | 2 (1L << 1) | 10 seconds C2S throughput test |
| TEST\_S2C | 4 (1L << 2) | 10 seconds S2C throughput test |
| TEST\_SFW | 8 (1L << 3) | Simple firewall test |
| TEST\_META | 32 (1L << 5) | META test |

Moreover, the NDT v3.5.5 introduced a new mechanism to avoid zombie Clients (i.e. queued Clients that will never start their test session). In order to enable this mechanism the Client MUST add the following test to its test suite request:

| **Symbolic name** | **Integer value** | **Description** |
|:------------------|:------------------|:----------------|
| TEST\_STATUS | 16 (1L << 4) | A special flag to notify the Server that this Client will respond to status requests |

All Clients MUST support TEST\_STATUS mechanism.

### Middlebox test ###

The Middlebox test is a short throughput test from the Server to the Client with a limited CWND to check for a duplex mismatch condition. Moreover, this test uses a pre-defined MSS value to check if any intermediate node will modify its connection settings.

#### MID protocol ####

All messages exchanged between the Client and the Server during the MID protocol are sent using the same connection and message format as the NDTP-Control protocol. Only the throughput packets are sent on the new connection and do not follow the NDTP-Control protocol message format.

As a first step the Server MUST bind a new port and send a TEST\_PREPARE message containing this port number to the Client using the NDTP-Control connection:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/18.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/18.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png)

The port number must be encoded as string.

Next, the Client MUST connect to this newly bound port and start reading data from the Server.

When the Client successfully connects, the NDT Server MUST start 5 seconds throughput test using the newly created connection.

The NDT Server in the MID Throughput test MUST send packets through the newly opened connection as fast as possible (i.e. without any delays) for 5 seconds. These packets must be written using the buffer of the following size:
```
(The current maximum segment size (MSS))
```

If for some reasons it is not possible to use such a big buffer, the Server MUST use a 8192 Byte one. The content of the buffer SHOULD be initialized a single time and sent repeatedly. The contents of the buffer SHOULD avoid repeating content (to avoid any automatic compression mechanisms) and MUST include only US-ASCII printable characters. The maximum value of the congestion window during the MID Throughput test MUST be set to the following value:
```
2 * (The current maximum segment size (MSS))
```

When the test is over, the Server MUST send its results encoded as strings in a TEST\_MSG message to the Client using the NDTP-Control connection. These results MUST contain the following variables:

| CurMSS | The current maximum segment size (MSS), in octets.|
|:-------|:--------------------------------------------------|
| WinScaleSent | The value of the transmitted window scale option if one was sent; otherwise, a value of -1. |
| WinScaleRcvd | The value of the received window scale option if one was received; otherwise, a value of -1. |
| SumRTT | The sum of all sampled round trip times |
| CountRTT | The number of round trip time samples |
| MaxRwinRcvd | The maximum window advertisement received, in octets. |

The above variables can be obtained by using a [web100](http://www.web100.org/) project. All available web100 data variables are described in [Appendix A](NDTProtocol#Appendix_A._web100_variables.md).

The whole TEST\_MSG message sent to the Client looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/12.%20TEST_MSG%20message%20to%20Client.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/12.%20TEST_MSG%20message%20to%20Client.png)

where results are encoded using JSON map with following entries:

| **JSON map's key** | **Variable** |
|:-------------------|:-------------|
| ServerAddress | server address |
| ClientAddress | client address visible from server |
| CurMSS | current MSS value |
| WinScaleSent | WinScaleSent |
| WinScaleRcvd | WinScaleRcvd |
| SumRTT | SumRTT |
| CountRTT | CountRTT |
| MaxRwinRcvd | MaxRwinRcvd |

Next, the Client MUST send to the Server its calculated throughput value encoded as string (float format) in a TEST\_MSG message using the NDTP-Control connection. The throughput value MUST be calculated using the following formula:
```
THROUGHPUT_VALUE = 8 * TRANSMITTED_BYTES / 1000 / TEST_DURATION_SECONDS
```

At the end the Server MUST close the MID test session by sending an empty TEST\_FINALIZE message using the NDTP-Control connection.

A Middlebox test session overview can be seen on the following sequence diagram:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/13.%20A%20Middlebox%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/13.%20A%20Middlebox%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png)

### Simple firewall test ###

The Simple firewall test tries to find out any firewalls between the Client and the Server that will prevent connections to an ephemeral port numbers. The test is performed in both directions (i.e. the Client is trying to connect to the Server and the Server is trying to connect to the Client).

#### SFW protocol ####

All messages exchanged between the Client and the Server during the SFW protocol are sent using the same connection and message format as the NDTP-Control protocol. Only a single TEST\_MSG message containing a pre-defined string "Simple firewall test" are sent using the new connection to check for firewall existence.

As a first step the Server MUST bind the ephemeral port and send a TEST\_PREPARE message containing this port number and a test time (in seconds) to the Client using the NDTP-Control connection:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/15.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20and%20test%20time%20%28in%20seconds%29%20to%20the%20Client.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/15.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20and%20test%20time%20%28in%20seconds%29%20to%20the%20Client.png)

The port number and the test time are encoded using JSON format as follows:
| **JSON map's key** | **Variable** |
|:-------------------|:-------------|
| empheralPortNumber | port number |
| testTime | test time |

Next, the Client MUST bind the ephemeral port and send a TEST\_MSG message containing only this port number to the Server using the NDTP-Control connection. The port number MUST be encoded as string.

The NDT Server MUST start the test by sending an empty TEST\_START message (using the NDTP-Control connection) immediately after it receives the Client's ephemeral port number.

The test MUST be performed in both directions in parallel:
  1. The Client MUST try to connect to the Server's ephemeral port and MUST send a TEST\_MSG message containing a pre-defined string "Simple firewall test" of length 20 using the newly created connection.
  1. The Server MUST try to connect to the Client's ephemeral port and MUST send a TEST\_MSG message containing a pre-defined string "Simple firewall test" of length 20 using the newly created connection.

The both messages looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/14.%20of%20length%2020.%20The%20whole%20message%20looks%20as%20following.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/14.%20of%20length%2020.%20The%20whole%20message%20looks%20as%20following.png)

After performing the Simple firewall test in both directions, the Server MUST send to the Client its SFW results encoded as string in the TEST\_MSG message using the NDTP-Control connection:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/16.%20Client%20its%20SFW%20%28c2s%29%20results%20encoded%20as%20string%20in%20the%20TEST_MSG%20message.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/16.%20Client%20its%20SFW%20%28c2s%29%20results%20encoded%20as%20string%20in%20the%20TEST_MSG%20message.png)

The Simple firewall test defines the following result codes:

| **Value** | **Description** |
|:----------|:----------------|
| "0" | Test was not started |
| "1" | Test was successful (i.e. connection to the ephemeral port was possible) |
| "2" | There was a connection to the ephemeral port, but it was not recognized properly |
| "3" | There was no connection to the ephemeral port within the specified time |

At the end the Server MUST close the SFW test session by sending an empty TEST\_FINALIZE message using the NDTP-Control connection.

A Simple firewall test session overview can be seen on the following sequence diagram:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/17.%20A%20Simple%20firewall%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/17.%20A%20Simple%20firewall%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png)

### C2S throughput test ###

The C2S throughput test tests the achievable network bandwidth from the Client to the Server by performing a 10 seconds memory-to-memory data transfer.

#### C2S protocol ####

All messages exchanged between the Client and the Server during the C2S protocol are sent using the same connection and message format as the NDTP-Control protocol. Only the throughput packets are sent on the new connection and do not follow the NDTP-Control protocol message format.

As a first step the Server MUST bind the new port and send a TEST\_PREPARE message containing this port number to the Client using the NDTP-Control connection:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/18.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/18.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png)

The port number must be encoded as string.

Next, the Client MUST connect to this newly bound port.

In order to start the test, the Server MUST send an empty TEST\_START message using the NDTP-Control connection.

The Client MUST start 10 seconds throughput test using the newly created connection and the Server MUST read the transmitted data.

The NDT Client in the C2S Throughput test MUST use a 8192 Byte buffer to send the packets through the newly opened connection as fast as possible (i.e. without any delays) for 10 seconds. The content of the buffer SHOULD be initialized a single time and sent repeatedly. The contents of the buffer SHOULD avoid repeating content (to avoid any automatic compression mechanisms) and MUST include only US-ASCII printable characters.

When the Client stops streaming the test data (or the test routine at the Server times out after 11 seconds from the last TEST\_START message), the Server MUST send to the Client its calculated throughput value encoded as string (float format) in the TEST\_MSG message using the NDTP-Control connection. The throughput value MUST be calculated using the following formula:
```
THROUGHPUT_VALUE = 8 * TRANSMITTED_BYTES / 1000 / TEST_DURATION_SECONDS
```

The Server's TEST\_MSG message looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/20.%20Client%20its%20calculated%20throughput%20value%20encoded%20as%20string%20%28float%20format%29%20in%20the%20TEST_MSG%20message.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/20.%20Client%20its%20calculated%20throughput%20value%20encoded%20as%20string%20%28float%20format%29%20in%20the%20TEST_MSG%20message.png)

At the end the Server MUST close the C2S test session by sending an empty TEST\_FINALIZE message using the NDTP-Control connection.

A C2S throughput test session overview can be seen on the following sequence diagram:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/21.%20A%20C2S%20throughput%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/21.%20A%20C2S%20throughput%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png)

### S2C throughput test ###

The S2C throughput test tests the achievable network bandwidth from the Server to the Client by performing a 10 seconds memory-to-memory data transfer.

The NDT Server uses a [web100](http://www.web100.org/) project to perform a detailed connection measurements. This means that the Server is also collecting the web100 data variables, that MUST be send to the Client at the end of the whole test session. All available web100 data variables are described in [Appendix A](NDTProtocol#Appendix_A._web100_variables.md).

#### S2C protocol ####

All messages exchanged between the Client and the Server during the S2C protocol are sent using the same connection and message format as the NDTP-Control protocol. Only the throughput packets are sent on the new connection and do not follow the NDTP-Control protocol message format.

As a first step the Server MUST bind the new port. Next, it MUST send a TEST\_PREPARE message containing this port number to the Client using the NDTP-Control connection:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/22.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/22.%20TEST_PREPARE%20message%20containing%20this%20port%20number%20to%20the%20Client.png)

The port number must be encoded as string.

Next, the Client MUST connect to this newly bound port.

In order to start the test, the Server MUST send an empty TEST\_START message using the NDTP-Control connection.

The Server MUST start 10 seconds throughput test using the newly created connection and the Client MUST read the transmitted data.

The NDT Server in the S2C Throughput test MUST use a 8192 Byte buffer to send the packets through the newly opened connection as fast as possible (i.e. without any delays) for 10 seconds. The content of the buffer SHOULD be initialized a single time and sent repeatedly. The contents of the buffer SHOULD avoid repeating content (to avoid any automatic compression mechanisms) and MUST include only US-ASCII printable characters.

When the Server stops streaming the test data, it MUST send to the Client its calculated throughput value (encoded as string in float format), amount of unsent data in the socket send queue (encoded as string in integer format) and overall number of sent bytes (encoded as string in float format) in the TEST\_MSG message using the NDTP-Control connection. The throughput value MUST be calculated using the following formula:
```
THROUGHPUT_VALUE = 8 * SENT_BYTES / 1000 / TEST_DURATION_SECONDS
```

The Server's TEST\_MSG message looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/23.%20overall%20number%20of%20sent%20bytes%20%28encoded%20as%20string%20in%20float%20format%29%20in%20the%20TEST_MSG%20message.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/23.%20overall%20number%20of%20sent%20bytes%20%28encoded%20as%20string%20in%20float%20format%29%20in%20the%20TEST_MSG%20message.png)

where results are encoded using JSON map with following entries:

| **JSON map's key** | **Variable** |
|:-------------------|:-------------|
| ThroughputValue | throughput calculated by server |
| UnsentDataAmount | unsent byte count |
| TotalSentByte | total sent byte count to client |

Next, the Client MUST send to the Server its calculated throughput value encoded as string (float format) in the TEST\_MSG message using the NDTP-Control connection. The throughput value MUST be calculated using the following formula:
```
THROUGHPUT_VALUE = 8 * TRANSMITTED_BYTES / 1000 / TEST_DURATION_SECONDS
```

The Client's TEST\_MSG message looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/24.%20MUST%20send%20to%20the%20Server%20its%20calculated%20throughput%20value%20encoded%20as%20string%20%28float%20format%29%20in%20the%20TEST_MSG%20message.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/24.%20MUST%20send%20to%20the%20Server%20its%20calculated%20throughput%20value%20encoded%20as%20string%20%28float%20format%29%20in%20the%20TEST_MSG%20message.png)

Following the throughput value, the Server SHOULD send its [web100 data variables](NDTProtocol#Appendix_A._web100_variables.md) gathered during S2C throughput test to the Client using the NDTP-Control connection.

Each name/value pair MUST be encoded as a US-ASCII string in individual and separate TEST\_MSG messages, each using the following format:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/ndt_24b.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/ndt_24b.png)

where '\n' is a single new line character.

All available web100 data variables are described in [Appendix A](NDTProtocol#Appendix_A._web100_variables.md). The Server MUST send at least the following variables:
  1. AckPktsIn
  1. CountRTT
  1. CongestionSignals
  1. CurRTO
  1. CurMSS
  1. DataBytesOut
  1. DupAcksIn
  1. MaxCwnd
  1. MaxRwinRcvd
  1. PktsOut
  1. PktsRetrans
  1. RcvWinScale
  1. Sndbuf
  1. SndLimTimeCwnd
  1. SndLimTimeRwin
  1. SndLimTimeSender
  1. SndWinScale
  1. SumRTT
  1. Timeouts


At the end the Server MUST close the S2C test session by sending an empty TEST\_FINALIZE message using the NDTP-Control connection.

A S2C throughput test session overview can be seen on the following sequence diagram:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/25.%20A%20S2C%20throughput%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/25.%20A%20S2C%20throughput%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png)

### META test ###

The META test allows the Client to send an additional information to the Server to be included along with the overall set of test results. Information received from the Client is encoded as key/value pairs.

#### META protocol ####

All messages exchanged between the Client and the Server during the META protocol are sent using the same connection and message format as the NDTP-Control protocol.

The Server MUST start this test by sending an empty TEST\_PREPARE message followed by an empty TEST\_START message.

Next, the Client SHOULD send as many name/value meta data pairs encoded as a US-ASCII string in individual and separate TEST\_MSG messages, each using the following format:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/26.%20Next,%20the%20Client%20SHOULD%20send%20meta%20data%20encoded%20as%20strings%20in%20the%20TEST_MSG%20messages.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/26.%20Next,%20the%20Client%20SHOULD%20send%20meta%20data%20encoded%20as%20strings%20in%20the%20TEST_MSG%20messages.png)

For example:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/27.%20For%20example.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/27.%20For%20example.png)

Both parts (keys and values) of the meta data MUST be strings with the following constraints:
  * Keys MUST be shorter than 64 characters
  * Values MUST be shorter than 256 characters

Although this protocol is generic enough to store any type of information (encoded as strings), it is RECOMMENDED to store at least the following data (the Server MUST properly recognize these keys):

| **Key name** | **Description** |
|:-------------|:----------------|
| "client.os.name" | Operating system name in a short format (for example "Linux", "Windows NT", "Mac OS", etc.) |
| "client.browser.name" | Web browser name with a version number (for example "MSIE 7.0", "Firefox/3.6.18", etc.) |
| "client.kernel.version" | Operating system kernel version (for example "2.6.35-30-server") |
| "client.version" | NDT client version (for example "3.6.4") |

The Client MUST finish uploading its meta data by sending an empty TEST\_MSG message.

At the end the Server MUST close the META test session by sending an empty TEST\_FINALIZE message.

A META test session overview can be seen on the following sequence diagram:

![http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/28.%20A%20META%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png](http://ndt.googlecode.com/svn/wiki/NDTProtocol.images/28.%20A%20META%20test%20session%20overview%20can%20be%20seen%20on%20the%20following%20sequence%20diagram.png)

# Appendix A. web100 variables #

| AbruptTimeouts | The number of timeouts that occurred without any immediately preceding duplicate acknowledgments or other indications of congestion. Abrupt Timeouts indicate that the path lost an entire window of data or acknowledgments. Timeouts that are preceded by duplicate acknowledgments or other congestion signals (e.g., ECN) are not counted as abrupt, and might have been avoided by a more sophisticated Fast Retransmit algorithm. |
|:---------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| CurMSS | The current maximum segment size (MSS), in octets.|
| X\_Rcvbuf | The socket receive buffer size in octets.  Note that the meaning of this variable is implementation dependent.  In particular, it may or may not include the reassembly queue. |
| X\_Sndbuf | The socket send buffer size in octets.  Note that the meaning of this variable is implementation dependent. Particularly, it may or may not include the retransmit queue. |
| AckPktsIn | The number of valid pure ack packets that have been received on this connection by the Local Host. |
| AckPktsOut | The number of pure ack packets that have been sent on this connection by the Local Host. |
| BytesRetrans | The number of octets retransmitted. |
| CongAvoid | The number of times the congestion window has been increased by the Congestion Avoidance algorithm. |
| CongestionOverCount | The number of congestion events which were 'backed out' of the congestion control state machine such that the congestion window was restored to a prior value. This can happen due to the Eifel algorithm [RFC3522](http://www.ietf.org/rfc/rfc3522.txt) or other algorithms which can be used to detect and cancel spurious invocations of the Fast Retransmit Algorithm. |
| CongestionSignals | The number of multiplicative downward congestion window adjustments due to all forms of congestion signals, including Fast Retransmit, ECN and timeouts. This object summarizes all events that invoke the MD portion of AIMD congestion control, and as such is the best indicator of how cwnd is being affected by congestion. |
| CountRTT | The number of round trip time samples included in tcpEStatsPathSumRTT and tcpEStatsPathHCSumRTT. |
| CurCwnd | The current congestion window, in octets. |
| CurRTO | The current value of the retransmit timer RTO. |
| CurRwinRcvd | The most recent window advertisement received, in octets. |
| CurRwinSent | The most recent window advertisement sent, in octets. |
| CurSsthresh | The current slow start threshold in octets. |
| CurTimeoutCount | The current number of times the retransmit timeout has expired without receiving an acknowledgment for new data. tcpEStatsStackCurTimeoutCount is reset to zero when new data is acknowledged and incremented for each invocation of section 5.5 in [RFC2988](http://www.ietf.org/rfc/rfc2988.txt). |
| DSACKDups | The number of duplicate segments reported to the local host by D-SACK blocks. |
| DataBytesIn | The number of octets contained in received data segments, including retransmitted data.  Note that this does not include TCP headers. |
| DataBytesOut | The number of octets of data contained in transmitted segments, including retransmitted data.  Note that this does not include TCP headers. |
| DataPktsIn | The number of segments received containing a positive length data segment. |
| DataPktsOut | The number of segments sent containing a positive length data segment. |
| DupAcksIn | The number of duplicate ACKs received. |
| ECNEnabled | Enabled(1) if Explicit Congestion Notification (ECN) has been negotiated on, selfDisabled(2) if it is disabled or not implemented on the local host, or peerDisabled(3) if not negotiated by the remote hosts. |
| FastRetran | The number of invocations of the Fast Retransmit algorithm. |
| MaxCwnd | The maximum congestion window used during Slow Start, in octets. |
| MaxMSS | The maximum MSS, in octets. |
| MaxRTO | The maximum value of the retransmit timer RTO. |
| MaxRTT | The maximum sampled round trip time. |
| MaxRwinRcvd | The maximum window advertisement received, in octets. |
| MaxRwinSent | The maximum window advertisement sent, in octets. |
| MaxSsthresh | The maximum slow start threshold, excluding the initial value. |
| MinMSS | The minimum MSS, in octets. |
| MinRTO | The minimum value of the retransmit timer RTO. |
| MinRTT | The minimum sampled round trip time. |
| MinRwinRcvd | The minimum window advertisement received, in octets. |
| MinRwinSent | The minimum window advertisement sent, excluding the initial unscaled window advertised on the SYN, in octets. |
| NagleEnabled | True(1) if the Nagle algorithm is being used, else false(2). |
| OtherReductions | The number of congestion window reductions made as a result of anything other than AIMD congestion control algorithms. Examples of non-multiplicative window reductions include Congestion Window Validation [RFC2861](http://www.ietf.org/rfc/rfc2861.txt) and experimental algorithms such as Vegas. |
| PktsIn | The total number of segments received. |
| PktsOut | The total number of segments sent. |
| PktsRetrans | The number of segments transmitted containing at least some retransmitted data. |
| RcvWinScale | The value of Rcv.Wind.Scale.  Note that RcvWinScale is either zero or the same as WinScaleSent. |
| SACKEnabled | True(1) if SACK has been negotiated on, else false(2). |
| SACKsRcvd | The number of SACK options received. |
| SendStall | The number of interface stalls or other sender local resource limitations that are treated as congestion signals. |
| SlowStart | The number of times the congestion window has been increased by the Slow Start algorithm. |
| SampleRTT | The most recent raw round trip time measurement used in calculation of the RTO. |
| SmoothedRTT | The smoothed round trip time used in calculation of the RTO. See SRTT in [RFC2988](http://www.ietf.org/rfc/rfc2988.txt). |
| SndWinScale | The value of Snd.Wind.Scale.  Note that SndWinScale is either zero or the same as WinScaleRcvd. |
| SndLimTimeRwin | The cumulative time spent in the 'Receiver Limited' state. |
| SndLimTimeCwnd | The cumulative time spent in the 'Congestion Limited' state. |
| SndLimTimeSender | The cumulative time spent in the 'Sender Limited' state. |
| SndLimTransRwin | The number of transitions into the 'Receiver Limited' state from either the 'Congestion Limited' or 'Sender Limited' states. This state is entered whenever TCP transmission stops because the sender has filled the announced receiver window. |
| SndLimTransCwnd | The number of transitions into the 'Congestion Limited' state from either the 'Receiver Limited' or 'Sender Limited' states. This state is entered whenever TCP transmission stops because the sender has reached some limit defined by congestion control (e.g. cwnd) or other algorithms (retransmission timeouts) designed to control network traffic. |
| SndLimTransSender | The number of transitions into the 'Sender Limited' state from either the 'Receiver Limited' or 'Congestion Limited' states. This state is entered whenever TCP transmission stops due to some sender limit such as running out of application data or other resources and the Karn algorithm. When TCP stops sending data for any reason which can not be classified as Receiver Limited or Congestion Limited it MUST be treated as Sender Limited. |
| SndLimBytesRwin | The cumulative bytes sent while in the 'Receiver Limited' state, as determined by the Local Host, when the Local Host is a sender.  This state is entered whenever TCP transmission stops due to Receiver not being able to receive data. |
| SndLimBytesCwnd |The cumulative bytes sent while in the 'Congestion Limited' state, as determined by the Local Host, when the Local Host is a sender.  This state is entered whenever TCP transmission stops due to congestion. |
| SndLimBytesSender | The cumulative bytes sent while in the 'Sender Limited' state, as determined by the Local Host, when the Local Host is a sender.  This state is entered whenever TCP transmission stops because there is no more data in sender's buffer. |
| SubsequentTimeouts | The number of times the retransmit timeout has expired after the RTO has been doubled. See section 5.5 in [RFC2988](http://www.ietf.org/rfc/rfc2988.txt). |
| SumRTT | The sum of all sampled round trip times. |
| ThruBytesAcked | The number of octets for which cumulative acknowledgments have been received, on systems that can receive more than 10 million bits per second.  Note that this will be the sum of changes in tcpEStatsAppSndUna. |
| Timeouts | The number of times the retransmit timeout has expired when the RTO backoff multiplier is equal to one. |
| TimestampsEnabled | Enabled(1) if TCP timestamps have been negotiated on, selfDisabled(2) if they are disabled or not implemented on the local host, or peerDisabled(3) if not negotiated by the remote hosts. |
| WinScaleRcvd | The value of the received window scale option if one was received; otherwise, a value of -1. |
| WinScaleSent | The value of the transmitted window scale option if one was sent; otherwise, a value of -1. |
| DupAcksOut | The number of duplicate ACKs sent. |
| StartTimeUsec | The value of sysUpTime at the time this listener was established.  If the current state was entered prior to the last re-initialization of the local network management subsystem, then this object contains a zero value. |
| Duration | The seconds part of the time elapsed between StartTimeStamp and the most recent protocol event (segment sent or received). |