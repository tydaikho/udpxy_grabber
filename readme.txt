Reverced udpxy. Grab stream from tcp(http) to udp

Copyright 20014 Alexandr Barabash s1z@ukr.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
_____________________________________________________________________

How to use:
1. need root permit
 sudo -s
2. clone git
 git clone https://github.com/skal1ozz/udpxy_grabber
3. enter clone dir
 cd ./udpxy_grabber
4. install it
 make install clean
5. run it
 grabber 192.168.1.1 80 /udpxy/238.1.1.1:1234 238.1.1.1 1234
