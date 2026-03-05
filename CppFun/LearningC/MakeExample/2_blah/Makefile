#https://makefiletutorial.com/

blah.exe: blah.o
	gcc -o blah.exe blah.o

blah.o: blah.c
	gcc -c blah.c

blah.c:
	echo "blah.c does not exist."

clean:
	rm -f blah.o
	rm -f blah.exe

# blah.c is a pre-requisite --> make will run if blah.c is newer than blah.
