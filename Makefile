DIRS = src

all: $(DIRS)

$(DIRS):
	-@cd $@ && make clean && make

clean:
	-@cd $(DIRS) && make clean

.PHONY: $(DIRS)
