/* 
 * Start of process to break the web100srv.c file into several
 * pieces.  This should make maintenance easier.  Define
 * the global variables here.
 *
 * Rich Carlson 3/10/04
 * rcarlson@interenet2.edu
 */

#ifndef SRC_WEB100SRV_H_
#define SRC_WEB100SRV_H_

#include "../config.h"

#if HAVE_LIBWEB100 && HAVE_LIBTCPE

// Prefer TCPE unless forced to use Web100
#if defined(FORCE_WEB100)
#define USE_WEB100 1
#define USE_TCPE 0
#else
#define USE_WEB100 0
#define USE_TCPE 1
#endif

#elif HAVE_LIBTCPE

#define USE_WEB100 0
#define USE_TCPE 1

#elif HAVE_LIBWEB100

#define USE_WEB100 1
#define USE_TCPE 0

#else

#define USE_WEB100 0
#define USE_TCPE 0

#endif

#define   _USE_BSD
#include <stdio.h>
#include <netdb.h>
#include <signal.h>
#if USE_WEB100
#include <web100.h>
#endif
#if USE_TCPE
#include <tcpe.h>
#endif
#ifdef HAVE_LIBPCAP
#include <pcap.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#ifdef HAVE_NETINET_IP6_H
#include <netinet/ip6.h>
#endif
#ifdef HAVE_NET_ETHERNET_H
#include <net/ethernet.h>
#endif
#include <arpa/inet.h>
#include <I2util/util.h>

/* move version to configure.ac file for package name */
/* #define VERSION   "3.0.7" */  // version number
#define RECLTH    8192

#define WEB100_VARS 128  // number of web100 variables you want to access
#define WEB100_FILE "web100_variables"  // names of the variables to access
/* Move to logging.h
#define LOG_FACILITY LOG_LOCAL0         // Syslog facility to log at
#define LOGDIR "serverdata"             // directory for detailed snaplog and tcpdump files
*/

/* Location of default config file */
#define CONFIGFILE  "/etc/ndt.conf"

/* hard-coded port values */
/*
#define PORT  "3001"
#define PORT2 "3002"
#define PORT3 "3003"
#define PORT4 "3003"
*/

// Congestion window peak information
typedef struct CwndPeaks {
  int min;  // trough of peak value
  int max;  // maximum peak value
  int amount;  // number of transitions between peaks
} CwndPeaks;

// Options to run test with
typedef struct options {
  u_int32_t limit;  // used to calculate receive window limit
  int snapDelay;  // Frequency of snap log collection in milliseconds
                  // (i.e logged every snapDelay ms)
  char avoidSndBlockUp;  // flag set to indicate avoiding  send buffer
                         // blocking in the S2C test
  char snaplog;  // enable collecting snap log
  char cwndDecrease;  // enable analysis of the cwnd changes (S2C test)
  char s2c_logname[256];  // S2C log file name - size changed to 256
  char c2s_logname[256];  // C2S log file name - size changed to 256
  int compress;  // enable compressing log files
} Options;

typedef struct portpair {
  int port1;
  int port2;
} PortPair;

// Structure defining NDT child process
struct ndtchild {
  int pid;  // process id
  char addr[64];  // IP Address
  char host[256];  // Hostname
  time_t stime;  // estimated start time of test
  time_t qtime;  // time when queued
  int pipe;  // writing end of pipe
  int running;  // Is process running?
  int ctlsockfd;  // Socket file descriptor
  int oldclient;  // Is old client?
  char tests[16];  // What tests are scheduled?
  struct ndtchild *next;  // next process in queue
};

/* structure used to collect speed data in bins */
struct spdpair {
  int family;  // Address family
  u_int32_t saddr[4];  // source address
  u_int32_t daddr[4];  // dest address

  u_int16_t sport;  // source port
  u_int16_t dport;  // destination port
  u_int32_t seq;  // seq number
  u_int32_t ack;  // number of acked bytes
  u_int32_t win;  // window size
  int links[16];  // bins for link speeds
  u_int32_t sec;  // time indicator
  u_int32_t usec;  // time indicator, microsecs
  u_int32_t st_sec;
  u_int32_t st_usec;
  u_int32_t inc_cnt;  // count of times window size was incremented
  u_int32_t dec_cnt;  // count of times window size was decremented
  u_int32_t same_cnt;  // count of times window size remained same
  u_int32_t timeout;  // # of timeouts
  u_int32_t dupack;  // # of duplicate acks
  double time;  // time, often sec+usec from above
  double totalspd;  // speed observed
  double totalspd2;  // running average (spd of current calculated (total speed)
                     // and prior value)
  u_int32_t totalcount;  // total number of valid speed data bins
};

struct web100_variables {
  char name[256];  // key
  char value[256];  // value
} web_vars[WEB100_VARS];

struct pseudo_hdr {  /* used to compute TCP checksum */
  uint64_t s_addr;  // source addr
  uint64_t d_addr;  // destination address
  char pad;  // padding characterr
  unsigned char protocol;  // protocol indicator
  uint16_t len;  // header length
};

int32_t gmt2local(time_t);

/* struct's and other data borrowed from ethtool */
/* CMDs currently supported */
#define ETHTOOL_GSET            0x00000001 /* Get settings. */

#define SPEED_10       10
#define SPEED_100     100
#define SPEED_1000   1000
#define SPEED_10000 10000

/* This should work for both 32 and 64 bit userland. */
struct ethtool_cmd {
  u_int32_t cmd;
  u_int32_t supported; /* Features this interface supports */
  u_int32_t advertising; /* Features this interface advertises */
  u_int16_t speed; /* The forced speed, 10Mb, 100Mb, gigabit */
  u_int8_t duplex; /* Duplex, half or full */
  u_int8_t port; /* Which connector port */
  u_int8_t phy_address;
  u_int8_t transceiver; /* Which tranceiver to use */
  u_int8_t autoneg; /* Enable or disable autonegotiation */
  u_int32_t maxtxpkt; /* Tx pkts before generating tx int */
  u_int32_t maxrxpkt; /* Rx pkts before generating rx int */
  u_int32_t reserved[4];
};

typedef int tcp_stat_var;

struct tcp_vars {
  tcp_stat_var Timeouts;
  tcp_stat_var SumRTT;
  tcp_stat_var CountRTT;
  tcp_stat_var PktsRetrans;
  tcp_stat_var FastRetran;
  tcp_stat_var DataPktsOut;
  tcp_stat_var AckPktsOut;
  tcp_stat_var CurrentMSS;
  tcp_stat_var DupAcksIn;
  tcp_stat_var AckPktsIn;
  tcp_stat_var MaxRwinRcvd;
  tcp_stat_var Sndbuf;
  tcp_stat_var CurrentCwnd;
  tcp_stat_var SndLimTimeRwin;
  tcp_stat_var SndLimTimeCwnd;
  tcp_stat_var SndLimTimeSender;
  tcp_stat_var DataBytesOut;
  tcp_stat_var SndLimTransRwin;
  tcp_stat_var SndLimTransCwnd;
  tcp_stat_var SndLimTransSender;
  tcp_stat_var MaxSsthresh;
  tcp_stat_var CurrentRTO;
  tcp_stat_var CurrentRwinRcvd;
  tcp_stat_var MaxCwnd;
  tcp_stat_var CongestionSignals;
  tcp_stat_var PktsOut;
  tcp_stat_var MinRTT;
  tcp_stat_var RcvWinScale;
  tcp_stat_var SndWinScale;
  tcp_stat_var CongAvoid;
  tcp_stat_var CongestionOverCount;
  tcp_stat_var MaxRTT;
  tcp_stat_var OtherReductions;
  tcp_stat_var CurTimeoutCount;
  tcp_stat_var AbruptTimeouts;
  tcp_stat_var SendStall;
  tcp_stat_var SlowStart;
  tcp_stat_var SubsequentTimeouts;
  tcp_stat_var ThruBytesAcked;
  /* Additional for web10g */
  tcp_stat_var MaxSsCwnd;
  tcp_stat_var MaxCaCwnd;
};

/* web100-pcap */
#ifdef HAVE_LIBPCAP
void init_vars(struct spdpair *cur);
void print_bins(struct spdpair *cur, int monitor_pipe[2]);
void calculate_spd(struct spdpair *cur, struct spdpair *cur2, int port2,
                   int port3);
void init_pkttrace(I2Addr srcAddr, struct sockaddr *sock_addr,
                   socklen_t saddrlen, int monitor_pipe[2], char *device,
                   PortPair* pair, char* direction, int compress);
void force_breakloop();
#endif

/* web100-util */

void get_iflist(void);

#if USE_TCPE
typedef struct tcpe_client tcp_stat_agent;
typedef int tcp_stat_connection;
typedef struct tcpe_data tcp_stat_snap;
/* Not relevent to web10g */
typedef void tcp_stat_group;
/* Log currently unimplemented in web10g */
typedef void tcp_stat_log;
#define tcp_stat_connection_from_socket web10g_connection_from_socket

/* Extra Web10G functions web10g-util.c */
int web10g_find_val(tcpe_data* data, char* name, struct tcpe_val* value);
int web10g_get_val(struct tcpe_client* client, int conn, char* name,
                   struct tcpe_val* value);
int web10g_connection_from_socket(struct tcpe_client* client, int sockfd);
int web10g_get_remote_addr(struct tcpe_client* client, int conn, char* out,
                           int size);

#elif USE_WEB100
typedef web100_agent tcp_stat_agent;
typedef web100_connection* tcp_stat_connection;
typedef web100_snapshot tcp_stat_snap;
/* Group only relevent to web100 */
typedef web100_group tcp_stat_group;
typedef web100_log tcp_stat_log;
#define tcp_stat_connection_from_socket web100_connection_from_socket

#endif

int tcp_stat_autotune(int sock, tcp_stat_agent* agent, tcp_stat_connection cn);
int tcp_stat_init(char *VarFileName);
void tcp_stat_middlebox(int sock, tcp_stat_agent* agent, tcp_stat_connection cn,
                      char *results, size_t results_strlen);
int tcp_stat_setbuff(int sock, tcp_stat_agent* agent, tcp_stat_connection cn,
                   int autotune);/* Not used so no web10g version */
void tcp_stat_get_data_recv(int sock, tcp_stat_agent* agent,
                            tcp_stat_connection cn, int count_vars);
int tcp_stat_get_data(tcp_stat_snap* snap, int testsock, int ctlsock,
                      tcp_stat_agent* agent, int count_vars);

// TODO web10g version of CwndDecrease
int CwndDecrease(tcp_stat_agent* agent, char* logname,
                 u_int32_t *dec_cnt, u_int32_t *same_cnt, u_int32_t *inc_cnt);
int tcp_stat_logvars(struct tcp_vars* vars, int count_vars);

int KillHung(void);
void writeMeta(int compress, int cputime, int snaplog, int tcpdump);
void ipv4mapped_to_ipv6(struct sockaddr_storage * ss);
char *get_remotehost();

/* global variables for signal processing */
sig_atomic_t sig1;
sig_atomic_t sig2;
sig_atomic_t sig13;
sig_atomic_t sig17;
pid_t sig17_pid[256];

#endif  // SRC_WEB100SRV_H_
