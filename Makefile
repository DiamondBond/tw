tw:
	@echo "compiling..."
	gcc tw.c -o tw.o --warn-return-type -lm

clean:
	@echo "cleaning..."
	rm -f tw.o
