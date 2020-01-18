# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
# SRIPATH ?= /data/DSP_HW3/103_2/srilm-1.5.10
# MACHINE_TYPE ?= i686-m64
SRIPATH ?= $(shell pwd)/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
RESULT1 = result1
RESULT2 = result2
.PHONY: all clean map run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

run: $(RESULT2)

$(RESULT1): disambig
	@echo "Check directory $@";
	@if [ ! -d $@ ]; then \
		mkdir $@; \
	fi;
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		./$< -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > $@/$$i.txt; \
	done;

$(RESULT2): $(TARGET)
	@echo "Check directory $@";
	@if [ ! -d $@ ]; then \
		mkdir $@; \
	fi;
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		./$< -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > $@/$$i.txt; \
	done;

map: 
	python3 mapping.py $(FROM) $(TO)

clean:
	$(RM) $(OBJ) $(TARGET)

cleanest:
	$(RM) $(OBJ) $(TARGET) 
	$(RM) -r $(RESULT2) 
	$(RM) lm.cnt bigram.lm
	$(RM) $(TO)
