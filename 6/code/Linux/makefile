CCBIN=gcc 


CC=${CCBIN}
LD=${CCBIN}


SRCS=main.c


OBJS=${SRCS:.c=.o}
PROG = linux


all:$(PROG) 


$(PROG) : $(OBJS)
	$(LD) -pthread -Wall $(OBJS) -o $(PROG)  
%.o : %.c
	$(CC) -pthread -Wall -c -o $@ $< -w


clean:
	rm -rf ${OBJS} $(PROG)
