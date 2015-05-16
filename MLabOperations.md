

# Operational Notes on the MLab NDT deployment #

## Nodes ##

The following is a list of MLab Nodes (Updated <font color='red'>November 17, 2011</font>)

  * MLab 1 Nodes
    * mlab1.sea01.measurement-lab.org
    * mlab1.nuq01.measurement-lab.org
    * mlab1.lax01.measurement-lab.org
    * mlab1.dfw01.measurement-lab.org
    * mlab1.ord01.measurement-lab.org
    * mlab1.lga01.measurement-lab.org
    * mlab1.lga02.measurement-lab.org
    * mlab1.atl01.measurement-lab.org
    * mlab1.mia01.measurement-lab.org
    * mlab1.lhr01.measurement-lab.org
    * mlab1.ams01.measurement-lab.org
    * mlab1.ams02.measurement-lab.org
    * mlab1.par01.measurement-lab.org
    * mlab1.ath01.measurement-lab.org
    * mlab1.ham01.measurement-lab.org
    * mlab1.syd01.measurement-lab.org
    * mlab1.wlg01.measurement-lab.org
    * mlab1.iad01.measurement-lab.org
    * mlab1.hnd01.measurement-lab.org
  * MLab 2 Nodes
    * mlab2.sea01.measurement-lab.org
    * mlab2.nuq01.measurement-lab.org
    * mlab2.lax01.measurement-lab.org
    * mlab2.dfw01.measurement-lab.org
    * mlab2.ord01.measurement-lab.org
    * mlab2.lga01.measurement-lab.org
    * mlab2.lga02.measurement-lab.org
    * mlab2.atl01.measurement-lab.org
    * mlab2.mia01.measurement-lab.org
    * mlab2.lhr01.measurement-lab.org
    * mlab2.ams01.measurement-lab.org
    * mlab2.ams02.measurement-lab.org
    * mlab2.par01.measurement-lab.org
    * mlab2.ath01.measurement-lab.org
    * mlab2.ham01.measurement-lab.org
    * mlab2.syd01.measurement-lab.org
    * mlab2.wlg01.measurement-lab.org
    * mlab2.iad01.measurement-lab.org
    * mlab2.hnd01.measurement-lab.org
  * MLab 3 Nodes
    * mlab3.sea01.measurement-lab.org
    * mlab3.nuq01.measurement-lab.org
    * mlab3.lax01.measurement-lab.org
    * mlab3.dfw01.measurement-lab.org
    * mlab3.ord01.measurement-lab.org
    * mlab3.lga01.measurement-lab.org
    * mlab3.lga02.measurement-lab.org
    * mlab3.atl01.measurement-lab.org
    * mlab3.mia01.measurement-lab.org
    * mlab3.lhr01.measurement-lab.org
    * mlab3.ams01.measurement-lab.org
    * mlab3.ams02.measurement-lab.org
    * mlab3.par01.measurement-lab.org
    * mlab3.ath01.measurement-lab.org
    * mlab3.ham01.measurement-lab.org
    * mlab3.syd01.measurement-lab.org
    * mlab3.wlg01.measurement-lab.org
    * mlab3.iad01.measurement-lab.org
    * mlab3.hnd01.measurement-lab.org
  * MLab 4 Nodes
    * mlab4.nuq01.measurement-lab.org

## Logging In ##

MLab nodes are build on the [PlanetLab](http://planet-lab.org/) infrastructure and require public key authentication.  The following login command will succeed if your public key has been distributed to all MLab nodes:

```
ssh -i SSH_KEY -p 806 iupui_ndt@HOSTNAME
```

## Running Programs ##

The following programs should be running in the NDT slice of each MLab node:

  * CRON
    * _**crond**_ running as root
  * RSYNC
    * _**/usr/bin/rsync --daemon**_ running as root
  * NDT
    * _**/usr/local/sbin/ndtd -a --snaplog --tcpdump --cputime --multiple --max\_clients=40**_ running as root
  * FakeWWW
    * _**/usr/local/sbin/fakewww**_
  * DONAR
    * _**/bin/bash ./monitor.sh**_ running as root

## Restarting Programs ##

### CRON ###

Cron should be restarted through the init system:

```
sudo /etc/init.d/crond restart
```

### RSYNC ###

RSYNC should always be running, and listens on port _**7999**_.  RSYNC must be started by hand:

```
sudo /usr/bin/rsync --daemon
```

### NDT and FakeWWW ###

NDT should be restarted through the init system, note that restarting NDT will restart FakeWWW as well:

```
sudo /etc/init.d/ndtd restart
```

If there are spurious NDT processes hanging around after a _**stop**_ that have not exited, try _**kill**_ing them:

```
sudo killall ndtd
```

NDT will be listening on port _**3001**_, FakeWWW will listen on port _**7123**_.  NDT may open additional connections on ports _**3002**_ and _**3003**_.

If the init.d script cannot start NDT, try the following steps:

  * See if there are hung instances of _**fakewww**_ or _**ndtd**_.  Kill as necessary
  * Stop DONAR:
```
/etc/init.d/donar stop
```
  * Stop cron:
```
/etc/init.d/crond stop
```
  * Relocate the current _**web100srv.log**_
```
mv /usr/local/ndt/web100srv.log /usr/local/ndt/web100srv.log.20100826
```
  * Restart ndt
```
sudo /etc/init.d/ndtd restart
```
  * Check to be sure things have re-started.  Restart crond and DONAR:
```
/etc/init.d/donar restart
/etc/init.d/crond restart
```

### DONAR ###

DONAR should be restarted through the init system:

```
sudo /etc/init.d/donar restart
```

DONAR is installed in _**/home/iupui\_ndt/donar**_ and has several moving parts:

  * Init script:
```
/etc/init.d/donar
```
  * Shell scripts:
    * Contains health checks for NDT
```
/home/iupui_ndt/donar/node_state.sh
```
    * Contains settings for DONAR
```
/home/iupui_ndt/donar/settings.sh
```
    * Calls DONAR jar file to register the host, reads other two shell scripts
```
/home/iupui_ndt/donar/monitor.sh
```
  * JAR File (Compiled for _**Java 1.5.x**_)
```
/home/iupui_ndt/donar/update/donar.jar
```
  * Key Files (_**MUST**_ be preserved between upgrades!)
```
/home/iupui_ndt/donar/update/nupkey.pub
/home/iupui_ndt/donar/update/nupkey.pvt
/home/iupui_ndt/donar/update/nupkey.seq
```

To check the status of DONAR on all MLAB hosts either run the following one line shell script:

```
for i in sea01 nuq01 lax01 dfw01 ord01 lga01 lga02 atl01 mia01 lhr01 ams01 ams02 par01 ath01 ham01 syd01 wlg01 iad01 hnd01; do echo "Testing \"$i\"";host ndt.iupui.$i.donar.measurement-lab.org;echo;done
```

Or consult one of the DONAR status pages:

  * http://donardns.org/services.php?service=ndt.iupui.donar.measurement-lab.org
  * http://donardns.org/monitor.php

## Convenience Scripts ##

There are several scripts available in _**/home/iupui\_ndt/bin**_ on _**mlab4.nuq01.measurement-lab.org**_.  These are meant to run commands on all nodes, or distribute software.  The following is a list of commands, what they do, and how to use them:

  * MLab Commands (Run on all MLab Nodes)
    * _**mlab.cmd**_ - Run an arbitrary command on all nodes:
      * The following command will restart NDT and get a ps listing:
```
mlab.cmd 'sh -c "sudo /etc/init.d/ndt restart;sleep 2;ps axw | grep ndt;"'
```
    * _**mlab.df**_ - Run _**df**_ on all nodes
    * _**mlab.du**_ - Run _**du**_ on all nodes
    * _**mlab.ping**_ - Ping all nodes from _**mlab4.nuq01**_
    * _**mlab.ps**_ - Run _**ps**_ on all nodes
    * _**mlab.psc**_ - Run _**ps**_ on all nodes and return only a count of ndtd from the results
    * _**mlab.scp**_ - Copies a file from _**mlab4.nuq01**_ to the _**/home/iupui\_ndt/tmp**_ directory on all remote machines.
      * The following command will copy the NDT daemon (web100srv):
```
mlab.scp /usr/local/sbin/web100srv
```
  * MLabX Command (substitute 1, 2, or 3 for _**X**_ to run on specific MLab sets)
    * _**mlabX.cmd**_ - Run an arbitrary command on all _**X**_ nodes:
      * The following command will restart NDT and get a ps listing:
```
mlabX.cmd 'sh -c "sudo /etc/init.d/ndt restart;sleep 2;ps axw | grep ndt;"'
```
    * _**mlabX.copy**_ - Copies a file from _**mlab4.nuq01**_ to the _**/home/iupui\_ndt/tmp**_ directory on all _**X**_ nodes.
      * The following command will copy the NDT daemon (web100srv):
```
mlabX.copy /usr/local/sbin/web100srv
```

## SSH Passphrase Errors ##

In most cases the _Convenience Scripts_ should work without needing to re-enter the passphrase since an SSH agent is always running.  If a message like this is seen:

```
[iupui_ndt@mlab4 ~]$ mlab.df
Initialising new SSH agent...
succeeded
Enter passphrase for /home/iupui_ndt/.ssh/id_rsa:
```

The environment must be reset.  E.g:

```
rm -f ~/.ssh/environment
```

## Deleting files that have been successfully sync'd (Clearing disk space) ##

  * check GB free via CoMon MLab Status (sort key: GB Free) - http://comon.cs.princeton.edu/status/tabulator.cgi?sort=28&limit=50&account=mlab
  * ssh to the machine
  * $ ./get\_safe\_delete\_date.py rsync://ndt.iupui.```hostname```:7999/ndt-data (will return a date such as 2010-04-03)
  * $ cd /usr/local/ndt/serverdata/2010/04 (will change to the April 2010 directory)
  * $ ls (displays the log directories, one for each day of the month, example 02 03 04 05)
  * $ sudo rm -frd 02 03 (deletes specific directories and all of its contents)


## Miscellaneous Notes ##

### Common Problems ###

  * A full disk is one of the more common reasons that ndtd will fail to start. This will show up in one of two ways.
    * The nagios ndt check will fail, this only tests the permanent listening ports. netstat -ln on logging in will show nothing on port 3001. Restarting ndt and re-checking netstat -ln will continue to show nothing listening on port 3001. Be sure to try df -h and clean up as needed, a full disk will often cause ndtd to crash immediately or soon after a restart.
    * The nagios mlab\_ndt check will fail, this uses the web100clt program and will detect time outs and problems with the actual test if the permanent listening ports are up. netstat -ln is not useful in this instance as a crash or hang of this sort may still show port 3001 as listening. Running the web100ctl bin with the -n parameter, using the affected host as the argument, will confirm this crash/hang if the test gets to c2s and/or s2c but reports a failure of either or both of those stages. Be sure to check disk usage (df -h) clearing up stale test data files as needed. Usually restarting ntdt, using the init system, will restore the server process to operation; confirm with web100clt.
  * If stopping and manually killing any left over ntdt processes revealed by ps doesn't work, try restarting the vserver. This is less disruptive than restarting the entire server node. These instructions assume root access, the Planet Lab tools may also support those with site\_admin access to perform a similar set of oeprations.
    * Log into the root partition of the server node.
    * vserver iupui\_ndt stop
    * vserver iupui\_ndt start

### SSH ###

Use port _**806**_ to connect via SSH to MLab nodes.

### CRON ###

The standard cron location, _**/etc/crontab**_, does not appear to work.  Instead use the crontab program:

```
crontab -u USER -e
```

To simply list what is in the file right now, run:

```
crontab -u USER -l
```

After editing, this will store a personal crontab file in _**/var/spool/cron**_ (e.g. either _**/var/spool/cron/iupui\_ndt**_ or _**/var/spool/cron/root**_).  As an FYI crontab entries should resemble this:

```
*/15 * * * * /home/iupui_ndt/some_script.sh
```

The 6 fields broken down:

  * _**Minutes**_
  * _**Hours**_
  * _**Day of Month**_
  * _**Month**_
  * _**Day of Week**_
  * _**Command**_

Note that each time entry can be a static value (_**01**_, minute 1 of a new hour/day/month), a repeating interval (_**/15**_, every 15 minutes), or open (use the star).

## Last Updated ##

$Id$