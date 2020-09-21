tw:
	@echo "compiling..."
	gcc tw.c -o tw.o -lm

clean:
	@echo "cleaning..."
	rm -f tw.o
