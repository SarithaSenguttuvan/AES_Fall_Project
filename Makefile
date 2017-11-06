all: main.c temp.c light.c generic.c log.c
	gcc main.c log.c generic.c light.c temp.c I2C_temp.c I2C_light.c -pthread -lrt -g

clean:
	$(RM) a.out
