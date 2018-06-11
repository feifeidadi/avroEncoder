CC = g++
CFLAGS = -Wall -I../inc -DDEBUG -lavrocpp
AVRO_COMPILER = avrogencpp

OBJECTS = avroEncoder_test.o
BINARY = avroEncoder_test
CPX_HEADER_FILE = cpx.h

all: $(CPX_HEADER_FILE) $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BINARY)

$(OBJECTS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(CPX_HEADER_FILE): %.h: ../schema/cpx.json
	$(AVRO_COMPILER) -n c -i $< -o $@
	mv $@ ../inc/

clean:
	rm -f *.o *.bin ../inc/$(CPX_HEADER_FILE) $(BINARY) 
