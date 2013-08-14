DIRS = src

all: $(DIRS)

$(DIRS):
	-@cd $@ && make clean && make

run: all
	./bin/pof-viewer

clean:
	-@cd $(DIRS) && make clean

.PHONY: $(DIRS)
