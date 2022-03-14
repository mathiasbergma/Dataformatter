# -*- MakeFile -*-
# target: dependencies
# 	 action

# kommando generer en executable fil
CAN_read: CAN_read.o
	gcc CAN_read.o -o CAN_read -l m
	
# kommando 'gcc -c i2c.c' generer en .o fil 'i2c.o'
CAN_read.o: CAN_read.c
	gcc -c CAN_read.c
	
# fjerne alle .o filer og den executable fil
clean:
	rm *.o CAN_read

