

# NDT Common Warnings and Errors #

The following sections compile a list of common NDT messages and warnings.

## Java Security Warning for Tcpbw100 - The application's digital signature has been verified. Do you want to run the application? ##

This _**warning**_ is actually just a notice, letting users know that NDT uses a _signed_-Java client applet Tcpbw100. This applet is signed by the [New America Foundation](http://newamerica.net/), whose [Open Technology Initiative](http://oti.newamerica.net/home) is a partner in [Measurement Lab](http://measurementlab.org/).  The signed applet assures the user that what they are running is the code that was released by Measurement Lab, and not something else. This certificate guarantees the authenticity of the test, and is required by some NDT integrators. While some earlier versions of the [Sun Java Runtime Environment](http://www.java.com/en/download/manual.jsp) will not display the notice when encountering NDT, the most up-to-date versions will. Users encountering a the above notice, letting them know that the app has been signed, should choose _**Run**_ to allow NDT to run normally.

## NDT message - Another client is currently being served, your test will begin within NNN seconds ##

To ensure accurate results, NDT servers are designed to handle a limited number of tests at once. During very busy times, NDT may queue a small number of newly arriving tests so that it can adequately serve tests that are already in progress.
NDT message - Another client is currently being served, your test will begin within NNN seconds

To ensure accurate results, NDT servers are designed to handle a limited number of tests at once. During very busy times, NDT may queue a small number of newly arriving tests so that it can adequately serve tests that are already in progress.

## Protocol error! or Protocol error!  expected XXXX got YYYY instead or Error in parsing test results! ##

As part of testing, the NDT client server perform a scripted interaction. This message from the client indicates that, in the course of this interaction, the client received results, messages or conditions that were unexpected.  Users receiving this message once should simply try again. If the condition persists, it may indicate a problem that NDT is unable to interpret. Anyone who gets this message repeatedly and reliably is encouraged to report the results to the NDT developers.

## Server process not running: start web100srv process on remote server ##

NDT client needs to connect to port _**3001**_ of the server. If an attempt to connect to that port is not answered or is otherwise refused, the above message is displayed. The message is a reminder to server operators to start the service. Other users receiving this message should consider whether they are behind a security firewall that is configured to block outbound TCP connections to port 3001 (this will not be the case for most residential users). If the condition persists, it may also be the case that the testing server is temporarily unavailable due to a network or server outage.

## Server Busy: Too many clients waiting in server queue. Please try again later. ##

To avoid extremely long waits during very busy times, NDT will reject newly arriving tests if the queue grows too large. Those newly arriving tests will get the above message. Users receiving this message should try testing later.

## About NDT ##

NDT was written by Rich Carlson and volunteer contributors and is maintained by Internet2.  For more information, please see NDT's website at http://www.internet2.edu/performance/ndt/.

## Last Updated ##

$Id$