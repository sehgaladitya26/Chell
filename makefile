# Defining target
linkTarget = shell

# Collecting all object files
objects = $(patsubst %.c,%.o,$(wildcard ./*.c))

# Making rebuildabiles
rebuildables = $(objects) $(linkTarget)

# Creating final cmd
$(linkTarget): $(objects)
	gcc -g -o $(linkTarget) $(objects)
	rm $(objects)

# Compiling objects
%.o: %.cpp
	gcc -o $@

#Call to clear all object files
.PHONEY:
clean:
	rm -rf $(objects)