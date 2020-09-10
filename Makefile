tw:
	@echo "compiling..."
	gcc tw.c -o tw.o -lm

two3:
	@echo "compiling with -O3..."
	gcc -O3 tw.c -o tw.o -lm

clean:
	@echo "cleaning..."
	rm -f tw.o
