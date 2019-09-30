// EPOS Cipher Mediator Test Program

#include <utility/random.h>
#include <machine.h>

using namespace EPOS;

OStream cout;

static const unsigned int ITERATIONS = 100;

int main()
{
    unsigned int seed = Random::random();

    cout << "EPOS Cipher Test" << endl;
    cout << "Configuration: " << endl;
    cout << "Cipher::KEY_SIZE = " << AES<16>::KEY_SIZE << endl;
    cout << "Random seed = " << seed << endl;
    cout << "Iterations = " << ITERATIONS << endl;

    unsigned int tests_failed = 0;

    Random::seed(seed);

    AES<16> cipher;

    if(cipher.mode() == AES<16>::Mode::ECB && AES<16>::KEY_SIZE == 16) {
        cout << endl;
        cout << "Testing AES-128-ECB with known vectors...";

        // Test vectors from National Institute of Standards and Technology Special Publication 800-38A 2001 ED
        const unsigned char clear_text[4][16] = {{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a},
                                                 {0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51},
                                                 {0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef},
                                                 {0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10}};

        const unsigned char key[] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};

        const unsigned char expected[4][16] = {{0x3a,0xd7,0x7b,0xb4,0x0d,0x7a,0x36,0x60,0xa8,0x9e,0xca,0xf3,0x24,0x66,0xef,0x97},
                                               {0xf5,0xd3,0xd5,0x85,0x03,0xb9,0x69,0x9d,0xe7,0x85,0x89,0x5a,0x96,0xfd,0xba,0xaf},
                                               {0x43,0xb1,0xcd,0x7f,0x59,0x8e,0xce,0x23,0x88,0x1b,0x00,0xe3,0xed,0x03,0x06,0x88},
                                               {0x7b,0x0c,0x78,0x5e,0x27,0xe8,0xad,0x3f,0x82,0x23,0x20,0x71,0x04,0x72,0x5d,0xd4}};

        unsigned char result[4][16];
        for(unsigned int i = 0; i < 4; i ++)
            cipher.encrypt(clear_text[i], key, result[i]);

        bool ok = true;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 16; j++)
                if(result[i][j] != expected[i][j]) {
                    ok = false;
                    break;
                }
        tests_failed += !ok;
        if(ok)
            cout << "OK!" << endl;
        else {
            cout << "ERROR!" << endl;
            cout << "Either the AES implementation is wrong or this is not an AES cipher!" << endl;
        }
    }

    for(unsigned int it = 0; it < ITERATIONS; it++) {
        unsigned char clear_text[AES<16>::KEY_SIZE];
        unsigned char cipher_text[AES<16>::KEY_SIZE];
        unsigned char decrypted_text[AES<16>::KEY_SIZE];
        unsigned char key[AES<16>::KEY_SIZE];
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++) {
            clear_text[i] = Random::random();
            key[i] = Random::random();
        }

        cout << endl;
        cout << "Iteration " << it << endl;

        cout << "clear_text =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(clear_text[i]);
        cout << endl;

        cout << "key =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(key[i]);
        cout << endl;

        cout << "Testing encryption...";
        cipher.encrypt(clear_text, key, cipher_text);

        bool ok = false;
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = true;
                break;
            }
        if(ok)
            cout << "OK!" << endl;
        else
            cout << "ERROR!" << endl;
        tests_failed += !ok;
        cout << "cipher_text =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(cipher_text[i]);
        cout << endl;



        cout << "Testing decryption...";

        cipher.decrypt(cipher_text, key, decrypted_text);
        ok = true;
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = false;
                break;
            }
        if(ok)
            cout << "OK!" << endl;
        else
            cout << "ERROR!" << endl;
        tests_failed += !ok;

        cout << "decrypted_text =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(decrypted_text[i]);
        cout << endl;



        cout << "Testing decryption with wrong key...";

        key[Random::random() % AES<16>::KEY_SIZE]++;

        cipher.decrypt(cipher_text, key, decrypted_text);

        ok = false;
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            if(decrypted_text[i] != clear_text[i]) {
                ok = true;
                break;
            }
        if(ok)
            cout << "OK!" << endl;
        else
            cout << "ERROR!" << endl;
        tests_failed += !ok;

        cout << "key =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(key[i]);
        cout << endl;
        cout << "decrypted_text =";
        for(unsigned int i = 0; i < AES<16>::KEY_SIZE; i++)
            cout << " " << hex << static_cast<unsigned int>(decrypted_text[i]);
        cout << endl;
    }

    cout << endl;
    cout << "Tests finished with " << tests_failed << " error" << (tests_failed > 1 ? "s" : "") << " detected." << endl;
    cout << endl;

    return 0;
}
