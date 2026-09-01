.PHONY: all lab4 lab5 lab6 evidence strict clean

all: lab4 lab5 lab6

lab4:
	$(MAKE) -C lab4 all

lab5:
	$(MAKE) -C lab5 all

lab6:
	$(MAKE) -C lab6 all

evidence:
	$(MAKE) -C lab4 evidence
	$(MAKE) -C lab5 evidence
	$(MAKE) -C lab6 evidence

strict:
	$(MAKE) -C lab4 strict
	$(MAKE) -C lab5 strict
	$(MAKE) -C lab6 strict

clean:
	$(MAKE) -C lab4 clean
	$(MAKE) -C lab5 clean
	$(MAKE) -C lab6 clean
