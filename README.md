# Пример реализации протокола передачи данных по проводным каналам связи ПД-ПКС

> Версия протокола 1.0.0

Программа demo-pd_pks демонстрирует реализацию обмена с радиостанциями ООО Пульсар-Телеком по протоколу ПД-ПКС v1.0.0 поверх UDP протокола.

## Использование

```bash
Usage: demo-pd_pks -h host -ps udp_port_src -pd udp_port_dest -sid SID
                           [-fec FEC] [-l size]
                           [-w timeout] [-n count] [-v] [-g] dmr_target
dmr_target - DMR number of target (1-16776415)
Options:
        -h              Host gateway address
        -ps             Source UDP port
        -pd             Destination UDP port
        -sid            Service identificator [0...255]
        -fec            Select Forward Error Correction:
                                0-Without correction
                                1-Rate 3/4 Trellis
                                2-Rate 1/2 Block Product Turbo Codes (default)
        -l              Packet size [0...1500], default=32
        -w              Timeout between packets [ms], default=1000 ms, 0-timeout depends on bandwidth
        -n              Count packets, default=0 (infinitely)
        -v              Verbose exchange
        -g              Target is DMR group
```

Пример:

```bash
demo-pd_pks.exe -h 172.20.30.31 -ps 5000 -pd 5000 -w 1000 101

[2023.11.23 17:08:01,499] Start sending to 101, group=N, timeout=1000 ms
[2023.11.23 17:08:01,631] Receive state from gateway:
  vendor:0
  model:1
  SN:1481857293
  MF_DATE:2016.01
  VERSION:5.9 h3
[2023.11.23 17:08:03,115] Packet 1 sent in 589 ms
[2023.11.23 17:08:04,050] Packet 2 sent in 509 ms
[2023.11.23 17:08:04,910] Packet 3 sent in 371 ms
[2023.11.23 17:08:05,900] Packet 4 sent in 346 ms
[2023.11.23 17:08:06,535] Received packet from 101, SID=0 DN=70, len=32
[2023.11.23 17:08:06,840] Packet 5 sent in 270 ms
[2023.11.23 17:08:07,699] Received packet from 101, SID=0 DN=71, len=32
[2023.11.23 17:08:07,842] Packet 6 sent in 274 ms
[2023.11.23 17:08:08,364] Received packet from 101, SID=0 DN=72, len=32
[2023.11.23 17:08:08,720] Packet 7 sent in 156 ms
[2023.11.23 17:08:09,384] Received packet from 101, SID=0 DN=73, len=32
[2023.11.23 17:08:09,720] Packet 8 sent in 143 ms
[2023.11.23 17:08:10,401] Received packet from 101, SID=0 DN=74, len=32
[2023.11.23 17:08:10,730] Packet 9 sent in 152 ms
[2023.11.23 17:08:11,402] Received packet from 101, SID=0 DN=75, len=32
[2023.11.23 17:08:11,716] Packet 10 sent in 150 ms
[2023.11.23 17:08:12,401] Received packet from 101, SID=0 DN=76, len=32
[2023.11.23 17:08:12,734] Packet 11 sent in 162 ms
[2023.11.23 17:08:13,429] Received packet from 101, SID=0 DN=77, len=32
[2023.11.23 17:08:13,726] Packet 12 sent in 157 ms
[2023.11.23 17:08:14,720] Packet 13 sent in 135 ms
```

## Разработка

Для разработки использовались:

- QT версии 5.5.1 MinGW
- QT Creator версии 11.0.1
- CMAKE версии 3.27.5
