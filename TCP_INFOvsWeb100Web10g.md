# Introduction #
Investigate the possible use of TCP\_INFO instead of web100/web10g.


# Details about TCP\_INFO #
## Structure of TCP\_INFO: ##
```
struct tcp_info
{
u_int8_t	tcpi_state; 
u_int8_t	tcpi_ca_state;
u_int8_t	tcpi_retransmits;
u_int8_t	tcpi_probes;
u_int8_t	tcpi_backoff;
u_int8_t	tcpi_options;
u_int8_t	tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;

u_int32_t	tcpi_rto;
u_int32_t	tcpi_ato;
u_int32_t	tcpi_snd_mss;
u_int32_t	tcpi_rcv_mss;

u_int32_t	tcpi_unacked;
u_int32_t	tcpi_sacked;
u_int32_t	tcpi_lost;
u_int32_t	tcpi_retrans;
u_int32_t	tcpi_fackets;

/* Times. */
u_int32_t	tcpi_last_data_sent;
u_int32_t	tcpi_last_ack_sent;	/* Not remembered, sorry.  */
u_int32_t	tcpi_last_data_recv;
u_int32_t	tcpi_last_ack_recv;

/* Metrics. */
u_int32_t	tcpi_pmtu;
u_int32_t	tcpi_rcv_ssthresh;
u_int32_t	tcpi_rtt;
u_int32_t	tcpi_rttvar;
u_int32_t	tcpi_snd_ssthresh;
u_int32_t	tcpi_snd_cwnd;
u_int32_t	tcpi_advmss;
u_int32_t	tcpi_reordering;
};
```
You can find this structure in:
```
/usr/include/netinet/tcp.h
```

## Description of variables: ##
```
tcpi_state;	/* Possible value:
		TCP_ESTABLISHED = 1,
		TCP_SYN_SENT,
		TCP_SYN_RECV,
		TCP_FIN_WAIT1,
		TCP_FIN_WAIT2,
		TCP_TIME_WAIT,
		TCP_CLOSE,
		TCP_CLOSE_WAIT,
		TCP_LAST_ACK,
		TCP_LISTEN,
		TCP_CLOSING */

tcpi_ca_state;	/* Congestion control state */
tcpi_retransmits;	/* Number of unrecovered [RTO] timeouts */
tcpi_probes;	/* Unanswered 0 window probes */
tcpi_backoff;	/* Backoff */
tcpi_options;	
tcpi_snd_wscale	/* Window scaling received from sender */
tcpi_rcv_wscale	/*Window scaling to send to receiver*/
tcpi_rto;	/* Retransmit timeout */
tcpi_ato;	/* Predicted tick of soft clock */
tcpi_snd_mss;	/* Cached effective mss, not including SACKS*/
tcpi_rcv_mss;	/* MSS used for delayed ACK decisions */
	
tcpi_unacked;	/* Packets which are "in flight" */
tcpi_sacked;	/* SACK'd packets */
tcpi_lost;	/* Lost packets */
tcpi_retrans;	/* Retransmitted packets out */
tcpi_fackets;	/* FACK'd packets */
	
tcpi_last_data_sent;	/* now – lsndtime (lsndtime → timestamp of last sent data packet (for restart window)) */
tcpi_last_ack_sent;	/* Not remembered, sorry.  */
tcpi_last_data_recv;	/* now – isck_ack.lrcvtime (isck_ack ->Delayed ACK control data; lrcvtime → timestamp of last received data packet) */
tcpi_last_ack_recv;	/* now - rcv_tstamp  (rcv_tstamp → timestamp of last received ACK (for keepalives)) */
	
tcpi_pmtu;	/* Last pmtu seen by socket (Path Maximum Transmission Unit) */
tcpi_rcv_ssthresh;	/* slow start size threshold for  receiving (Current window clamp) */
tcpi_rtt;	/* Smoothed Round Trip Time (SRTT) */
tcpi_rttvar;	/* Medium deviation */
tcpi_snd_ssthresh;	/* Slow start size threshold for sending */
tcpi_snd_cwnd;	/* Sending congestion window */
tcpi_advmss;	/* Advertised Maximum Segment Size (MSS) */
tcpi_reordering;	/* Indicates the amount of reordering. Packet reordering metric */
	
tcpi_rcv_rtt;	/* Receiver side RTT estimation */
tcpi_rcv_space;	/* Receiver queue space */
	
tcpi_total_retrans;	/* Total retransmits for entire connection */
```

## Useful information about the TCP\_INFO: ##
TCP\_INFO structure is filled in the tcp\_get\_info() function.You can find it in kernel source:
```
/lib/modules/`uname -r`/build/net/ipv4/tcp.c 
```
Most of the data is copied from tcp\_sock and inet\_connection\_sock structure.<br>
tcp_sock struct you can find in:<br>
<pre><code>/lib/modules/`uname -r`/build/include/linux/tcp.h<br>
or<br>
/lib/modules/`uname -r`/build/include/net/tcp.h<br>
</code></pre>
inet_connection_sock structure is in:<br>
<pre><code>/lib/modules/`uname -r`/build/include/net/inet_connection_sock.h<br>
</code></pre>

<h1>Comparison with Web100/Web10g</h1>
first column: variables get by UI<br>
second column: equivalent variables in Web100/10g or how it's calculated<br>
third column: equivalent variables in TCP_INFO<br>

<table><thead><th> UI 		</th><th> Web100/10g variables </th><th> TCP_INFO </th></thead><tbody>
<tr><td> status 	</td><td>	-	</td><td>	-	</td></tr>
<tr><td> diagnosis	</td><td>  		</td><td>		</td></tr>
<tr><td> errmsg 	</td><td>	- 	</td><td>	-	</td></tr>
<tr><td> host 	</td><td> 	-	</td><td>	-	</td></tr>
<tr><td> ClientToServerSpeed 	</td><td> 	-	</td><td>	-	</td></tr>
<tr><td> ServerToClientSpeed 	</td><td> 	-	</td><td>	-	</td></tr>
<tr><td> avgrtt 	</td><td> SumRTT / CountRTT </td><td> tcpi_rtt? </td></tr>
<tr><td> Jitter 	</td><td> MaxRTT – MinRTT </td><td> tcpi_rttvar? </td></tr>
<tr><td> CurRwinRcvd </td><td> CurRwinRcvd	</td><td>		</td></tr>
<tr><td> MaxRwinRcvd </td><td> MaxRwinRcvd	</td><td>		</td></tr>
<tr><td> loss </td><td> CongestionSignals / PktsOut </td><td> tcpi_lost </td></tr>
<tr><td> MinRTT </td><td> MinRTT	 	</td><td> 	- 	</td></tr>
<tr><td> MaxRTT </td><td> MaxRTT	 	</td><td> 	- 	</td></tr>
<tr><td> Waitsec </td><td> (CurRTO <code>*</code> Timeouts) / 1000 </td><td>	 </td></tr>
<tr><td> CurRTO 	</td><td> CurRTO	</td><td>		</td></tr>
<tr><td> SACKsRcvd 	</td><td> SACKsRcvd	</td><td> tcpi_sacked 	</td></tr>
<tr><td> cwndtime 	</td><td> SndLimTimeCwnd / (SndLimTimeRwin + SndLimTimeCwnd + SndLimTimeSender)		</td><td>		</td></tr>
<tr><td> rwintime 	</td><td> SndLimTimeRwin  / (SndLimTimeRwin + SndLimTimeCwnd + SndLimTimeSender)		</td><td>		</td></tr>
<tr><td> optimalRcvrBuffer </td><td> MaxRwinRcvd <code>*</code> KILO_BITS</td><td> 	</td></tr>
<tr><td> DupAcksIn </td><td> DupAcksIn	</td><td> 		</td></tr></tbody></table>

<br>
<h3>Web100/10g variables used in tests:</h3>
<table><thead><th> middlebox </th><th> CurMss, WinScaleSent, WinScaleRcvd, SumRTT, CountRTT, MaxRwinRcvd </th></thead><tbody>
<tr><td> sfw </td><td> MaxRTT, MaxRTO </td></tr></tbody></table>

<br>
In server are also test for wireless link and DSL/Cable modem link which is dependent from SndLimTransRwin, SndLimTransCwnd and SndLimTransSender variables (and more).<br>
<br>
<h2>Function work without Web100/10g</h2>
<ol><li>C2S throughput test<br>
</li><li>S2C throughput test<br>
</li><li>Detect S2C & C2S throughput test: Packet queuing<br>
</li><li>Simple firewall test (after little change in calculate testTime)<br>
</li><li>Connection type (in client)<br>
Rest is depend from Web100/10g specific variables.</li></ol>

<h3>Graph of the variables</h3>
<img src='http://ndt.googlecode.com/svn/wiki/TCP_INFOvsWeb100Web10g.images/graph.jpg' />

<a href='http://ndt.googlecode.com/svn/wiki/TCP_INFOvsWeb100Web10g.images/graph.jpg'>Download graph</a>