DIRS = src

all: $(DIRS)

$(DIRS):
	-@cd $@ && make clean && make

run: all
	./bin/pof-viewer ./pof/Capital01.pof

clean:
	-@cd $(DIRS) && make clean

.PHONY: $(DIRS)
