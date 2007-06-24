#!/usr/bin/python

import network
import threading
import sys
import time
from communication import *


class TFWclient:
    def __init__(self, host, port):
        self._host = host
        self._port = port
        self._working = 1
        self._threads = []

    def connect(self):
        print "Connecting to", self._host, self._port
        self._sock = network.connect(self._host, self._port)
        self._sock.setblocking(True)
        self._sock.settimeout(None)
        print "   >>> Done"

    def logIn(self):
        print "Logging in..."
        self._sock.sendall("C")
        print "   >>> Done"

    def operate(self):
        while 1:
            msg = recvMsg(self._sock)
            if len(msg) > 0:
                print msg
                if msg == 'OPEN':
                    print 'opening...'
                    s = network.listen('::', None)
                    sendMsg(self._sock, str('OPENED:'+str(s.getsockname()[1])))
                    print 'accepting...'
                    conn, addr = s.accept()
                    print 'starting...'
                    # Start garbage thread
                    self._working = 1
                    thread = threading.Thread(target=self.garbageListener, args=(conn, addr))
                    self._threads.append(thread)
                    thread.start()
                elif msg.startswith('TRAFFIC/'):
                    list = msg[8:].split('/')
                    thost = list[0]
                    tport = list[1]
                    tspeed = list[2]
                    print 'host:', thost, 'port:', tport, 'speed:', tspeed
                    # Start worker thread
                    self._working = 1
                    thread = threading.Thread(target=self.worker, args=(thost, tport, tspeed))
                    self._threads.append(thread)
                    thread.start()
                elif msg == 'STOP':
                    print 'stopping...'
                    self._working = 0
                    for thread in self._threads:
                        thread.join()
                        self._threads.remove(thread)
                    print '...done!'
                else:
                    print "Unrecognized command"
                    self._working = 0
                    sys.exit()
            else:
                print "Connection has been closed"
                self._working = 0
                sys.exit()

    def garbageListener(self, conn, addr):
        try:
            print 'reading...'
            while self._working:
                data = conn.recv(4096)
                if len(data) == 0:
                    break
        except:
            pass
        print 'closing connection...'
        try:
            conn.close()
        except:
            pass

    def worker(self, host, port, speed):
        socket = network.connect(host, port)
    
        rspeed = float(int(speed) * 128)
        
        data = '12345678901234567890123456789012345678901234567890123456789012345678901234567890'
        delay = 1.0 / (rspeed / 80.0)
        
        print 'delay:', delay
        
        while self._working:
            socket.sendall(data)
            time.sleep(delay)

        print 'stop writing...'
        try:
            socket.close()
        except:
            pass


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage:", sys.argv[0], "<host> <port>"
        sys.exit()
    client = TFWclient(sys.argv[1], sys.argv[2])
    client.connect()
    client.logIn()
    client.operate()
