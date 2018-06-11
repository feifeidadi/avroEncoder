#include "cpx.h"
#include "avroEncoder.h"

#define SCHEMA_FILE "../schema/cpx.json"

static
void avroEncode(c::cpx &c1)
{
    c1.re = 1.0;
    c1.im = 2.0;
}

int
main()
{
    c::cpx c1;
    avroEncode(c1);

    int len = 0;
    avroEncoder<c::cpx> test;
    if(test.init(SCHEMA_FILE) != 0) {
        return -1;
    }

    uint8_t* avroBinBuf = test.binaryEncode(c1, len);

#ifdef DEBUG
    test.dumpContent(avroBinBuf, len);
    test.saveContentToFile("cpx.bin", avroBinBuf, len);
#endif

    if(avroBinBuf) delete avroBinBuf;

    return 0;
}
