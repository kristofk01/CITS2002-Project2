PROJECT	= duplicates
HEADERS	= $(PROJECT).h
OBJ 	= driver.o store.o duplicates.o strSHA2.o

C11 	= cc -std=c11
FLAGS	= -Wall -Werror -pedantic

$(PROJECT) : $(OBJ)
	$(C11) $(FLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(C11) $(FLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)