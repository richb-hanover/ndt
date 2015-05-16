# FAQ about How To Interpret NDT Results #

## Introduction ##
This document contains a list of questions asked by NDT users and the respective answers provided by different people who worked or are still working on NDT.
The plan is to write a complete document on how to interpret the results of an NDT test. Until that document is ready, please refer to this list. If you don't find an answer to your question, please contact the NDT team. If your question can help other users, it will included in the following list.

## Questions and Answers ##

### Question 1 ###
A consumer has written to us in relation to an NDT measurement report indicating that the server’s 85.0 KByte buffer is limiting throughput to 19.16 Mbps. Considering that ADSL connections are up to 24Mbps and VDSL is on the way, he is wondering if/how the NDT server could measure a faster connection.

### Answer 1 ###
Currently most TCP stacks use autotuning to control the TCP send and receive buffer size.  Windows XP is old enough that it uses fixed size buffers, later versions (Vista, 7), as well as Linux, BSD, and MacOS stacks autotune by default.  The MLAB nodes are setup to start at 64KB with the ability to scale up to 4MB as required.  This tuning happens automatically.  The NDT server reports the maximum buffer size it scaled to.

In this case the MLab server did not try and go beyond 85KB, it would have continued growing, up to 4 MB, if that would have improved the throughput.  To understand why the server stopped growing you would have to look at the other parameters.  For example, if the connection is really limited by the congestion window then growing the transmit buffer to a larger value would not improve the throughput.  The current autotuning code takes this into account when determining how large to grow the buffers. The server does report what the limiting factor is, and in most cases it is the receiver buffer or the congestion window. There is a serious server resource problem if the transmit buffer is the limiting factor.

### Question 2 ###
A consumer asked for information on how to choose an appropriate client-side buffer size and make best use of the NDT’s measurement (detailed) results.

### Answer 2 ###
The NDT development team is currently developing documentation to describe the operations and reporting messages that the server puts out.  When done, it will be published on this wiki.

If the client is of modern vintage, then it will autotune and the user would not need to set a fixed value.  There are good tuning sites available on the web, search for 'tcp tuning' and 4 of the top 5 hits point to 2 good tuning sites (ESnet and PSC).  These guides will also tell you how to view/change the max value your stack can grow to if autotuning is used.  There is a brief discussion, with links to these 2 tuning sites, on the NDT server page, just scroll down below the applet.