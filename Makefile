#CC=arm-linux-gnueabihf-gcc 
CC=arm-none-linux-gnueabihf-gcc
#STRIP=arm-linux-gnueabihf-strip 
STRIP=arm-none-linux-gnueabihf-strip
#DEBUG=-fsanitize=address -g 
CCFLAGS=-Ofast -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations
LDFLAGS=-lm 
SRC=main.c misc.c ini.c 
SRC_UTL=mlinkutil.c misc.c serial2.c tcpsock.c
all : 
	$(CC) $(DEBUG) $(SRC) $(CCFLAGS) -o bbvmode $(LDFLAGS)
	$(STRIP) bbvmode
	
x86 : 
	gcc $(DEBUG) $(SRC) -o bbvmode_x86 $(LDFLAGS)
	strip bbvmode_x86
	
clean:
	rm -f bbvmode bbvmode_x86 *~ *.orig DEADJOE
	
	


