server:  server.o termulator.o
	gcc server.o termulator.o  -o Server.ok

server.o: server.c
	gcc -lpthread -c server.c


client:  client.o termulator.o
	gcc client.o termulator.o  -o Client.ok

client.o: client.c
	gcc -c client.c


termulator.o: termulator.c termulator.h
	gcc -c termulator.c 


clean:
	rm *.o 

uninstall:
	rm *.o *.ok

intro:
	@echo "\n\n-----TERMULATOR: Emulador de terminal em que cliente envia comandos ao servidor e obtem resposta-----\n\nTodo o programa está adaptado e tem variadíssimos bugs, pelo que não devem ser considerados erros de escrita,\n confirmação de erros de leitura/abertura de pipes ou ficheiros e alguns outros bugs.\n\nSe alguém quiser modificar e melhorar este pequeno projeto de revisão sinta-se à vontade.\n\nREGRAS:\n\n\t./Server.ok admin (ou ./Server.ok [qualquer coisa]) para executar servidor\n\t./Cliente.ok (só pode ser lançado com ./Server.ok em execução) para executar cliente\n\tAconselho apenas um server e um cliente em execução: poderá existir conflito caso contrário\n\nEnorme abraço e bom estudo!\n\nRenato (lol, ok / raoky / cenas fixes e porreiras man)\n\n"

all: intro server client