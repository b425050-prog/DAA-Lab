.PHONY: all lab4 lab5 lab6 lab7 lab8 lab9 evidence strict clean

all: lab4 lab5 lab6 lab7 lab8 lab9

lab4:
	$(MAKE) -C lab4 all

lab5:
	$(MAKE) -C lab5 all

lab6:
	$(MAKE) -C lab6 all

lab7:
	$(MAKE) -C lab7/lab7 all

lab8:
	$(MAKE) -C lab8 all

evidence:
	$(MAKE) -C lab4 evidence
	$(MAKE) -C lab5 evidence
	$(MAKE) -C lab6 evidence
	$(MAKE) -C lab7/lab7 evidence
	$(MAKE) -C lab8 evidence
	$(MAKE) -C lab9 evidence

strict:
	$(MAKE) -C lab4 strict
	$(MAKE) -C lab5 strict
	$(MAKE) -C lab6 strict
	$(MAKE) -C lab7/lab7 strict
	$(MAKE) -C lab8 strict
	$(MAKE) -C lab9 strict

clean:
	$(MAKE) -C lab4 clean
	$(MAKE) -C lab5 clean
	$(MAKE) -C lab6 clean
	$(MAKE) -C lab7/lab7 clean
	$(MAKE) -C lab8 clean
	$(MAKE) -C lab9 clean

lab9:
	$(MAKE) -C lab9 all
