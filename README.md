# Remote Connections
### Connecting to W806
TCP connection on 192.168.4.1:8000
```
sudo socat -v -x PTY,link="/dev/W806",raw,echo=0,group-late=tty,mode=660 TCP4:192.168.4.1:8000,nodelay,tos=7
```
### Connecting to ATTiny427
TCP connection on 192.168.4.1:8001 and 192.168.4.1:8002 for U1 and U2 respectively
```
sudo socat -v -x PTY,link="/dev/UPDI1",raw,echo=0,group-late=tty,mode=660 TCP4:192.168.4.1:8001,nodelay,tos=7
```
```
sudo socat -v -x PTY,link="/dev/UPDI2",raw,echo=0,group-late=tty,mode=660 TCP4:192.168.4.1:8002,nodelay,tos=7
```
# HTTP Upload
### Upload Flash To W806
```
curl --data-binary @flash.bin http://192.168.4.1/upload/w806
```
### Upload Flash To ATTiny427
```
curl --data-binary @flash.bin http://192.168.4.1/upload/attiny/1
```
```
curl --data-binary @flash.bin http://192.168.4.1/upload/attiny/2
```