# -*- MakeFile -*-
# target: dependencies
# 	 action

# kommando generer en executable fil
CAN_convert_data: CAN_convert_data.o
	gcc CAN_convert_data.o -o CAN_convert_data -l m
	
# kommando 'gcc -c i2c.c' generer en .o fil 'i2c.o'
CAN_convert_data.o: CAN_convert_data.c
	gcc -c CAN_convert_data.c
	
# fjerne alle .o filer og den executable fil
clean:
	rm *.o CAN_convert_data

