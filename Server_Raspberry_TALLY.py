#!/usr/bin/python
# -*- coding: utf-8 -*-
from getmac import get_mac_address
import socket, struct
from time import sleep
import RPi.GPIO as GPIO
import subprocess
#### 50.100 - 50.110

sleep(30)

UDP_IP = "192.168.50.100"
UDP_PORT = 4213

MESSAGE_A = "A"
BUF_IP = [1,2,3,4,5,6,7,8,9,10]

PIN_PGM = [26, 19, 13 , 6, 5]
PIN_PRE = []

ip2int = lambda ipstr: struct.unpack('!I', socket.inet_aton(UDP_IP))[0]
int2ip = lambda n: socket.inet_ntoa(struct.pack('!I', n))

INT_IP = ip2int(UDP_IP)

def restartWIFI():
  proc = subprocess.Popen('sudo ifdown wlan0', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  proc.wait()
  sleep(13)
  proc = subprocess.Popen('sudo killall hostapd', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  proc.wait()
  sleep(2)
  proc = subprocess.Popen('sudo ifup wlan0', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  proc.wait()
  sleep(2)
  return True

def setupPi():
  GPIO.setmode(GPIO.BCM)
  for pin in PIN_PGM:
      GPIO.setup(pin, GPIO.IN)

  return True

def scanIP():
  global MESSAGE_A, UDP_IP, UDP_PORT, INT_IP, BUF_IP
  BUF_IP = []
  for inc in range(11):
     IIP = get_mac_address(ip=int2ip(INT_IP+inc))
     if (IIP[0] <> '0'):
         BUF_IP.append(inc)
  print BUF_IP
  return True


def ESPsend(mes, sock):
  global MESSAGE_A, UDP_IP, UDP_PORT, INT_IP, BUF_IP
  for inc in BUF_IP:
      sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      IIP = get_mac_address(ip=int2ip(INT_IP+inc))
#      sock.setblocking(0)
#      print socket.gethostname()
      sock.sendto(mes, (int2ip(INT_IP+inc), UDP_PORT+100+inc))
      print UDP_PORT, inc, UDP_PORT+100+inc
      sock.close()
  return True


def sendUDPall(mes):
  global MESSAGE_A, UDP_IP, UDP_PORT, INT_IP
  for inc in range(11):
      sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      sock.sendto(mes, (int2ip(INT_IP+inc), UDP_PORT))
      sleep(0.2)
      #sock.settimeout(0.2)
      #sock.sendto(mes, (int2ip(INT_IP+inc), UDP_PORT))
      #sock.settimeout(0.2)
  return True

def readGPIO():
  STATUS = 0

  for pin in PIN_PGM:
      B = GPIO.input(pin)
      STATUS = STATUS | B
      STATUS = STATUS << 1
  return STATUS

##**************************************************************************************
def main():
  setupPi()
  restartWIFI()
  global MESSAGE_A, UDP_IP, UDP_PORT, INT_IP
  print MESSAGE_A
  print UDP_IP
  print UDP_PORT

##-------------------------------------
  count = 0
  mess = readGPIO()
  sockets = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  scanIP()
  while True:
    mes = readGPIO()
    if (mess <> mes):
        #sendUDPall( chr(mes))
        ESPsend(chr(mes), sockets)
        mess = mes
        count = 0
        print "mes", mes
        sleep(0.2)
    else:
        sleep(0.2)
        count +=1
        if count > 3:
           mess = 1
           scanIP()

  return True


##************************************************************************************
if __name__ == "__main__":
    main()
        
