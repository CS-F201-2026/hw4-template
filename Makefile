# A4: AI-assisted instructor scaffolding.
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS =
OBJECTS = main.o readings.o report.o

.PHONY: all clean test sanitize
all: sensor_report

sensor_report: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

main.o: main.c readings.h report.h
readings.o: readings.c readings.h
report.o: report.c report.h readings.h

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: sensor_report
	python3 tests/test_public.py ./sensor_report

sanitize:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -fno-pie" LDFLAGS="-fsanitize=address,undefined -no-pie" test

clean:
	rm -f $(OBJECTS) sensor_report
