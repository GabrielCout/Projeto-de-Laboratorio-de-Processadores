FONTES = boot.s malloc.c main.c

EXEC = kernel.elf
MAP = kernel.map

PREFIXO = arm-none-eabi-
LDSCRIPT = kernel.ld
AS = ${PREFIXO}as
LD = ${PREFIXO}ld
GCC = ${PREFIXO}gcc
OBJ = $(FONTES:.s=.o)
OBJETOS = $(OBJ:.c=.o)
OPTS = -g

#
# Opções do linker
#
LDOPTS = -L/mingw64/lib/gcc/arm-none-eabi/12.2.0/ -L/mingw64/arm-none-eabi/lib
LDOPTS += -lg -lgcc

# Alvo: gerar executável
${EXEC}: ${OBJETOS}
	${LD} -T ${LDSCRIPT} -M=${MAP} -o $@ ${OBJETOS} ${LDOPTS}

# Alvo: montar arquivos em assembler
.s.o:
	${AS} -g -o $@ $<

# Alvo: Compilar arquivos em C
.c.o:
	${GCC} ${OPTS} -c -o $@ $<

# Alvo: limpar tudo
clean:
	rm -f *.o ${EXEC} ${MAP}

# Alvo: iniciar qemu
qemu: ${EXEC}
	@if netstat -na | grep ":1234" > /dev/null ; then\
		echo "qemu já está executando"; \
	else qemu-system-arm -s -M virt & \
	fi

# Alvo: iniciar gdb
gdb: ${EXEC}
	gdb-multiarch -ex "set architecture arm" \
						-ex "target extended-remote :1234" \
						-ex "load" \
						${EXEC}
