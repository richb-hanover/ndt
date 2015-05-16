# Protocol validation logging #
## Table of Contents ##




## Features of the protocol log file ##

The protocol log file contains key value pairs containing protocol information in the sequence in which they were received/sent. A sample is:
|event="message", direction="client\_to\_server", test="None", type="MSG\_LOGIN", len="1", msg\_body\_format="bitfield", msg="00111111", pid="8253", socket="3", time="20111101T20:18:45.0Z"|
|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

## Enabling protocol logging ##
The web100srv and web100clt process can be run with 2 additional options:
  1. --enableprotolog: Enables protocol logging. This is turned off by default.
  1. --u _dirname_:  Directory name to place protocol logs in. If this option is not specified, the logs are placed in the default BASEDIR/LOGDIR (Ex: /usr/local/ndt/serverdata)

If case of an RPM installation, you may edit the NDT configuration options file _/etc/sysconfig/ndt_ and change WEB100SRV\_OPTIONS to include protocol logging. For example: WEB100SRV\_OPTIONS="-a -s -l /var/log/ndt/web100srv.log --enableprotolog -u /home/kkumar/ndtlogs --tcpdump"

Else, you can always specify these as command line options:
|web100srv -ddddddddd  -a -s -l /var/log/ndt/web100srv.log  -t –snaplog --enableprotolog |
|:-----------------------------------------------------------------------------------------|

|web100clt -ddddd -n lab234 --enableprotolog -u /home/kkumar/ndtlogs/ |
|:--------------------------------------------------------------------|

### Names of log files ###
Logs are created for every endpoint the tests are run from/to. The name of the logfile includes the local and remote endpoint IP addresses.
Say you executed a test using the command line tool (CLT) installed at IP-A to the NDT server at IP-B. The name of the log file at the client end will be web100srvprotocol\_IP-A\_IP-B.log
(viz. web100srvprotocol\_207.75.164.174\_207.75.165.234.log). The corresponding server-side log file for this client’s test will be web100srvprotocol\_IP-B\_IP-A.log.


## What is logged? ##
> ### Start/End of tests ###
> #### Examples ####
> |event="test\_started", name="Middlebox", pid="14008", time="20111009T21:53:53.669305000Z"|
|:----------------------------------------------------------------------------------------|

> |event="test\_complete", name="SFW", pid="14008", time="20111009T21:53:53.669305000Z"|
|:-----------------------------------------------------------------------------------|

> #### Data pairs logged ####
    * event name
      * Possible values: "test\_started", "test\_complete"
    * test name
    * pid
    * time

> ### All protocol message exchanges ###
> #### Examples ####
> |event="message", direction="server\_to\_client", test="None", type="SRV\_QUEUE", len="1", msg\_body\_format="string", msg="0", pid="8253", socket="3", time="20111101T20:18:45.0Z|
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

> |event="message", direction="server\_to\_client", test="C2S", type="TEST\_MSG", len="6", msg\_body\_format="string", msg="849847", pid="8253", socket="3", time="20111101T20:19:05.0Z|
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

> #### Data pairs logged ####
    * event="message"
    * direction
      * Possible values: client\_to\_server, server\_to\_client
    * Test name
      * Possible values: "None","Middlebox","SFW","C2S", "S2C","Meta"
    * NDTP message type
    * Protocol message payload length
    * Protocol message payload type
      * Possible values: bitfield, string
      * Note: If message payload type=bitfield, then
        * the value is a string of length 8 chars, which is the bit representation of the character.
        * The “len” field will indicate “1” (not 8), since it is still 1 character being passed from server toclient.
        * Example:  “MSG\_LOGIN” message from client to server consists of data indicating which tests were scheduled to be run
> > > > |event="message", direction="client\_to\_server", test="None", type="MSG\_LOGIN", len="1", msg\_body\_format="bitfield", msg="00111111", pid="4191", socket="5", time="20111101T20:18:45.0Z" |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
    * protocol message payload
    * pid
    * socket file descriptor
    * time: This is the precise time at which the protocol messages were exchanged.


> ### Incoming "connections" received ###
> These indicate the start of socket connections from client. They help indicate protocol exchanges that do not use a standard NDTP message type (like TEST\_MSG, TEST\_RESULTS etc). Such a case is seen during the C2S test when the throughput data is sent over a socket.

> #### Examples ####
> |event="started", name="connect", test="C2S", pid="14008", time="20111009T21:53:53.669305000Z"|
|:--------------------------------------------------------------------------------------------|

> |event="started", name="connect", test="SFW", pid="14008", time="20111009T21:53:53.669305000Z"|
|:--------------------------------------------------------------------------------------------|
> #### Data pairs logged ####
    * event="started"
    * name="connect" (currently always indicates incoming)
    * test name
    * pid
    * time

## Changes in code ##
This section is included to point the developer community to the areas in the codebase that were changed to include the protocol logging functionality.

Code is common across server and command line tool. All of the protocol message logging was done with simple additions to src/network.c. Process status (like reception of "connections" from the server ) have been specifically added in code at the locations where they occur. All such code is aggregated in src/logging.c, src/ndtpconstants.c, src/runningtest.c.
