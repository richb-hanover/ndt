# NDT Test Methodology #

## Abstract ##

The Network Diagnostic Tool (NDT) is a client/server program that provides network configuration and performance testing to a user's computer. NDT is designed to identify both performance problems and configuration problems. Performance problems affect the user experience, usually causing data transfers to take longer than expected. These problems are usually solved by tuning various TCP (Transmission Control Protocol) network parameters on the end host. Configuration problems also affect the user experience; however, tuning will not improve the end-to-end performance. The configuration fault must be found and corrected to change the end host behavior. NDT is providing enough information to accomplish these tasks. This document describes how these information is gathered and what NDT is and is not capable of answering.

## Table of Contents ##



## Introduction ##

NDT is a typical memory to memory client/server test device. Throughput measurements closely measure the network performance, ignoring disk I/O effects. The real strength is in the advanced diagnostic features that are enabled by the kernel data automatically collected by the web100 monitoring infrastructure.  This data is collected during the test and analyzed after the test completes to determine what, if anything, impacted the test. One of the MAJOR issues facing commodity Internet users is the performance limiting host configuration settings for the Windows XP operating system. To illustrate this, a cable modem user with basic service (15 Mbps download) would MAX out at 13 Mbps with a 40 msec RTT delay. Thus unless the ISP proxies content, the majority of traffic will be limited by the clients configuration and NOT the ISP's infrastructure.  The NDT server can detect and report this problem, saving consumers and ISP's dollars by allowing them to quickly identify where to start looking for a problem.

NDT operates on any client with a Java-enabled Web browser; further:
  * What it can do:
    * Positively state if Sender, Receiver, or Network is operating properly
    * Provide accurate application tuning info
    * Suggest changes to improve performance
  * What it can’t do:
    * Tell you where in the network the problem is
    * Tell you how other servers perform
    * Tell you how other clients will perform

## Document Definitions ##

|Throughput|In this document, the term "Throughput" refers to Application-Level Throughput, the amount of data received by the application from the TCP connection over the duration of the TCP connection. |
|:---------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|Total Send Throughput|The term "Total Send Throughput" refers to outgoing, TCP-level data throughput. This includes the all the data octets sent in TCP packets, including retransmits, over the duration of the TCP connection. |
|Theoretical Maximum Throughput|The term "Theoretical Maximum Throughput" refers to the maximum throughput of the link according to the [Matthis equation](http://www.psc.edu/networking/papers/model_ccr97.ps). |

## Performed tests ##

### Middlebox Test ###

The middlebox test is a short throughput test from the server to the client with a limited Congestion Window ([congestion window](http://en.wikipedia.org/wiki/Congestion_window) - one of the factors that determines the number of bytes that can be outstanding at any time) to check for a duplex mismatch condition. Moreover, this test uses a pre-defined Maximum Segment Size (MSS) to check if any intermediate node is modifying the connection settings.

A detailed description of all of the Middlebox protocol messages can be found in the [NDT Protocol document](NDTProtocol#Middlebox_test.md).

The general test workflow is as follows:

  1. The server selects a random port and opens a socket on that port
  1. The server sends this port number to the client
  1. The server sets the MSS on this port to 1456
  1. The client creates a connection to the server's port
  1. The server sets the congestion window of the connection to be `2 * (The current MSS)`
> > By setting this congestion window setting the code limits TCP to sending only 2 packets per RTT. This mechanism is a part of the duplex mismatch detection. The idea for this came from Matt Mathis, and the NPAD duplex mismatch detection. If the server sends a maximum of 2 packets then it will never trigger a mismatch condition. Later when the Server-To-Client test is run a duplex mismatch will cause the Middlebox test to be faster than the Server-To-Client test.
  1. The server performs a 5 second throughput test over the connection
  1. The server can temporarily stop sending packets when the following formula is fulfilled:
```
    BUFFER_SIZE * 16 < ((Next Sequence Number To Be Sent) - (Oldest Unacknowledged Sequence Number) - 1)
```
> > The both `"Next Sequence Number To Be Sent"` and `"Oldest Unacknowledged Sequence Number"` values are obtained from the connection with the help of the [web100](http://www.web100.org/) library. This code is trying to keep the server from filling up the TCP transmit queue.
  1. After the throughput test, the server sends the following results to the client:
> > | CurMSS | The current maximum segment size (MSS), in octets.|
|:-------|:--------------------------------------------------|
> > | WinScaleSent | The value of the transmitted window scale option if one was sent; otherwise, a value of -1. |
> > | WinScaleRcvd | The value of the received window scale option if one was received; otherwise, a value of -1. |
> > | SumRTT | The sum of all sampled round trip times |
> > | CountRTT | The number of round trip time samples |
> > | MaxRwinRcvd | The maximum window advertisement received, in octets. |
  1. After the client has received the results, it sends its calculated throughput value to the server. The throughput value is calculated by taking the received bytes over the duration of the test. This value, in Bps, is then converted to kbps. This can be shown by the following formula:
```
    THROUGHPUT_VALUE = (RECEIVED_BYTES / TEST_DURATION_SECONDS) * 8 / 1000
```

#### Known Issues (Middlebox Test) ####

The middlebox test's use of sequence numbers assumes that TCP Reno is being used.

### Simple Firewall Test ###

The simple firewall test tries to find out any firewalls between the NDT client and the NDT server that will prevent connections to an ephemeral port numbers. The test is performed in both directions (i.e. the NDT client is trying to connect to the NDT server and the NDT server is trying to connect to the NDT client).

A detailed description of all of the SFW protocol messages can be found in the [NDT Protocol document](NDTProtocol#Simple_firewall_test.md).

The general test workflow is as follows:

  1. The server selects a random port and opens a socket on that port
  1. The server sends this port number to the client
  1. The client selects a random port and opens a socket on that port
  1. The client sends this port number to the server
  1. In parallel, the client and server tries to connect to the other component's ports
  1. When the client or server connects to the other component, it sends a TEST\_MSG message containing  a pre-defined string "Simple firewall test" over the new connection.
  1. If, after waiting for 3 seconds (or 1 second if MaxRTT or MaxRTO is less than 3 seconds), the client or server closes the port
  1. Once the server has finished connecting to the client and waiting for the client to connect, it sends its results to the client with one of the following values:
| **Value** | **Description** |
|:----------|:----------------|
| "0" | The test was not started/results were not received (this means an error condition like the client sending the wrong control messages, or an error on the server like an Out-Of-Memory error that prevented it from running the test) |
| "1" | The test was successful (i.e. connection to the random port was possible and the pre-defined string was received) |
| "2" | There was a connection to the random port, but the pre-defined string was not received |
| "3" | There was no connection to the random port within the specified time |

#### Known Issues (Simple Firewall Test) ####

The client does not send its results to the server which means the server is not sure whether or not it was able to properly connect to the client.

### Client-To-Server Throughput Test ###

The Client-To-Server throughput test measures the throughput from the client to the server by performing a 10 seconds memory-to-memory data transfer.

A detailed description of all of the Client-To-Server protocol messages can be found in the [NDT Protocol document](NDTProtocol#C2S_throughput_test.md).

The general test workflow is as follows:

  1. The server selects a random port and opens a socket on that port
  1. The server sends this port number to the client
  1. The client connects to the port the server opened
  1. The server starts one or more of the following routines:
    * libpcap routine to perform packet trace used by the [Bottleneck Link Detection](NDTTestMethodology#Bottleneck_Link_Detection.md) algorithm.
    * [tcpdump trace](NDTDataFormat#tcpdump_trace.md) to save to a standard tcpdump file all packets sent during the Client-To-Server throughput test on the newly created connection. The packets for the tcpdump trace are saved by the same libpcap routine as used for the [Bottleneck Link Detection](NDTTestMethodology#Bottleneck_Link_Detection.md) algorithm. This tcpdump trace dump is only started when the `-t, --tcpdump` options are set by the NDT administrator.
    * [web100 snaplog trace](NDTDataFormat#web100_snaplog_trace.md) to dump web100 kernel MIB variables' values written in a fixed time interval (default is 5 msec) during the Client-To-Server throughput test for the newly created connection. This snaplog trace dump is only started when the `--snaplog` option is set by the NDT administrator.
  1. The client performs a 10 second throughput test over the newly created connection
  1. The server calculates its throughput, in Kbps, according to the following formula:
```
  THROUGHPUT_VALUE = (RECEIVED_BYTES / TEST_DURATION_SECONDS) * 8 / 1000
```
  1. The server sends the calculated throughput value to the client

#### Known Limitations (Client-To-Server Throughput Test) ####

A 10 second test may not be enough time for TCP to reach a steady-state on a high bandwidth, high latency link.

### Server-To-Client Throughput Test ###

The Server-To-Client throughput test measures the throughput from the server to the client by performing a 10 seconds memory-to-memory data transfer.

A detailed description of all of the Server-To-Client protocol messages can be found in the [NDT Protocol document](NDTProtocol#S2C_throughput_test.md).

The general test workflow is as follows:

  1. The server selects a random port and opens a socket on that port
  1. The server sends this port number to the client
  1. The client connects to the port the server opened
  1. The server starts one or more of the following routines:
    * libpcap routine to perform packet trace used by the [Bottleneck Link Detection](NDTTestMethodology#Bottleneck_Link_Detection.md) algorithm.
    * [tcpdump trace](NDTDataFormat#tcpdump_trace.md) to save to a standard tcpdump file all packets sent during the Server-To-Client throughput test on the newly created connection. The packets for the tcpdump trace are saved by the same libpcap routine as used for the [Bottleneck Link Detection](NDTTestMethodology#Bottleneck_Link_Detection.md) algorithm. This tcpdump trace dump is only started when the `-t, --tcpdump` options are set by the NDT administrator.
    * [web100 snaplog trace](NDTDataFormat#web100_snaplog_trace.md) to dump web100 kernel MIB variables' values written in a fixed time interval (default is 5 msec) during the Server-To-Client throughput test for the newly created connection. This snaplog trace dump is only started when the `--snaplog` option is set by the NDT administrator.
  1. The client performs a 10 second throughput test over the newly created connection
  1. The server takes a web100 snapshot
  1. The server calculates its throughput, in Kbps, according to the following formula:
```
  THROUGHPUT_VALUE = (BYTES_SENT_TO_SEND_SYSCALL / TEST_DURATION_SECONDS) * 8 / 1000
```
  1. The server sends to the client its calculated throughput value, the amount of unsent data in the socket send queue and the total number of bytes the application sent to the send syscall
  1. The server sends to the client all the web100 variables it collected in the final snapshot

#### Known Limitations (Server-To-Client Throughput Test) ####

A 10 second test may not be enough time for TCP to reach a steady-state on a high bandwidth, high latency link. However, the increased information that the web100 variables provide could be used to answer this question. Thus the NDT server could detect and report if something was preventing the system from fully utilising the link. For example tracking how long slow start ran and what the maximum CWND value was can tell us if the connection was network limited or configuration limited.  On a transatlantic 10 Gbps link slow start should run for about 3 seconds and CWND should indicate the speed peaked at 10 Gbps.

## Specific Detection Algorithms/Heuristics ##

Most of the following detection algorithms and heuristics use data obtained during the [Server-To-Client throughput test](NDTTestMethodology#Server-To-Client_Throughput_Test.md). This means, that the NDT server is the sender and the client is the receiver during all these heuristics.

The [Bottleneck Link Detection](NDTTestMethodology#Bottleneck_Link_Detection.md) algorithm uses data collected during both the Client-To-Server and the Server-To-Client throughput tests.

The [Firewall Detection](NDTTestMethodology#Firewall_Detection.md) heuristic uses data collected during the Simple Firewall Test.

The [NAT Detection](NDTTestMethodology#NAT_Detection.md) and [MSS Modification Detection](NDTTestMethodology#MSS_Modification_Detection.md) heuristics use data collected during the Middlebox Test.

These detection algorithms and heuristics were developed based on an analytical model of the TCP connection, and were tuned during tests performed in real LAN, MAN and WAN environments.

### Bottleneck Link Detection ###

NDT attempts to detect the link in the end-to-end path with the smallest capacity (i.e. the narrowest link) using the following methodology.

The way NDT handles sends, there is no application-induced delay between successive packets being sent, so any delays between packets are introduced in-transit. NDT uses the inter-packet delay and the size of the packet as a metric to gauge what the narrowest link in the path is. It does this by calculating the inter-packet throughput which, on average, should correspond to the bandwidth of the lowest-speed link.

The algorithm NDT uses to calculate the narrowest link is as follows:
  1. NDT records the arrival time of each packet using the libpcap routine
  1. NDT calculates the inter-packet throughput by dividing the packet's size, in bits, by the difference between the time that it arrived and the time the previous packet arrived
  1. NDT quantizes the throughput into one of a group of pre-defined bins (described below), incrementing the counter for that bin
  1. Once the test is complete, NDT determines the link speed according to the bin with the largest counter value

The bins are defined in mbits/second:

  * 0 < inter-packet throughput (mbits/second) <= 0.01 - **RTT**
  * 0.01 < inter-packet throughput (mbits/second) <= 0.064 - **Dial-up Modem**
  * 0.064 < inter-packet throughput (mbits/second) <= 1.5 - **Cable/DSL modem**
  * 1.5 < inter-packet throughput (mbits/second) <= 10 - **10 Mbps Ethernet or WiFi 11b subnet**
  * 10 < inter-packet throughput (mbits/second) <= 40 - **45 Mbps T3/DS3 or WiFi 11 a/g subnet**
  * 40 < inter-packet throughput (mbits/second) <= 100 - **100 Mbps Fast Ethernet subnet**
  * 100 < inter-packet throughput (mbits/second) <= 622 - **a 622 Mbps OC-12 subnet**
  * 622 < inter-packet throughput (mbits/second) <= 1000 - **1.0 Gbps Gigabit Ethernet subnet**
  * 1000 < inter-packet throughput (mbits/second) <= 2400 - **2.4 Gbps OC-48 subnet**
  * 2400 < inter-packet throughput (mbits/second) <= 10000 - **10 Gbps 10 Gigabit Ethernet/OC-192 subnet**
  * bits cannot be determined - **Retransmissions** (this bin counts the duplicated or invalid packets and does not denote a real link type)
  * otherwise - ?

#### Known Limitations (Bottleneck Link Detection) ####

The Bottleneck Link Detection assumes that packet coalescing is disabled.

Networking, especially DSL/Cable and WiFi, have become notedly faster than when the bin boundaries were first defined.

The results are quantized, meaning that the NDT doesn’t recognize fractional link speed (Ethernet, T3, or FastE). It also wouldn’t detect bonded Etherchannel interfaces.

### Duplex Mismatch Detection ###

Duplex mismatch is a condition whereby the host Network Interface Card (NIC) and building switch port fail to agree on whether to operate at 'half-duplex' or 'full-duplex'. While this failure will have a large impact on application performance, basic network connectivity still exists. This means that normal testing procedures (e.g., ping, traceroute) may report that no problem exists while real applications will run extremely slowly.

NDT contains two heuristics for the duplex mismatch detection. This heuristic was determined by looking at the web100 variables and determining which variables best indicated faulty hardware. The first heuristic detects whether or not the desktop client link has a duplex mismatch condition. The second heuristic is used to discover if an internal network link has a duplex mismatch condition.

The client link duplex mismatch detection uses the following heuristic.

  * The connection spends over 90% of its time in the congestion window limited state.
  * The Theoretical Maximum Throughput over this link is less than 2 Mbps.
  * There are more than 2 packets being retransmitted every second of the test.
  * The connection experienced a transition into the TCP slow-start state.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The ['Congestion Limited' state time share](NDTTestMethodology#'Congestion_Limited'_state_time_share.md) **is greater than 90%**
  * The [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) **is greater than 2Mibps**
  * The number of segments transmitted containing at least some retransmitted data **is greater than 2 per second**
  * The maximum slow start threshold, excluding the initial value, **is greater than 0**
  * The cumulative time of the expired retransmit timeouts RTO **is greater than 1% of the total test time**
  * The link type detected by the [Link Type Detection Heuristics](NDTTestMethodology#Link_Type_Detection_Heuristics.md) is not a wireless link
  * The throughput measured during the Middlebox test (with a limited CWND) **is greater than** the throughput measured during the Server-To-Client test
  * The throughput measured during the Client-To-Server test **is greater than** the throughput measured during the Server-To-Client test

The internal network link duplex mismatch detect uses the following heuristic.

  * The measured client to server throughput rate exceeded 50 Mbps.
  * The measured server to client throughput rate is less than 5 Mbps.
  * The connection spent more than 90% of the time in the receiver window limited state.
  * There is less that 1% packet loss over the life of the connection.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The throughput measured during the Server-To-Client test **is greater than 50 Mbps**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is less than 5 Mbps**
  * The ['Receiver Limited' state time share](NDTTestMethodology#'Receiver_Limited'_state_time_share.md) **is greater than 90%**
  * The [packet loss](NDTTestMethodology#Packet_loss.md) **is less than 1%**

#### Known Issues/limitations (Duplex Mismatch Detection) ####

The client link duplex mismatch heuristic does not work with multiple simultaneous tests. In order to enable this heuristic, the multi-test mode must be disabled (so the `-m, --multiple` options cannot be set).

<font color='red'>NDT does not appear to implement the heuristic correctly.</font> The condition "The link type detected by the [Link Type Detection Heuristics](NDTTestMethodology#Link_Type_Detection_Heuristics.md) is not a wireless link" is always fulfilled, because the Duplex Mismatch Detection heuristic is run before the Link Type Detection heuristic. Also, the condition "The Theoretical Maximum Throughput over this link is less than 2 Mbps" does not appear to be handled correctly since the [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) is calculated in Mibps not Mbps, and NDT checks if the [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) is greater than 2, not less than 2.

The difference between the Server-To-Client throughput (> 50 Mbps) and the [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) (< 5 Mbps) is incredibly big, so it looks like a bug in the formula.

### Link Type Detection Heuristics ###

The following link type detection heuristics are run only when there is no duplex mismatch condition detected and the [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) is the same or smaller than the [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) (which is an expected situation).

#### DSL/Cable modem ####

The link is treated as a DSL/Cable modem when the NDT Server isn't a bottleneck and the [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) is less than 2 Mbps and less than the [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md).

NDT implements the above heuristic by checking that the following conditions are all true:

  * The cumulative time spent in the 'Sender Limited' state **is less than 0.6 ms**
  * The number of transitions into the 'Sender Limited' state **is 0**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is less than 2 Mbps**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is less than** [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md)

##### Known Issues (DSL/Cable modem detection heuristic) #####

<font color='red'>The <a href='NDTTestMethodology#DSL/Cable_modem.md'>DSL/Cable modem</a> heuristic appears to be broken now because the DSL/Cable modems commonly go above 2Mbps nowadays.</font>

#### IEEE 802.11 (WiFi) ####

The link is treated as a wireless one when the [DSL/Cable modem](NDTTestMethodology#DSL/Cable_modem.md) is not detected, the NDT Client is a bottleneck and the [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) is less than 5 Mbps but the [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) is greater than 50 Mibps.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The heuristic for DSL/Cable modem link **gives negative results**
  * The cumulative time spent in the 'Sender Limited' state **is 0 ms**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is less than 5 Mbps**
  * The [Theoretical Maximum Throughput](NDTTestMethodology#Theoretical_Maximum_Throughput.md) **is greater than 50 Mibps**
  * The number of transitions into the 'Receiver Limited' state **is the same** as the number of transitions into the 'Congestion Limited' state
  * The ['Receiver Limited' state time share](NDTTestMethodology#'Receiver_Limited'_state_time_share.md) **is greater than 90%**

#### Ethernet link (Fast Ethernet) ####

The link is treated as an Ethernet link (Fast Ethernet) when the WiFi and DSL/Cable modem are not detected, the [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) is between 3 and 9.5 Mbps and the connection is very stable.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The heuristics for WiFi and DSL/Cable modem links **give negative results**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is less than 9.5 Mbps**
  * The [Total Send Throughput](NDTTestMethodology#Total_Send_Throughput.md) **is greater than 3 Mbps**
  * The Server-To-Client throughput test measured **is less than 9.5 Mbps**
  * The [packet loss](NDTTestMethodology#Packet_loss.md) **is less than 1%**
  * The [out of order packets proportion](NDTTestMethodology#Packets_arriving_out_of_order.md) **is less than 35%**

### Faulty Hardware Link Detection ###
NDT uses the following heuristic to determine whether or not faulty hardware, like a bad cable, is impacting performance. This heuristic was determined by looking at the web100 variables and determining which variables best indicated faulty hardware.

  * The connection is losing more than 15 packets per second.
  * The connection spends over 60% of the time in the congestion window limited state.
  * The packet loss rate is less than 1% of the packets transmitted. While the connection is losing a large number of packets per second (test 1) the total number of packets transferred during the test is extremely small so the percentage of retransmitted packets is also small value of packet loss rate.
  * The connection entered the TCP slow-start state.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The [packet loss](NDTTestMethodology#Packet_loss.md) multiplied by 100 and divided by the [total test time](NDTTestMethodology#Total_test_time.md) in seconds **is greater than 15**
  * The ['Congestion Limited' state time share](NDTTestMethodology#'Congestion_Limited'_state_time_share.md) divided by the [total test time](NDTTestMethodology#Total_test_time.md) in seconds **is greater than 0.6**
  * The [packet loss](NDTTestMethodology#Packet_loss.md) **is less than 1%**
  * The maximum slow start threshold, excluding the initial value, **is greater than 0**

#### Known Issues (Faulty Hardware Link Detection) ####

<font color='red'>NDT does not appear to implement the heuristic correctly.</font> Instead of taking the total number of lost packets, and dividing by the test duration to calculate the packet per second loss rate, the loss rate is multiplied times 100. Since the "The [packet loss](NDTTestMethodology#Packet_loss.md)" is less than 1%, then the packet loss multiplied by 100 and divided by the total test time in seconds is less than 1. Moreover, the ['Congestion Limited' state time share](NDTTestMethodology#'Congestion_Limited'_state_time_share.md) should **not** be divided by the [total test time](NDTTestMethodology#Total_test_time.md) in seconds (it should be directly compared to 0.6).

### Full/Half Link Duplex Setting ###

NDT has a heuristic to detect a half-duplex link in the path. This heuristic was determined by looking at the web100 variables and determining which variables best indicated a half-duplex link.

NDT looks for a connection that toggles rapidly between the sender buffer limited and receiver buffer limited states. However, even though the connection toggles into and out of the sender buffer limited state numerous times, it does not remain in this state for long periods of time as over 95% of the time is spent in the receiver buffer limited state

NDT implements the above heuristic by checking that the following conditions are all true:

  * The ['Receiver Limited' state time share](NDTTestMethodology#'Receiver_Limited'_state_time_share.md) **is greater than 95%**
  * The number of transitions into the 'Receiver Limited' state **is greater than 30 per second**
  * The number of transitions into the 'Sender Limited' state **is greater than 30 per second**

### Normal Congestion Detection ###

A normal congestion is detected when the connection is congestion limited a non-trivial percent of the time, there isn't a duplex mismatch detected and the NDT Client's receive window isn't the limiting factor.

NDT implements the above heuristic by checking that the following conditions are all true:

  * The ['Congestion Limited' state time share](NDTTestMethodology#'Congestion_Limited'_state_time_share.md) **is greater than 2%**
  * The duplex mismatch condition heuristic **gives negative results**
  * The maximum window advertisement received **is greater than** the maximum congestion window used during Slow Start

### Firewall Detection ###

A firewall is detected when the connection to the ephemeral port was unsuccessful in the specified time. The results for the server are independent from the results for the client.

Please note, that the NDT states that the node is only **probably** behind a firewall. The connection can be unsuccessful for a variety of other reasons.

Moreover, if there is a connection and the pre-defined string is properly transferred, then there is also only **probably** no firewall on the end-to-end path (technically there still could be a firewall with a range of opened ports or a special rules that allowed this one particular connection to the ephemeral port).

### NAT Detection ###

A Network Address Translation (NAT) box is detected by comparing the client/server IP addresses as seen from the server and the client boxes.

When the server IP address seen by the client is different from the one known to the server itself, a NAT box is modifying the server's IP address.

Similarly, when the client IP address seen by the server is different from the one known to the client itself, a NAT box is modifying the client's IP address.

### MSS Modification Detection ###

NDT checks packet size preservation by comparing the final value of the MSS variable in the Middlebox test (the NDT Server sets the MSS value to 1456 on the listening socket before the NDT Client connects to it; the final value of the MSS variable is read after the NDT Client connects).

When this variable's value is 1456, it means that the packet size is preserved End-to-End. If the MSS variable is not 1456, a network middlebox had to change it during the test.

## Computed variables ##

### Total test time ###

The total test time is the total time used by the Server-To-Client throughput test.

The total test time is computed using the following formula:

```
SndLimTimeRwin + SndLimTimeCwnd + SndLimTimeSender
```

where:
  * **SndLimTimeRwin** - The cumulative time spent in the 'Receiver Limited' state during the Server-To-Client throughput test
  * **SndLimTimeCwnd** - The cumulative time spent in the 'Congestion Limited' state during the Server-To-Client throughput test
  * **SndLimTimeSender** - The cumulative time spent in the 'Sender Limited' state during the Server-To-Client throughput test

The total test time is kept in microseconds.

### Total Send Throughput ###

The Total Send Throughput is the total amount of data (including retransmits) sent by the NDT Server to the NDT Client in the Server-To-Client throughput test.

The Total Send Throughput is computed using the following formula:

```
DataBytesOut / TotalTestTime * 8
```

where:
  * **DataBytesOut** - The number of octets of data contained in transmitted segments, including retransmitted data.
  * **TotalTestTime** - [Total test time](NDTTestMethodology#Total_test_time.md)

The Total Send Throughput is kept in Mbps (because [Total test time](NDTTestMethodology#Total_test_time.md) is kept in microseconds).

### Packet loss ###

The packet loss is the percentage of the lost packets during the Server-To-Client throughput test.

The packet loss proportion is computed using the following formula:

```
CongestionSignals/PktsOut
```

where:
  * **CongestionSignals** - The number of multiplicative downward congestion window adjustments due to all forms of congestion signals (this roughly correspond to a single lost packet)
  * **PktsOut** - The total number of segments sent

To avoid possible division by zero, the NDT sets the packet loss percentages to the following values when the `CongestionSignals` is 0:
  * **0.0000000001** - if a link type detected by the Bottleneck Link Detection algorithm using the **Client --> Server data packets**' inter-packet arrival times is faster than a 100 Mbps Fast Ethernet subnet.
  * **0.000001** - otherwise

### Packets arriving out of order ###

The packets arriving out of order is the percentage of the duplicated packets during the Server-To-Client throughput test.

The out of order packets proportion is computed using the following formula:

```
DupAcksIn/AckPktsIn
```

where:
  * **DupAcksIn** - The number of duplicate ACKs received (this roughly correspond to a single out-of-order packet when the TCP is forcing retries of such packets)
  * **AckPktsIn** - The number of non-duplicate ACKs received

### Average round trip time (Latency/Jitter) ###

The average round trip time is computed using the following formula:

```
SumRTT/CountRTT
```

where:
  * **SumRTT** - The sum of all sampled round trip times
  * **CountRTT** - The number of round trip time samples

The average round trip time is kept in milliseconds.

#### Known Limitations (Average round trip time) ####
The average round trip time is calculated during the Server-To-Client throughput test. Because NDT is attempting to fill the link to discover what throughput it can obtain, the RTT calculations will be skewed by NDT. In this way, NDT's calculation of the RTT is conservative since the actual RTT should be no worse than the RTT when NDT is running the throughput test.

### Theoretical Maximum Throughput ###

The Theoretical Maximum Throughput is computed using the following formula:

```
(CurrentMSS / (AvgRTTSec * sqrt(PktsLoss))) * 8 / 1024 / 1024
```

where:
  * **CurrentMSS** - The current maximum segment size (MSS), in octets
  * **AvgRTTSec** - [Average round trip time (Latency/Jitter)](NDTTestMethodology#Average_round_trip_time_(Latency/Jitter).md) in seconds
  * **PktsLoss** - [Packet loss](NDTTestMethodology#Packet_loss.md)

The Theoretical Maximum Throughput is kept in Mibps.

The above Theoretical Maximum Throughput comes from the matthis equation (http://www.psc.edu/networking/papers/model_ccr97.ps):

```
Rate < (MSS/RTT)*(1 / sqrt(p))
```

where p is the loss probability.

#### Known Issues (Theoretical Maximum Throughput) ####

The Theoretical Maximum Throughput should be computed to receive Mbps instead of Mibps. This is the only variable in the NDT that is kept in Mibps, so it might lead to the inconsistent results when comparing it with the other values.

### 'Congestion Limited' state time share ###

The 'Congestion Limited' state time share is the percentage of the time that the NDT Server was limited in sending due to the congestion window.

The 'Congestion Limited' state time share is computed using the following formula:

```
SndLimTimeCwnd/TotalTestTime
```

where:
  * **SndLimTimeCwnd** - The cumulative time spent in the 'Congestion Limited' state
  * **TotalTestTime** - [Total test time](NDTTestMethodology#Total_test_time.md)

### 'Receiver Limited' state time share ###

The 'Receiver Limited' state time share is the percentage of the time that the NDT Server was limited in sending due to the NDT Client's receive window.

The 'Receiver Limited' state time share is computed using the following formula:

```
SndLimTimeRwin/TotalTestTime
```

where:
  * **SndLimTimeRwin** - The cumulative time spent in the 'Receiver Limited' state
  * **TotalTestTime** - [Total test time](NDTTestMethodology#Total_test_time.md)

### 'Sender Limited' state time share ###

The 'Sender Limited' state time share is the percentage of the time that the NDT Server was limited in sending due to its own fault.

The 'Sender Limited' state time share is computed using the following formula:

```
SndLimTimeSender/TotalTestTime
```

where:
  * **SndLimTimeSender** - The cumulative time spent in the 'Sender Limited' state
  * **TotalTestTime** - [Total test time](NDTTestMethodology#Total_test_time.md)

## Known Issues/Limitations ##

Two overall known limitations are that NDT requires that the TCP congestion algorithms be Reno, and that it requires packet coalescing to be disabled. If these are not the case, some of NDT's heuristics may not be accurate. These limitations, however, will negatively impact the throughput tests. NDT's results are, thus, conservative, showing the worst performance a client might see.

Some specific issues/limitations have been found in the NDT regarding the following areas:
  * [Middlebox Test](NDTTestMethodology#Known_Issues_(Middlebox_Test).md)
  * [Bottleneck Link Detection](NDTTestMethodology#Known_Limitations_(Bottleneck_Link_Detection).md)
  * [Duples Mismatch Detection](NDTTestMethodology#Known_Issues/limitations_(Duplex_Mismatch_Detection).md)
  * [DSL/Cable modem detection heuristic](NDTTestMethodology#Known_Issues_(DSL/Cable_modem_detection_heuristic).md)
  * [Faulty Hardware Link Detection](NDTTestMethodology#Known_Issues_(Faulty_Hardware_Link_Detection).md)
  * [Theoretical maximum throughput](NDTTestMethodology#Known_Issues_(Theoretical_Maximum_Throughput).md)