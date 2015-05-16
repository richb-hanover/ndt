# NDT architecture and life cycle #

## Abstract ##

The Network Diagnostic Tool (NDT) is a client/server program that provides network configuration and performance testing to a user's computer. The NDT consists of several modules responsible for various aspects of the [test methodology](NDTTestMethodology.md). This document identifies where in the overall codebase each of the high-level components reside to help with future development and maintenance. It also includes build instructions and documents the build and packaging process.

## Table of Contents ##



## Introduction ##

Only NDT Client and NDT Server are needed to perform all tests defined by the [NDT Protocol](NDTProtocol.md). However, the full NDT suite consists of the wide spectrum of tools that can be used for different purposes. We can specify three categories of the NDT components based on their areas of usage:
  * Client Tools
  * Server Tools
  * Administrative Tools

The NDT project is hosted on the [Google Project Hosting](http://code.google.com/p/ndt) infrastructure. The latest project source code can be obtained anonymously over HTTP by using the following command:

`svn checkout http://ndt.googlecode.com/svn/trunk/ ndt-read-only`

Moreover, it can be also obtained by downloading the newest tar package from the download page:

`http://code.google.com/p/ndt/downloads`

## Life Cycle ##

The NDT client and server interaction details can be found in the [NDT Protocol description](NDTProtocol#Protocol_description.md).

### Test session with the Java Applet-based client ###

A basic NDT test session with the Java Applet-based client looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDT_flow_1.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDT_flow_1.png)

  1. The process starts with the user opening a browser and entering a known NDT server URL.
    * This URL can either point directly to a specific NDT server, or the web browser can be redirected to a different NDT server (either via a Web redirect, or using a DNS-based solution like [DONAR](http://donardns.org/)).
  1. The web server responds by returning the page, with an embedded java applet (class or jar file).
  1. The user must manually request a test be performed by clicking the “start” button.
  1. The applet opens a connection back to the server’s testing engine (web100srv process).
  1. A child process is created to handle the test and the parent goes back to listening for more test requests. The parent keeps a FIFO queue to process multiple requests.
  1. A control channel is created between the server and the client to control the client’s actions and synchronize the start of the various tests. The server and the client negotiate the test suite.
  1. The NDT client and the NDT server performs the negotiated test suite. The client opens new data channels back to the server for testing purposes. Allowing the client to open connections makes it easy to get past client-side firewall boxes.
  1. The server extracts the Web100 data and analyzes the connection for faults.
  1. The results are recorded in the [servers’ log file](NDTDataFormat#web100srv.log_file.md) and the results are returned to the client for display to the user.

### Test session with the web100clt client ###

A basic NDT test session with the web100clt client looks as following:

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDT_flow_2.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDT_flow_2.png)

  1. The process starts with the user specifying a known NDT server for the web100clt client to connect to.
    * Unlike in the Applet client scenario, the web100clt does not support redirection so, unless a DNS-based solution like [DONAR](http://donardns.org/) is employed, the user must direct the client to a specific NDT server.
  1. The web100clt client connects to the server's testing engine (web100srv process).
  1. A child process is created to handle the test and the parent goes back to listening for more test requests. The parent keeps a FIFO queue to process multiple requests.
  1. A control channel is created between the server and the client to control the client’s actions and synchronize the start of the various tests. The server and the client negotiate the test suite.
  1. The NDT client and the NDT server performs the negotiated test suite. The client opens new data channels back to the server for testing purposes. Allowing the client to open connections makes it easy to get past client-side firewall boxes.
  1. The server extracts the Web100 data and analyzes the connection for faults.
  1. The results are recorded in the [servers’ log file](NDTDataFormat#web100srv.log_file.md) and the results are returned to the client for display to the user.

## Architecture Overview ##

We can look at the NDT architecture in two different ways:
  * by distinguish separate executable components (i.e. talk about web100clt and web100srv as two NDT components)
  * by distinguish different subcomponents that the NDT components are using (i.e. both web100clt and web100srv are using logging subcomponent)

### NDT Components ###

A brief description of each of the NDT components can be found in the following table. All sources locations are relative to the root of the checked out sources from the code repository.

| **Component** | **Type** | **Description** | **Sources Location** |
|:--------------|:---------|:----------------|:---------------------|
| **web100clt** | Client Tools | The NDT Client application (used from command line). | **src** <br /><br /> `web100clt.c test_sfw_clt.c test_mid_clt.c test_c2s_clt.c test_s2c_clt.c test_meta_clt.c test_results_clt.c network.c usage.c logging.c utils.c protocol.c runningtest.c ndtptestconstants.c strlutils.c jsonutils.c` |
| **Tcpbw100.jar** | Server/Client Tools | The Java Applet-based client that requires JVM browser plugin installed at the client machine. | **Applet/src** <br /><br /> `Tcpbw100.java StatusPanel.java Protocol.java Message.java UserAgentTools.java NDTConstants.java OsfwWorker.java NewFrame.java ResultsTextPane.java MessageType.java NDTUtils.java JSONUtils.java `|
| **web100srv** | Server Tools | The NDT Server application. | **src** <br /><br /> `web100srv.c test_sfw_srv.c test_meta_srv.c test_c2s_srv.c test_s2c_srv.c test_mid_srv.c heuristics.c web100-util.c web100-pcap.c web100-admin.c runningtest.c network.c protocol.c usage.c utils.c mrange.c logging.c testoptions.c ndtptestconstants.c ndt_odbc.c strlutils.c jsonutils.c` |
| analyze | Administrative Tools | Console based utility application that can look through the NDT log files and display the old test results. | **src** <br /><br /> `analyze.c usage.c logging.c` |
| viewtrace | Administrative Tools | Console based utility application that can read [tcpdump trace](NDTDataFormat#tcpdump_trace.md) files to perform packet pair timings to determine [what type of bottleneck link exists](NDTTestMethodology#Bottleneck_Link_Detection.md) on the analyzed path. | **src** <br /><br /> `viewtrace.c usage.c logging.c utils.c` |
| genplot | Administrative Tools | Console based utility that can read [Web100 snaplog trace](NDTDataFormat#web100_snaplog_trace.md) files and generate xplot graphs from the obtained Web100 variables. | **src** <br /><br /> `genplot.c usage.c` |
| JAnalyze.jar | Administrative Tools | Java based utility application that can look through the NDT log files and display the old test results (along with displaying existing snaplog/tcpdump trace files and additional graphs). | **janalyze/src** <br /><br /> `DBConfFrame.java Helpers.java LoadingDBFrame.java MaxPeakInfo.java PeakInfo.java ResultsContainer.java SimpleTextPane.java SwingWorker.java FilterFrame.java JAnalyze.java LoadingFrame.java MinPeakInfo.java PropertyListener.java ResultsList.java SnaplogFrame.java` |
| Admin.class | Administrative Tools | The Java Applet-based application that provides an administrator view of the NDT usage statistics (this applet is responsible for drawing the bar graph that appears on the admin view html page). | **Admin** <br /><br /> `Admin.java` |
| fakewww | Server Tools | Optional 'lite' web server. <font color='red'>NOTE: fakewww is deprecated. It is suggested that folks use a server like Apache instead</font> | **src** <br /><br /> `fakewww.c troute.c troute6.c tr-tree.c tr-tree6.c network.c usage.c logging.c` |
| tr-mkmap | Administrative Tools | Console based utility application can be used with NDT's federated mode. The utility takes files containing traceroute runs between all the federated ndt servers, and generates a "tree" file. fakewww will consult this tree file when clients connect to ascertain which is the closest NDT server to the client. fakewww will then redirect the client to that server. | **src** <br /><br /> `tr-mkmap.c tr-tree.c tr-tree6.c usage.c logging.c` |

### NDT subcomponents ###

#### NDT Server subcomponents ####

All subcomponents of the NDT Server (web100srv) can be seen on the following diagram:

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA2.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA2.png)

#### NDT C Client subcomponents ####

All subcomponents of the NDT C Client (web100clt) can be seen on the following diagram:

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA3.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA3.png)

#### Subcomponents description ####

A description of each of the NDT subcomponents can be found in the following table. All source files are located in the **src** directory under the root of the checked out sources from the code repository.

##### Test Subcomponents #####
| **Subcomponent** | **Used by** | **Description** | **API** | **Sources** |
|:-----------------|:------------|:----------------|:--------|:------------|
| SFW Test (C) | web100clt | The client part of the Simple Firewall Test | `int `**test\_sfw\_clt**`(int ctlsockfd, char tests, char* host, int conn_options)`<br />`int `**results\_sfw**`(char tests, char* host)` | `test_sfw_clt.c test_sfw.h` |
| SFW Test (S) | web100srv | The server part of the Simple Firewall Test | `int `**test\_sfw\_srv**`(int ctlsockfd, web100_agent* agent, TestOptions* options, int conn_options)` | `test_sfw_srv.c test_sfw.h testoptions.h` |
| MID Test (C) | web100clt | The client part of the Middlebox Test | `int `**test\_mid\_clt**`(int ctlSocket, char tests, char* host, int conn_options, int buf_size, char* tmpstr2)` | `test_mid_clt.c clt_tests.h` |
| MID Test (S) | web100srv | The server part of the Middlebox Test | `int `**test\_mid**`(int ctlsockfd, web100_agent* agent, TestOptions* options, int conn_options, double* s2c2spd)` | `test_mid_srv.c tests_srv.h testoptions.h` |
| C2S Test (C) | web100clt | The client part of the Client-To-Server Throughput Test | `int `**test\_c2s\_clt**`(int ctlSocket, char tests, char* host, int conn_options, int buf_size)` | `test_c2s_clt.c clt_tests.h` |
| C2S Test (S) | web100srv | The server part of the Client-To-Server Throughput Test | `int `**test\_c2s**`(int ctlsockfd, web100_agent* agent, TestOptions* testOptions, int conn_options, double* c2sspd, int set_buff, int window, int autotune, char* device, Options* options, int record_reverse, int count_vars, char spds[4][256], int* spd_index)` | `test_c2s_srv.c tests_srv.h testoptions.h` |
| S2C Test (C) | web100clt | The client part of the Server-To-Client Throughput Test | `int `**test\_s2c\_clt**`(int ctlSocket, char tests, char* host, int conn_options, int buf_size, char* tmpstr)` | `test_s2c_clt.c clt_tests.h` |
| S2C Test (S) | web100srv | The server part of the Server-To-Client Throughput Test | `int `**test\_s2c**`(int ctlsockfd, web100_agent* agent, TestOptions* testOptions, int conn_options, double* s2cspd, int set_buff, int window, int autotune, char* device, Options* options, char spds[4][256], int* spd_index, int count_vars, CwndPeaks* peaks)` | `test_s2c_srv.c tests_srv.h testoptions.h` |
| META Test (C) | web100clt | The client part of the META Test | `int `**test\_meta\_clt**`(int ctlSocket, char tests, char* host, int conn_options)` | `test_meta_clt.c test_meta.h` |
| META Test (S) | web100srv | The server part of the META Test | `int `**test\_meta\_srv**`(int ctlsockfd, web100_agent* agent, TestOptions* testOptions, int conn_options)` | `test_meta_srv.c test_meta.h testoptions.h` |

##### Utility Subcomponents #####
| logging | web100srv<br />web100clt | Logging subcomponent that is responsible for printing informational messages on to the console. It can filter messages according to the currently set debug level. Moreover, this subcomponent is also responsible for dates/timestamps handling. | `void `**log\_init**`(char* progname, int debuglvl)`<br />`void `**set\_debuglvl**`(int debuglvl)`<br />`void `**set\_logfile**`(char* filename)`<br />`int `**get\_debuglvl**`()`<br />`char* `**get\_logfile**`()`<br />`I2ErrHandle `**get\_errhandle**`()`<br />`void `**log\_print**`(int lvl, const char* format, ...)`<br />`void `**log\_println**`(int lvl, const char* format, ...)`<br />`void `**log\_free**`(void)`<br />`void `**set\_timestamp**`()`<br />`char* `**get\_ISOtime**`(char * isoTime)`<br />`void `**get\_YYYY**`(char * year)`<br />`void `**get\_MM**`(char * month)`<br />`void `**get\_DD**`(char * day)` | `logging.c logging.h` |
|:--------|:-------------------------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----------------------|
| network | web100srv<br />web100clt | Subcomponent that is responsible for all network operations. | `I2Addr `**CreateListenSocket**`(I2Addr addr, char* serv, int options, int buf_size)`<br />`int `**CreateConnectSocket**`(int* sockfd, I2Addr local_addr, I2Addr server_addr, int option, int buf_sizes)`<br />`int `**send\_msg**`(int ctlSocket, int type, void* msg, int len)`<br />`int `**recv\_msg**`(int ctlSocket, int* type, void* msg, int* len)`<br />`int `**writen**`(int fd, void* buf, int amount)`<br />`int `**readn**`(int fd, void* buf, int amount)` | `network.c network.h` |
| protocol | web100srv<br />web100clt | Functions to support [NDT Protocol](NDTProtocol.md). | `int `**check\_msg\_type**`(char* prefix, int expected, int received, char* buff, int len)` | `protocol.c protocol.h` |
| utils | web100srv<br />web100clt | Functions to handle numbers sanity checks and some other utility things. | `int `**check\_int**`(char* text, int* number)`<br />`int `**check\_rint**`(char* text, int* number, int minVal, int maxVal)`<br />`int `**check\_long**`(char* text, long* number)`<br />`double `**secs**`()`<br />`void `**err\_sys**`(char* s)`<br />`int `**sndq\_len**`(int fd)`<br />`void `**mysleep**`(double time)` | utils.c utils.h |
| Web100 support | web100srv | Functions needed to read the Web100 variables. | `void `**get\_iflist**`(void)`<br />`int `**web100\_init**`(char *VarFileName)`<br />`int `**web100\_autotune**`(int sock, web100_agent* agent, web100_connection* cn)`<br />`void `**web100\_middlebox**`(int sock, web100_agent* agent, web100_connection* cn, char *results)`<br />`int `**web100\_setbuff**`(int sock, web100_agent* agent, web100_connection* cn, int autotune)`<br />`void `**web100\_get\_data\_recv**`(int sock, web100_agent* agent, web100_connection* cn, int count_vars)`<br />`int `**web100\_get\_data**`(web100_snapshot* snap, int ctlsock, web100_agent* agent, int count_vars)`<br />`int `**CwndDecrease**`(web100_agent* agent, char* logname, int *dec_cnt, int *same_cnt, int *inc_cnt)`<br />`int `**web100\_logvars**`(int *Timeouts, int *SumRTT, int *CountRTT, int *PktsRetrans, int *FastRetran, int *DataPktsOut, int *AckPktsOut, int *CurrentMSS, int *DupAcksIn, int *AckPktsIn, int *MaxRwinRcvd, int *Sndbuf, int *CurrentCwnd, int *SndLimTimeRwin, int *SndLimTimeCwnd, int *SndLimTimeSender, int *DataBytesOut, int *SndLimTransRwin, int *SndLimTransCwnd, int *SndLimTransSender, int *MaxSsthresh, int *CurrentRTO, int *CurrentRwinRcvd, int *MaxCwnd, int *CongestionSignals, int *PktsOut, int *MinRTT, int count_vars, int *RcvWinScale, int *SndWinScale, int *CongAvoid, int *CongestionOverCount, int *MaxRTT, int *OtherReductions, int *CurTimeoutCount, int *AbruptTimeouts, int *SendStall, int *SlowStart, int *SubsequentTimeouts, int *ThruBytesAcked)` | `web100-util.c web100srv.h` |
| pcap support | web100srv | Functions that use pcap library in the bottleneck detection algorithm. | `void `**init\_pkttrace**`(I2Addr srcAddr, struct sockaddr *sock_addr, socklen_t saddrlen, int monitor_pipe[2], char *device, PortPair* pair, char * direction, int compress)`<br />`int `**check\_signal\_flags**`()` | `web100-pcap.c web100srv.h` |
| admin page | web100srv | Functions needed to handle the Admin page, which allows a remote user to view the usage statistics via a web page. | `void `**view\_init**`(int refresh)`<br />`int `**calculate**`(char now[32], int SumRTT, int CountRTT, int CongestionSignals, int PktsOut, int DupAcksIn, int AckPktsIn, int CurrentMSS, int SndLimTimeRwin, int SndLimTimeCwnd, int SndLimTimeSender, int MaxRwinRcvd, int CurrentCwnd, int Sndbuf, int DataBytesOut, int mismatch, int bad_cable, int c2sspd, int s2cspd, int c2sdata, int s2cack, int view_flag)`<br />`void `**gen\_html**`(int c2sspd, int s2cspd, int MinRTT, int PktsRetrans, int Timeouts, int Sndbuf, int MaxRwinRcvd, int CurrentCwnd, int mismatch, int bad_cable, int totalcnt, int refresh)` | `web100-admin.c web100-admin.h` |
| mrange | web100srv | Functions to handle port number ranges. This means that the numbers generated by these methods are always between 1 and 65535 inclusive. | `int `**mrange\_parse**`(char* text)`<br />`char* `**mrange\_next**`(char* port)` | `mrange.c mrange.h` |

Some of the Utility Subcomponents depend on other of the Utility Subcomponents. The Test Subcomponents dependencies were included in the diagrams above. The Utility Subcomponent dependencies are listed in the diagram below.

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA4.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA4.png)

#### NDT Java Applet subcomponents ####

The NDT Java Applet-based client (Tcpbw100.jar) has the same functionality as the command line client (web100clt), but all its code is located in a single Tcpbw100.java file. The concise implementation allow us to only distinguish the following subcomponents related to particular tests:

| **Subcomponent** | **Location** |
|:-----------------|:-------------|
| SFW Test (C) | `boolean `**test\_sfw**`(Protocol ctl)` |
| MID Test (C) | `boolean `**test\_mid**`(Protocol ctl)` |
| C2S Test (C) | `boolean `**test\_c2s**`(Protocol ctl)` |
| S2C Test (C) | `boolean `**test\_s2c**`(Protocol ctl, Socket ctlSocket)` |
| META Test (C) | `boolean `**test\_meta**`(Protocol ctl) ` |
| Protocol | `Class `**Protocol** |
| User Interface | Throughout |

This simple architecture can be seen on the following diagram:

![http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA1.png](http://ndt.googlecode.com/svn/wiki/NDTArchitecture.images/NDTA1.png)

## Build Instructions ##

### Requirements ###

NDT makes use of a number of third-party components:
| Library | Required To Build Client? | Required To Run Client? | Required To Build Server? | Required To Run Server? |
|:--------|:--------------------------|:------------------------|:--------------------------|:------------------------|
| Web100 Kernel Modifications | No | No | No | Yes |
| Web100 Library | No | No | Yes | Yes |
| Jansson library | Yes (C client only) | Yes (C client only) | Yes | Yes |
| pcap library | No | No | Yes | Yes |
| I2Util library | Yes (C Client Only) | No | Yes | No |
| zlib library | No | No | No (compression of test results will be disabled) | No |
| odbc library | No | No | No (storing results in a SQL database will be disabled) | No |
| Java SDK | Yes (Java Client-only) | Yes (Java Client-only) | No (JAnalyze application and Admin.class will not be built) | No |

If NDT discovers the above components during the _configure_ process, the associated software will be built. Otherwise, only the software that can be built with the available third-party components will be built.

#### Web100 patched Linux kernel ####

Web100 patch for the Linux kernel can be obtained from the [Web100 download page](http://www.web100.org/download/).

The base Linux kernel can be obtained from the [kernel.org](http://kernel.org) ftp site:
  * ftp [ftp.kernel.org](ftp://ftp.kernel.org) and change directory to the proper Linux kernel directory

Unpack the kernel tar file in the `/usr/src` directory, unpack the Web100 kernel patch, and apply the patch:
  * `patch –p3 < path-to-kernel-patch-file`

It is important to note that kernel patch files are keyed to a specific version of the Linux kernel. You must look at the Web100 kernel readme file to determine which kernel version you need to download/patch.

Configure and build the new kernel; note: you must enable _“Prompt for development code ...”_ so that Web100 options appear under _“Networking Options.”_ Modify the boot loader configuration file, reboot, and test the new system.

#### Web100 user library ####

Web100 user library can be obtained from the [Web100 download page](http://www.web100.org/download/).

After downloading the Web100 user library sources, unpack the downloaded tar file and use the standard GNU automake commands:
  1. Change directory to package directory
  1. Create local make files (`./configure {--prefix=dir`})
  1. Build library and utility files (`make`)
  1. Install library and utility files (`make install`)

Once the kernel is built and running, it will automatically begin collecting data on every TCP connection to/from the server. The user library file contains the routines needed to extract that kernel data from the system. The administrator can verify that the kernel is recording properly by using the `/usr/local/bin/gutil` program. This X-windows based GUI allows the administrator to monitor any TCP connection going to/from the server.

#### pcap library ####

The pcap library provides raw access to the network interface. The NDT uses packet pair dispersion techniques to determine the bottleneck link speed/type. This means that the _libpcap.so_ runtime library must be installed on the NDT server. This library is a standard part of all Linux distributions. The administrator should check the NDT server to ensure the pcap library was installed when the system was built.

The command:

`ls /usr/lib/libpcap*`

will return several files if the libraries are installed. If they are not installed, obtain the installation package from your favorite mirror site or download and install the source from the http://www.tcpdump.org web site.

#### Jansson library ####

The Jansson library is used by NDT server and C-client to encode/decode messages using JSON format. The source code can be obtained from `http://www.digip.org/jansson`.

#### I2Util library ####

The I2Util library is already included in the newest tar package from the NDT download page (`http://code.google.com/p/ndt/downloads`).

However, it is not included in the source code obtained from the NDT subversion repository (`http://ndt.googlecode.com/svn/trunk`). In this case the I2Util library sources can be obtained anonymously over HTTP by using the following command:

`svn checkout http://anonsvn.internet2.edu/svn/I2util/trunk/ I2util`

Next, the following steps need to be performed before the NDT can be properly built:
  1. Change directory to the newly created I2Util directory
  1. Create the _configure_ scripts (`./bootstrap`)

### Sources ###

NDT sources can be obtained in two ways:
  * by checking out the newest code from the subversion repository (`http://ndt.googlecode.com/svn/trunk`)
  * by downloading the newest tar package from the download page (`http://code.google.com/p/ndt/downloads`)

### Building the NDT components ###

Once all the pre-requisites are installed, the admin is now ready to create the NDT system by using the standard GNU automake tools to create and install the executables:
  1. Change directory to package directory
  1. Create make files (./configure {--prefix=/some/dir})
> > The _configure_ process displays the short summary about the configured NDT modules:
```
***********************************
***   Network Diagnostic Tool   ***
***********************************
*       configured modules        *
***********************************


*** Client Tools - complete
* web100clt:    YES
* Tcpbw100.jar: YES
* flash-client: YES

*** Server Tools - complete
* fakewww:      YES
* web100srv:    YES

*** Administrative Tools - complete
* analyze:      YES
* viewtrace:    YES
* tr-mkmap:     YES
* genplot:      YES
* JAnalyze.jar: YES
* Admin.class:  YES
```
  1. Build executables (make)
  1. Install executables (make install)

This process creates both server programs (web100srv and fakewww), the client tool (web100clt), the administrative tools (analyze, viewtrace, genplot, tr-mkmap), and it builds the java class and jar programs. The _make install_
process puts all the executables in the proper place.

### Preparing release packages ###

In order to prepare the release package, the [I2Util library sources](NDTArchitecture#I2Util_library.md) have to be located inside the NDT sources directory.

The release package is created with the following command:

`make dist`

The above command will create a gzipped tar archive (ndt-VERSION.tar.gz).

It is also possible to create release packages in different formats. You can find all supported formats in the following table:

| **command** | **package type** | **package name** |
|:------------|:-----------------|:-----------------|
| `make dist-gzip` | tar archive compressed with `gzip` | ndt-VERSION.tar.gz |
| `make dist-bzip2` | tar archive compressed with `bzip2` | ndt-VERSION.tar.bz2 |
| `make dist-lzma` | tar archive compressed with `lzma` | ndt-VERSION.tar.lzma |
| `make dist-xz` | tar archive compressed with `xz` | ndt-VERSION.tar.xz |
| `make dist-tarZ` | tar archive compressed with `compress` | ndt-VERSION.tar.Z |
| `make dist-shar` | shar archive compressed with `gzip` | ndt-VERSION.shar.gz |
| `make dist-zip` | zip archive | ndt-VERSION.zip |