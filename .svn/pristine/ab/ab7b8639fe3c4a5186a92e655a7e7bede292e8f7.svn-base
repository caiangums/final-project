// EPOS RS-485 Mediator Test Program

#include <machine.h>
#include <time.h>

using namespace EPOS;

int main()
{
    OStream cout;

    RS485 rs485(1, 19200, 8, 0, 1);
    char buf[16];

    union { // conversao hexadecimal para Float IEEE 754
        char c[4];
        float f;
    } conv;

    int k;
    int l;
    float tensaor;//valor da tensao medida na fase R
    float tensaos;//valor da tensao medida na fase S
    float tensaot;//valor da tensao medida na fase T
    float potativa;//valor potencia ativa
    float fatorpot;//valor do fator de potencia
    float corrente;//valor da corrente media


    while(true) {

        //tensao medida na fase R
        cout << "Write tensao fase R" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x01;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0x60;
        buf[7] = 0x0A;
        rs485.write(buf, 8); //Envio da mensagem RS485
        Delay(350000);
        int res = rs485.read(&buf, 16); //Leitura da mensagem RS485
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        tensaor = conv.f;// conversao hexadecimal para Float IEEE 754
        cout << tensaor;
        cout << endl;
        Delay(1000000);

        //tensao medida na fase S
        cout << "Write tensao fase S" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x02;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0x90;
        buf[7] = 0x0A;
        rs485.write(buf, 8);
        Delay(350000);
        res = rs485.read(&buf, 16);
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        tensaos = conv.f;
        cout << tensaos;
        cout << endl;
        Delay(1000000);

        //tensao medida na fase T
        cout << "Write tensao fase T" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x03;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0xC1;
        buf[7] = 0xCA;
        rs485.write(buf, 8);
        Delay(350000);
        res = rs485.read(&buf, 16);
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        tensaot = conv.f;
        cout << tensaot;
        cout << endl;
        Delay(1000000);

        //potencia ativa
        cout << "Write potencia ativa" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x0C;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0xF1;
        buf[7] = 0xC9;
        rs485.write(buf, 8);
        Delay(350000);
        res = rs485.read(&buf, 16);
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        potativa = conv.f;
        cout << potativa;
        cout << endl;
        Delay(1000000);

        //corrente media
        cout << "Write corrente media" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x04;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0x70;
        buf[7] = 0x0B;
        rs485.write(buf, 8);
        Delay(350000);
        res = rs485.read(&buf, 16);
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        corrente = conv.f;
        cout << corrente;
        cout << endl;
        Delay(1000000);

        //fator de potencia medio
        cout << "Write fator de potencia medio" << endl;
        buf[0] = 0x01;
        buf[1] = 0x04;
        buf[2] = 0x00;
        buf[3] = 0x08;
        buf[4] = 0x00;
        buf[5] = 0x01;
        buf[6] = 0xB0;
        buf[7] = 0x08;
        rs485.write(buf, 8);
        Delay(350000);
        res = rs485.read(&buf, 16);
        k = buf[4];
        l = buf[5];
        conv.c[0] = 0x00;
        conv.c[1] = 0x00;
        conv.c[2] = l;
        conv.c[3] = k;
        fatorpot = conv.f;
        cout << fatorpot;
        cout << endl;
        Delay(1000000);

    }

    return 0;
}
