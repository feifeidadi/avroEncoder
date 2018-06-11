#ifndef __DATA_ENCODER_H__
#define __DATA_ENCODER_H__

#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>

#include "avro/Compiler.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "avro/Specific.hh"

using namespace std;

template <class T>
class avroEncoder
{
public:
    avroEncoder() {};
    virtual ~avroEncoder() {};

    // Return 0 on success
    int init(const char *jsonSchema);

    /* In case of encoding error, returning NULL and length is 0
     * Its caller responsibility to free the memory of avro binary encoded buffer
     */ 
    uint8_t* binaryEncode(T &dataStruct, int &length);

#ifdef DEBUG
    void dumpContent(const uint8_t *buffer, int length);
    void saveContentToFile(const char *filename, const uint8_t *bytes, int length);
#endif

private:
    // Binary encoded buffer length will be returned
    int avroBinaryEncode(T &dataStruct);
    uint8_t* getBinaryStream(int &length);
    int readStreamToBuf(uint8_t *buffer);

private:
    ifstream ifs;
    avro::ValidSchema validSchema;
    auto_ptr<avro::OutputStream> out;
};

#ifdef DEBUG
template <class T>
void avroEncoder<T>::saveContentToFile(const char *filename, const uint8_t *bytes, int length)
{
    ofstream fout(filename, ios::out | ios::binary);
    fout.write((const char *)&bytes[0], length);
    fout.close();
}

template <class T>
void avroEncoder<T>::dumpContent(const uint8_t *buffer, int length)
{
    int count = 0;
    bool evenNum = false;
    while (count < length) {
        if((count % 16) == 0)  printf("\n");

        printf("%02x", buffer[count++]);
        if(evenNum) printf(" ");

        evenNum = !evenNum;
    }
    cout << endl;
}
#endif

template <class T>
int avroEncoder<T>::init(const char *jsonSchema)
{
    ifs.open(jsonSchema, ifstream::in);
    if (ifs.is_open()) {
        cout << "Open " << jsonSchema << " successfully!" << endl;
    } else {
        cout << "Open " << jsonSchema << " failed." << endl;
        return -1;
    }

    avro::compileJsonSchema(ifs, validSchema);

    return 0;
}

template<typename T>
int avroEncoder<T>::avroBinaryEncode(T &dataStruct)
{
    out = avro::memoryOutputStream();
    avro::EncoderPtr e = avro::validatingEncoder(validSchema, avro::binaryEncoder());
    e->init(*out);

    avro::encode(*e, dataStruct);
    e->flush(); // Flush to get the real length of encoded buffer

    return out->byteCount();
}

template<typename T>
int avroEncoder<T>::readStreamToBuf(uint8_t *buffer)
{
    auto_ptr<avro::InputStream> in = avro::memoryInputStream(*out);
    avro::StreamReader* reader = new avro::StreamReader(*in);

    uint8_t *p = buffer;
    while (reader->hasMore()) {
        *p++ = reader->read();
    }

    delete reader;

    return 0;
}

template<typename T>
uint8_t* avroEncoder<T>::getBinaryStream(int &length)
{
    uint8_t *encodedBuffer = new uint8_t[length];
    if(encodedBuffer == NULL) {
        printf("Allocate memory uint8_t[%d] failed!\n", length);
        return NULL;
    }

    memset(encodedBuffer, 0, length);
    readStreamToBuf(encodedBuffer);

    return encodedBuffer;
}

template<typename T>
uint8_t* avroEncoder<T>::binaryEncode(T &dataStruct, int &length)
{
    length = avroBinaryEncode(dataStruct);
    printf("Length = %d.\n", length);

    if(length == 0) {
        cout << "Error, encoding failed!" << endl;
        printf("Error, encoding failed!\n");
        return NULL;
    }

    return getBinaryStream(length);
}

#endif

