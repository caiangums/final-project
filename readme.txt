Alunos: Diogo Junior, Ilê Caian Gums, Ramna Sidharta

Como não conseguimos finalizar a implementação do driver, realizamos os testes considerando a NIC PCNet32. O relatório explicando a implementação do driver se encontra no pdf em anexo (Relatorio_Implementacao_Driver.pdf).
Em anexo a esse tarball seguem apenas os arquivos criados ou alterados em relação a revisão 5501 do EPOS. O projeto completo se encontra no endereço: https://github.com/caian-gums/final-project

# Desenvolvimento

Foi criado um protocolo simplificado (DIR_Protocol), que funciona como um communicator, com a função de desacoplar a comunicação com a NIC e o tratamento da transferência de dados do escopo da aplicação.
Esse protocolo foi acrescentado no arquivo `include/communicator.h` e inclui uma pequena alteração no arquivo `include/network/ethernet.h`

# Arquivos de Testes

- Teste demonstrando a operação do driver com DMA, tanto para recepção quanto para transmissão:
Foram criados dois arquivos de teste separados, com suas respectivas traits:
`dma_test1.cc` `dma_test1_traits.h`
`dma_test2.cc` `dma_test2_traits.h`
O primeiro arquivo realiza a execução de uma tarefa CPU-bound (utilizado cálculo de fibonacci como exemplo), seguida de uma tarefa com uso intensivo da rede (utilizando troca de mensagens entre dois QEMUs). O tempo de cada tarefa é calculado.
O segundo arquivo realiza a execução das duas tarefas concorrentemente, com a criação de uma Thread para cada. Como o tempo total calculado é levemente maior do que o máximo do primeiro teste, mostra-se que a NIC está utilizando DMA, caso contrário as duas tarefas teriam que ser executadas integralmente pela CPU e o tempo total seria igual a soma dos tempos do primeiro teste.

- Teste demonstrando a operação do driver suportando comunicação entre dois QEMUs, operando com interrupções, e o fluxo de pacotes de rede entre o driver e a aplicação considerando o desacoplamento do tratamento de interrupções, o escalonamento e a transferência dos dados para o escopo da aplicação:
E os arquivos de teste respectivos criados:
`dir_protocol_test.cc`
`dir_protocol_test_traits.h`

- Teste demonstrando a interoperabilidade com outros protocolos:
Foram criados dois testes, demonstrando que a comunicação através do DIR_Protocol opera sem conflito com outros protocolos.
`dir_interop_test.cc` `dir_interop_test_traits.h`
`dir_interop_test_simple.cc` `dir_interop_test_simple_traits.h`