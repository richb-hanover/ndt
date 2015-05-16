# Data collected by the NDT #

## Abstract ##

The Network Diagnostic Tool (NDT) is a client/server program that provides network configuration and performance testing to a user's computer. The measured factors along with some additional analysis and detailed test traces are stored by the server for the future reuse. Moreover, multi-level results allow novice and expert users to view and understand the test results. This document describes what, where and when is stored by the NDT server. The information contained here should be enough to support parsing of the collected data for analysis and visualization purposes.

## Table of Contents ##



## Introduction ##

The NDT uses the following files to store the test results data:

| **File** | **Scope of the collected data** | **Always used?** | **File extension** |
|:---------|:--------------------------------|:-----------------|:-------------------|
| **tcpdump trace** | C2S/S2C tests | **NO** (enabled by `-t, --tcpdump` options) | `*`.c2s\_ndttrace / `*`.s2c\_ndttrace |
| **web100 snaplog trace** | C2S/S2C tests | **NO** (enabled by `--snaplog` option) | `*`.c2s\_snaplog / `*`.s2c\_snaplog |
| **web100srv.log** | test session | YES | `*`.log |
| **cputime trace** | test session | **NO** (enabled by `--cputime` option) | `*`.cputime |
| **meta data file** | test session | YES | `*`.meta |

The web100srv.log file is by default written under the `/usr/local/ndt/` directory. However, this can be changed by using the `-l, --log` options.

All other files are saved under the data directory (by default `/usr/local/ndt/serverdata/`) which can be set by using the `-L, --log_dir` options. To minimize the number of files in one directory, the test results are further grouped by the year, month and day the test was started in.

To sum up, the result files are saved in the following directories:

```
/DataDirPath/YYYY/MM/DD/
```

For example:

```
/usr/local/ndt/serverdata/2011/08/24/
```

The mentioned files (all except web100srv.log) are saved using the following filename format:

| **ISOtime**`_`**serverFQDN**:**portNumber**.**extension** |
|:----------------------------------------------------------|

where:
  * **ISOtime** - current time in the ISO format
  * **serverFQDN** - server host fully qualified name
  * **portNumber** - currently used port number (ephemeral ports for the snaplog and tcpdump traces, control session port for the cputime and meta files)
  * **extension** - extension used to distinguish file types

For example:

```
20110824T21:11:45.99598000Z_jeremian-laptop.local:33687.c2s_ndttrace
```

## Collected data formats ##

### meta data file ###

The meta data file is always created for every test session. If the particular test does not have a corresponding meta data file, it can be caused by the following issues:
  * the test was abruptly interrupted by some unexpected error
  * there was an error during file creation (for example lack of disk space)
  * the test was performed by the old NDT version, that did not write meta data files

The meta data file contains the names of the other files, the clients & servers IP addr and FQDN, and the encoded line of the web100/analysis data. The meta data contains lines in the following order:

| **#** | **Name** | **Description/Comment** |
|:------|:---------|:------------------------|
| 1. | Date/Time | Timestamp of the beginning of the test session |
| 2. | C2S [snaplog](NDTDataFormat#web100_snaplog_trace.md) file | Filename of the snaplog taken during C2S Throughput test |
| 3. | C2S [ndttrace](NDTDataFormat#tcpdump_trace.md) file | Filename of the tcpdump trace taken during C2S Throughput test |
| 4. | S2C [snaplog](NDTDataFormat#web100_snaplog_trace.md) file | Filename of the snaplog taken during S2C Throughput test |
| 5. | S2C [ndttrace](NDTDataFormat#tcpdump_trace.md) file | Filename of the tcpdump trace taken during S2C Throughput test |
| 6. | [cputime](NDTDataFormat#cputime_trace.md) file | Filename of the cputime trace taken during the whole test session |
| 7. | server IP address | IP address of the NDT server |
| 8. | server hostname | Hostname of the NDT server |
| 9. | server kernel version | Kernel version of the NDT server's OS |
| 10. | client IP address | IP address of the NDT client |
| 11. | client hostname | Fully qualified hostname of the NDT client |
| 12. | client OS name | (**optional**) Name of the NDT client's OS |
| 13. | client browser name | (**optional**) Name of the NDT client's web browser (if applicable) |
| 14. | Summary data | A one-line summary of the test containing the final values of some of the web100 variables with an additional analysis details. The particular values are separated with commas without any spaces. The results are written with the same order and contain the same values as the one-line summary from the [web100srv.log file](NDTDataFormat#web100srv.log_file.md) (except the date and client name) |
| 15. | Additional data | (**optional**) A multi-line information containing additional data sent by the client using the META protocol. Currently defined key/value pairs can be found in the [NDT Protocol document](NDTProtocol#META_test.md) |

The optional values from the meta data file are set during the [META test](NDTProtocol#META_test.md). This means that these values are empty when the client does not support the [META test](NDTProtocol#META_test.md) or it does not send these values.

Please look at the following sample meta data file:

```
Date/Time: 20091009T20:46:28.141927000Z
c2s_snaplog file: 20091009T20:46:28.141927000Z_149.20.53.166:60102.c2s_snaplog.gz
c2s_ndttrace file: 20091009T20:46:28.141927000Z_149.20.53.166:60102.c2s_ndttrace.gz
s2c_snaplog file:
s2c_ndttrace file: 20091009T20:46:28.141927000Z_149.20.53.166:61730.s2c_ndttrace.gz
cputime file:
server IP address: 4.71.254.147
server hostname: mlab1.atl01.measurement-lab.org
server kernel version: 2.6.22.19-vs2.3.0.34.32.mlab.pla
client IP address: 149.20.53.166
client hostname: nb.tech.org
client OS name:
client_browser name:
Summary data:313,2246,1658,0,69963,1020,19,8,1961,0,1448,392,1428,525600,112560,
43440,0,10001122,25307,2899352,0,15,15,21720,270,525600,100,0,0,0,1,6,2,8,10,8,1961,
66,10,22,964,0,166,0,-1208754176,3,0,31,0,360,1448,43440,8
```


### tcpdump trace ###

Writing tcpdump trace files is enabled by the `-t, --tcpdump` options.

These are a standard trace files for all packets (except the initial syn & syn/ack exchange) sent during the individual C2S/S2C subtests (only the throughput tests' traffic is gathered that is sent on a newly created connection, all the NDT control protocol communication is not captured in these dump files).

The packets from the C2S Throughput test are stored in the `*`.c2s\_ndttrace file, the packets from the S2C Throughput test are stored in the `*`.s2c\_ndttrace file.

These tcpdump files are written using the [pcap](http://www.tcpdump.org/) library.

The tcpdump files can be read using [tcpdump](http://www.tcpdump.org/) and [tcptrace](http://www.tcptrace.org/) programs.

### web100 snaplog trace ###

Writing snaplog files is enabled by the `--snaplog` option.

These files contain all the [web100](http://www.web100.org/) kernel MIB variables' values written in a fixed time (default is 5 msec) increments for the individual C2S/S2C subtests (these snapshots are taken only for a newly created connection used for the throughput test).

The snapshots from the C2S Throughput test are stored in the `*`.c2s\_snaplog file, the snaphots from the S2C Throughput test are stored in the `*`.s2c\_snaplog file.

The snaplog files are a binary files written in the format defined by the [web100](http://www.web100.org/) library. In addition to the standard [web100](http://www.web100.org/) variables' values written in each snapshot, the 'Duration' variable is also stored to indicate the number of seconds from the beginning of the snaplog session.

The list of logged variables with a short description can be found in the [NDT Protocol document](NDTProtocol#Appendix_A._web100_variables.md).

The NDT package comes with a utility (called genplot) that can convert snaplog trace files into text or xplot graph files. Alternatively the user can write their own analysis program using the [web100](http://www.web100.org/) library functions.

### web100srv.log file ###

The NDT always writes a one-line summary of the test to the `web100srv.log` file. The `web100srv.log` file is a standard log file and may contain lines that aren't summary lines (e.g. if debugging is enabled, there's a ton of information logged to this file).

This one-line summary contains the final values of some of the web100 variables with an additional analysis details. The particular values are separated with commas without any spaces. The results are stored with the following order:

| **#** | **Name** | **Description/Comment** |
|:------|:---------|:------------------------|
| 1. | date | start time of the test session (in the form "Jun 30 21:49:08"). **Warning:** this field does not contain information about the year |
| 2. | client name | remote (client) host fully qualified name (if the fully qualified name cannot be determined, then the numeric form of the address will be used here) |
| 3. | MID throughput speed | CWND limited throughput speed measured during Middlebox test (value in kb/s). The details how this value is computed can be found in the [NDT Test Methodology document](NDTTestMethodology#Middlebox_Test.md). |
| 4. | S2C throughput speed | measured throughput speed from server to client (value in kb/s). The details how this value is computed can be found in the [NDT Test Methodology document](NDTTestMethodology#S2C_Throughput_Test.md). |
| 5. | C2S throughput speed | measured throughput speed from client to server (value in kb/s). The details how this value is computed can be found in the [NDT Test Methodology document](NDTTestMethodology#C2S_Throughput_Test.md). |
| 6. | Timeouts | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of times the retransmit timeout has expired when the RTO backoff multiplier is equal to one. |
| 7. | SumRTT | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The sum of all sampled round trip times. |
| 8. | CountRTT | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of round trip time samples included in tcpEStatsPathSumRTT and tcpEStatsPathHCSumRTT. |
| 9. | PktsRetrans | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of segments transmitted containing at least some retransmitted data. |
| 10. | FastRetran | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of invocations of the Fast Retransmit algorithm. |
| 11. | DataPktsOut | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of segments sent containing a positive length data segment. |
| 12. | AckPktsOut | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of pure ack packets that have been sent on this connection by the Local Host. |
| 13. | CurMSS | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The current maximum segment size (MSS), in octets.|
| 14. | DupAcksIn | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of duplicate ACKs received. |
| 15. | AckPktsIn | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of valid pure ack packets that have been received on this connection by the Local Host. |
| 16. | MaxRwinRcvd | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The maximum window advertisement received, in octets. |
| 17. | Sndbuf | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The socket send buffer size in octets. Note that the meaning of this variable is implementation dependent. Particularly, it may or may not include the retransmit queue. |
| 18. | MaxCwnd | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The maximum congestion window used during Slow Start, in octets. |
| 19. | SndLimTimeRwin | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The cumulative time spent in the 'Receiver Limited' state. |
| 20. | SndLimTimeCwnd | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The cumulative time spent in the 'Congestion Limited' state. |
| 21. | SndLimTimeSender | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The cumulative time spent in the 'Sender Limited' state. |
| 22. | DataBytesOut | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of octets of data contained in transmitted segments, including retransmitted data. Note that this does not include TCP headers. |
| 23. | SndLimTransRwin | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of transitions into the 'Receiver Limited' state from either the 'Congestion Limited' or 'Sender Limited' states. This state is entered whenever TCP transmission stops because the sender has filled the announced receiver window. |
| 24. | SndLimTransCwnd | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of transitions into the 'Congestion Limited' state from either the 'Receiver Limited' or 'Sender Limited' states. This state is entered whenever TCP transmission stops because the sender has reached some limit defined by congestion control (e.g. cwnd) or other algorithms (retransmission timeouts) designed to control network traffic. |
| 25. | SndLimTransSender | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The number of transitions into the 'Sender Limited' state from either the 'Receiver Limited' or 'Congestion Limited' states. This state is entered whenever TCP transmission stops due to some sender limit such as running out of application data or other resources and the Karn algorithm. When TCP stops sending data for any reason which can not be classified as Receiver Limited or Congestion Limited it MUST be treated as Sender Limited. |
| 26. | MaxSsthresh | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The maximum slow start threshold, excluding the initial value. |
| 27. | CurRTO | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The current value of the retransmit timer RTO. |
| 28. | CurRwinRcvd | (**`*`**[tcp-kis.txt](http://www.web100.org/download/kernel/tcp-kis.txt)) The most recent window advertisement received, in octets. |
| **29.** | **link** | A detected link type by the set of custom heuristics. The details of the detection algorithm can be found in the [NDT Test Methodology document](NDTTestMethodology#Link_Type_Detection_Heuristics.md).<br><br>This variable can have the following values:<ul><li><b>100</b> - link type cannot be detected (tests did not recognize the link)</li><li><b>0</b> - detection algorithm failed (due to some error condition)</li><li><b>10</b> - Ethernet link (Fast Ethernet)</li><li><b>3</b> - wireless link</li><li><b>2</b> - DSL/Cable modem link</li></ul> <br>
<tr><td> <b>30.</b> </td><td> <b>duplex mismatch</b> </td><td> A detected duplex mismatch condition. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Duplex_Mismatch_Detection.md'>NDT Test Methodology document</a>.<br><br>This variable can have the following values:<ul><li><b>0</b> - no duplex mismatch condition was detected</li><li><b>1</b> - possible duplex mismatch condition was detected by the Old Duplex-Mismatch algorithm</li><li><b>2</b> - possible duplex mismatch condition was detected by the new algorithm: Switch=Full and Host=Half</li></ul> </td></tr>
<tr><td> <b>31.</b> </td><td> <b>bad_cable</b> </td><td> A detected bad cable condition. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Faulty_Hardware_Link_Detection.md'>NDT Test Methodology document</a>.<br><br>This variable can have the following values:<ul><li><b>0</b> - no bad cable condition was detected</li><li><b>1</b> - possible bad cable condition was detected</li></ul> </td></tr>
<tr><td> <b>32.</b> </td><td> <b>half_duplex</b> </td><td> A detected half duplex condition. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Full/Half_Link_Duplex_Setting.md'>NDT Test Methodology document</a>.<br><br>This variable can have the following values:<ul><li><b>0</b> - no half duplex condition was detected (it means a full duplex subnet in the case of the Fast Ethernet link)</li><li><b>1</b> - possible half duplex condition was detected (it means a half duplex subnet in the case of the Fast Ethernet link)</li></ul> </td></tr>
<tr><td> <b>33.</b> </td><td> <b>congestion</b> </td><td> A detected congestion condition. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Normal_Congestion_Detection.md'>NDT Test Methodology document</a>.<br><br>This variable can have the following values:<ul><li><b>0</b> - no congestion condition was detected</li><li><b>1</b> - possible congestion condition was detected (it means that other network traffic was congesting the link during the test)</li></ul> </td></tr>
<tr><td> <b>34.</b> </td><td> <b>c2sdata</b> </td><td> A link type detected by the Bottleneck Link Detection algorithm using the <b>Client --> Server data packets</b>' inter-packet arrival times. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Bottleneck_Link_Detection.md'>NDT Test Methodology document</a>. <br><br>This variable can have the following values:<ul><li><b>-1</b> - System Fault</li><li><b>0</b> - RTT</li><li><b>1</b> - Dial-up Modem</li><li><b>2</b> - Cable/DSL modem</li><li><b>3</b> - 10 Mbps Ethernet or WiFi 11b subnet</li><li><b>4</b> - 45 Mbps T3/DS3 or WiFi 11 a/g subnet</li><li><b>5</b> - 100 Mbps Fast Ethernet subnet</li><li><b>6</b> - a 622 Mbps OC-12 subnet</li><li><b>7</b> - 1.0 Gbps Gigabit Ethernet subnet</li><li><b>8</b> - 2.4 Gbps OC-48 subnet</li><li><b>9</b> - 10 Gbps 10 Gigabit Ethernet/OC-192 subnet</li><li><b>10</b> - Retransmissions</li></ul></td></tr>
<tr><td> <b>35.</b> </td><td> <b>c2sack</b> </td><td> A link type detected by the Bottleneck Link Detection algorithm using the <b>Client <-- Server Ack packets</b>' inter-packet arrival times. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Bottleneck_Link_Detection.md'>NDT Test Methodology document</a>. <br><br>This variable can have the same values as the <b>c2sdata</b> variable </td></tr>
<tr><td> <b>36.</b> </td><td> <b>s2cdata</b> </td><td> A link type detected by the Bottleneck Link Detection algorithm using the <b>Server --> Client data packets</b>' inter-packet arrival times. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Bottleneck_Link_Detection.md'>NDT Test Methodology document</a>. <br><br>This variable can have the same values as the <b>c2sdata</b> variable </td></tr>
<tr><td> <b>37.</b> </td><td> <b>s2cack</b> </td><td> A link type detected by the Bottleneck Link Detection algorithm using the <b>Server <-- Client Ack packets</b>' inter-packet arrival times. The details of the detection algorithm can be found in the <a href='NDTTestMethodology#Bottleneck_Link_Detection.md'>NDT Test Methodology document</a>. <br><br>This variable can have the same values as the <b>c2sdata</b> variable </td></tr>
<tr><td> 38. </td><td> CongestionSignals </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of multiplicative downward congestion window adjustments due to all forms of congestion signals, including Fast Retransmit, ECN and timeouts. This object summarizes all events that invoke the MD portion of AIMD congestion control, and as such is the best indicator of how cwnd is being affected by congestion. </td></tr>
<tr><td> 39. </td><td> PktsOut </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The total number of segments sent. </td></tr>
<tr><td> 40. </td><td> MinRTT </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The minimum sampled round trip time. </td></tr>
<tr><td> 41. </td><td> RcvWinScale </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The value of Rcv.Wind.Scale. Note that RcvWinScale is either zero or the same as WinScaleSent. </td></tr>
<tr><td> 42. </td><td> autotune </td><td> (<b>deprecated/not used</b>) This value kept the information about web100 autotune functionality. It could have the following values: 0 - autotune is disabled, 1 - sbufmode autotune is enabled, 2 - rbufmode autotune is enabled, 3 - all autotune modes are enabled, 22 - autotune params cannot be found, 23 - autotune params cannot be read </td></tr>
<tr><td> 43. </td><td> CongAvoid </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of times the congestion window has been increased by the Congestion Avoidance algorithm. </td></tr>
<tr><td> 44. </td><td> CongestionOverCount </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of congestion events which were 'backed out' of the congestion control state machine such that the congestion window was restored to a prior value. This can happen due to the Eifel algorithm <a href='http://www.ietf.org/rfc/rfc3522.txt'>RFC3522</a> or other algorithms which can be used to detect and cancel spurious invocations of the Fast Retransmit Algorithm. </td></tr>
<tr><td> 45. </td><td> MaxRTT </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The maximum sampled round trip time. </td></tr>
<tr><td> 46. </td><td> OtherReductions </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of congestion window reductions made as a result of anything other than AIMD congestion control algorithms. Examples of non-multiplicative window reductions include Congestion Window Validation <a href='http://www.ietf.org/rfc/rfc2861.txt'>RFC2861</a> and experimental algorithms such as Vegas. </td></tr>
<tr><td> 47. </td><td> CurTimeoutCount </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The current number of times the retransmit timeout has expired without receiving an acknowledgment for new data. tcpEStatsStackCurTimeoutCount is reset to zero when new data is acknowledged and incremented for each invocation of section 5.5 in <a href='http://www.ietf.org/rfc/rfc2988.txt'>RFC2988</a>. </td></tr>
<tr><td> 48. </td><td> AbruptTimeouts </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of timeouts that occurred without any immediately preceding duplicate acknowledgments or other indications of congestion. Abrupt Timeouts indicate that the path lost an entire window of data or acknowledgments. Timeouts that are preceded by duplicate acknowledgments or other congestion signals (e.g., ECN) are not counted as abrupt, and might have been avoided by a more sophisticated Fast Retransmit algorithm. </td></tr>
<tr><td> 49. </td><td> SendStall </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of interface stalls or other sender local resource limitations that are treated as congestion signals. </td></tr>
<tr><td> 50. </td><td> SlowStart </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of times the congestion window has been increased by the Slow Start algorithm. </td></tr>
<tr><td> 51. </td><td> SubsequentTimeouts </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of times the retransmit timeout has expired after the RTO has been doubled. See section 5.5 in <a href='http://www.ietf.org/rfc/rfc2988.txt'>RFC2988</a>. </td></tr>
<tr><td> 52. </td><td> ThruBytesAcked </td><td> (<b><code>*</code></b><a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a>) The number of octets for which cumulative acknowledgments have been received, on systems that can receive more than 10 million bits per second.  Note that this will be the sum of changes in tcpEStatsAppSndUna. </td></tr>
<tr><td> 53. </td><td> peaks.amount </td><td> The number of times the CWND peaked (i.e. transitioned from increasing to decreasing) during the S2C test. By default, collected at 5ms intervals. </td></tr>
<tr><td> 54. </td><td> peaks.min </td><td> The minimum CWND peak seen during the S2C test. By default, collected at 5ms intervals. </td></tr>
<tr><td> 55. </td><td> peaks.max </td><td> The maximum CWND peak seen during the S2C test. By default, collected at 5ms intervals. </td></tr></tbody></table>

(<b><code>*</code></b>) web100 variable. For a more detailed and up-to-date description please look at the current version of the <a href='http://www.web100.org/download/kernel/tcp-kis.txt'>tcp-kis.txt</a> file.<br>
<br>
<h3>cputime trace</h3>

Writing cputime file is enabled by <code>--cputime</code> option.<br>
<br>
This file contains lines with <code>times</code> routine results recorded using a 100ms interval. Each of the lines contains the following data separated by a single space in this order:<br>
<br>
<table><thead><th> <b>#</b> </th><th> <b>Name</b> </th><th> <b>Description/Comment</b> </th></thead><tbody>
<tr><td> 1. </td><td> time </td><td> seconds from the beginning of the test </td></tr>
<tr><td> 2. </td><td> user time </td><td> contains  the  CPU  time spent executing instructions of the calling process </td></tr>
<tr><td> 3. </td><td> system time </td><td> contains the CPU time spent in the system while executing tasks on behalf of the calling proces </td></tr>
<tr><td> 4. </td><td> user time of the children </td><td> contains the sum of the user time and user time of the children values for all waited-for terminated children </td></tr>
<tr><td> 5. </td><td> system time of the children </td><td> contains the sum of the system time and system time of the children values for all waited-for terminated children </td></tr></tbody></table>

All times reported are in clock ticks.<br>
<br>
A clock tick is same as a cycle, the smallest unit of time recognized by a device. For personal computers, clock ticks generally refer to the main system clock. The number of clock ticks per second can be obtained using:<br>
<pre><code>sysconf(_SC_CLK_TCK);<br>
</code></pre>

<h2>Displaying collected data</h2>

The NDT server also contains a Java application that can look through the NDT log files and link log entries to trace files, and it also simplifies the viewing of this data.<br>
<br>
The JAnalyze application can be started by running a single jar file:<br>
<br>
<pre><code>$ java -jar JAnalyze.jar<br>
</code></pre>

The NDT web100srv.log file can be loaded by using <code>Load</code> option from the <code>File</code> menu.<br>
<br>
You can see the JAnalyze application in action on the following screenshot:<br>
<br>
<img src='http://ndt.googlecode.com/svn/wiki/NDTDataFormat.images/janalyze.png' />

The utility will need to be rewritten slightly to take advantage of the meta files instead of (in addition to) the log files.<br>
<br>
The detailed tcpdump and snaplog files can be used to re-examine the test to see what happened, i.e.:<br>
<ul><li>was there packet loss during the s2c test, and if so when/how often did it occur?<br>
</li><li>what was the maximum throughput during TCP's slow-start growth phase?<br>
</li><li>how many times did TCP oscillate in the Congestion Avoidance phase?<br>
</li><li>did the test compete with other traffic?<br>
</li><li>was there non-congestive loss on the path?<br>
</li><li>was the test limited by the user's PC (default tunable settings)?<br>
</li><li>how often did TCP retransmit packets, and were any of these unnecessary?<br>
</li><li>were packets being reordered (e.g., sent 1, 2, 3, 4, 5 but received 1, 2, 4, 3, 5)<br>
</li><li>what was the capacity of the bottleneck link in this path?<br>
</li><li>did the test produce the expected results?<br>
</li><li>was the client connected to a wired or wireless (WiFi) network?<br>
</li><li>is there a firewall and/or NAT box in the path?</li></ul>

More information about NDT:<br>
<a href='http://www.internet2.edu/performance/ndt/'>http://www.internet2.edu/performance/ndt/</a>