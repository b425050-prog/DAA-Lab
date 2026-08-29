.PHONY: all lab4 lab5 evidence strict clean

all: lab4 lab5

lab4:
	$(MAKE) -C lab4 all

lab5:
	$(MAKE) -C lab5 all

evidence:
	$(MAKE) -C lab4 evidence
	$(MAKE) -C lab5 evidence

strict:
	$(MAKE) -C lab4 strict
	$(MAKE) -C lab5 strict

clean:
	$(MAKE) -C lab4 clean
	$(MAKE) -C lab5 clean
