CC = g++

RM = /bin/rm -f 

Phase2:  Phase2.o LinkProcessor.o PacketList.o 
	$(CC) $(CFLAGS) -o Phase2 Phase2.o LinkProcessor.o PacketList.o $(LDFLAGS) 
%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean: 
	$(RM) *.o Phase2
