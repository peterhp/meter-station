# Project meter-station
A meter station manages all meter monitors and uploads data to remote servers.

## Tasks
* Create a listener on a serial port, and establish a mechanism of message, thus commands from the peer can be captured.
* Create a protocol agent to pack and parse message with modbus protocol in data communications over bus or networks.

## Brief modbus description
* Frame format
```
| addr     | func     | data     | check    |
| -------- | -------- | -------- | -------- |
| 1B       | 1B       | N*1B     | 2B       |
```

* Request message format
```
| domain   | value    |
| -------- | -------- |
| addr     | 01       |
| func     | 03       |
| mid0     | 01       |
| mid1     | 48       |
| mid2     | 02       |
| padd     | 03       |
| crc0     | 85       |
| crc1     | 41       |
```

* Reply message format
```
| domain   | value    |
| -------- | -------- |
| addr     | 01       |
| func     | 03       |
| dlen     | 08       |
| mid0     | 01       |
| mid1     | 48       |
| mid2     | 02       |
| padd     | 03       |
| mdt0     | 01       |
| mdt1     | 09       |
| mdt2     | 03       |
| mdt3     | 05       |
| crc0     | C8       |
| crc1     | F0       |
```
