# Audio Routing Components - Overview

These follow pretty simple rules. Simply that an audio destination is not a single router destination but six sequential destinations that may be routed in parallel to make a 5.1.

A coherent route is where the six elements of a source match the six tallies of a destination.

Ports are configured using database 6 and 7 using the following convention:

|Entry|Use|
|-----|---|
|L|Left|
|R|Right|
|M|Mono|
|1<br/>2<br/>3<br/>4<br/>5<br/>6|Surround|

For example:
```
[Database_7]
0017=L
0018=R
0019=L
0020=R
0021=M
0022=M
0023=M
0024=M
0025=1
0026=2
0027=3
0028=4
```
The audio components make allowances for incompatible source/destination types.

These components were written assuming a very large router spanning multiple BNCS **infodriver** devices - i.e. dests 1 to 4096 are on the first device, dest 4097 is dest 1 on a second device etc. etc. Sources are assumed to be a flat list of up to 65535 sources.

|Component|Description|
|--|--|
|[Audio Take](audio_take/docs/audio_take.html)|Audio Take Button|
|[Audio Dest](audio_dest/docs/audio_dest.html)|Audio Destination Button|
|[Audio List](audio_list/docs/audio_list.html)|Audio List Control|
