Alunos: Diogo Junior, Ilê Caian Gums, Ramna Sidharta

O arquivo de teste utilizado é nomeado de app/p3_timestamp.cc. A configuração
de logs de warning está ativada para essa aplicação, assim executando ela normalmente
com `make APPLICATION=p3_timestamp` mostrará nos consoles do recebtor e transmissor
os seus respectivos tempos `Alarm::_elapsed()` e timestamps.

Para essa entrega, executamos a sincronização com o tempo do master (fazendo uma
checagem, no receptor, de endereço transmissor) a cada duas mensagens recebidas
dele.

Timestamps são agora sempre adicionados ao Header do DIRP, porém apenas utilizados
em mensagens recebidas do master.

O tempo enviado pelo master é literalmente seu valor para `Clock::now()` no momento
do envio. Isso significa que não implementamos a obtenção do tempo de um "servidor
de tempo", através de comunicação por porta serial com um QEMU executando uma aplicação
python. Tentamos implementar essa primeira etapa, contudo não conseguimos tê-la funcionando.

O arquivo do servidor python é `app/server.py`. A aplicação `app/timestamp_test.cc`
foi o código utilizado para os testes de comunicação com o servidor.

Para a sincronização temporal, utilizamos o algorítmo NTP simplificado e adaptado
para o nosso cenário. O offset é calculado a partir da média das diferenças dos
relógios do transmissor e receptor para as duas mensagens recebidas anteriormente.
Alarm::_elapsed é então alterado de acordo com esse offset.
