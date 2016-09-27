#  captcha.mak - Compile captcha.c   Version 1.0.0
#  Copyright (C) 2016 aquila62 at github.com

#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to:

   #  Free Software Foundation, Inc.
   #  59 Temple Place - Suite 330
   #  Boston, MA 02111-1307, USA.

OBJ=captcha.o \
	elcginit.o \
	elcg.o \
	elcgunif.o \
	elcgint.o \
	elcgpwr.o

CC=gcc

CFLAGS=-c -Wall -O2

LDFLAGS=

captcha:			$(OBJ)
		$(CC) -Wall -O2 $(OBJ) -o captcha $(LDFLAGS)

captcha.o:			captcha.c
		$(CC) $(CFLAGS) captcha.c

elcginit.o:			elcginit.c
		$(CC) $(CFLAGS) elcginit.c

elcg.o:				elcg.c
		$(CC) $(CFLAGS) elcg.c

elcgunif.o:			elcgunif.c
		$(CC) $(CFLAGS) elcgunif.c

elcgint.o:			elcgint.c
		$(CC) $(CFLAGS) elcgint.c

elcgpwr.o:			elcgpwr.c
		$(CC) $(CFLAGS) elcgpwr.c

clean:
		rm -f captcha $(OBJ)
