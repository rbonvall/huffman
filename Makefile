CC = g++
CCFLAGS = -Wall -O2

all: encoder decoder

encoder: encoder.cpp
	$(CC) $(CCFLAGS) $< -o $@

decoder: decoder.cpp
	$(CC) $(CCFLAGS) $< -o $@

clean:
	rm encoder decoder out.txt out.txt.decoded

.PHONY: clean all

