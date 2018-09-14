#ifndef M_BASE_MD5_HPP__
#define M_BASE_MD5_HPP__

#include "slience/base/config.hpp"
#include <string>
#include <stdint.h> //  for data type

M_BASE_NAMESPACE_BEGIN

#include "md5_helper.ipp"

/* MD5 context. */ 
typedef struct _MD5_CTX{

	/* state (ABCD) */   
	/*�ĸ�32bits�������ڴ�����ռ���õ�����ϢժҪ������Ϣ���ȡ�512bitsʱ��Ҳ���ڴ��ÿ��512bits���м���*/ 
	uint32 state[4];  

	/* number of bits, modulo 2^64 (lsb first) */    
	/*�洢ԭʼ��Ϣ��bits������,����������bits���Ϊ 2^64 bits����Ϊ2^64��һ��64λ�������ֵ*/
	uint32 count[2];

	/* input buffer */ 
	/*����������Ϣ�Ļ�������512bits*/
	unsigned char buffer[64];

} MD5_CTX;

class MD5
{
public:
	MD5();

	MD5(const char* input);

	MD5(const MD5& md5);

	bool operator == (const MD5& md5);

	MD5& operator = (const MD5& md5);

	std::string toString();

	const unsigned char* get()const;

protected:

	void encrypt(const unsigned char* input);

	// ��ʼ��
	void _init();

	void _update(const unsigned char* input, unsigned int inputLen);

	void _final();
private:

	MD5_CTX m_ctx;
	
	unsigned char m_digest[17];
};


MD5::MD5()
{
	R_memset((unsigned char*)m_digest,0,sizeof(m_digest));
	m_digest[16]='\0';
}

MD5::MD5(const char* input)
{
	encrypt((unsigned char*)input);
}

MD5::MD5(const MD5& md5)
{
	*this=md5;
}

bool MD5::operator==(const MD5& md5)
{
	return (R_memcmp(m_digest,md5.m_digest,sizeof(m_digest))==0);
}

MD5& MD5::operator=(const MD5& md5)
{
	R_memcpy(m_digest,md5.m_digest,sizeof(m_digest));
	return *this;
}

std::string MD5::toString()
{
	static const char hex_digits[] = "0123456789ABCDEF";
	char output[33];
	int index, j = 0;
	for (int i = 0; i < 16; i++) {
		index = (m_digest[i] & 0xF0) >> 4;
		output[j++] = hex_digits[index];
		index = m_digest[i] & 0x0F;
		output[j++] = hex_digits[index];
	}
	output[j] = 0;
	return std::string(output);
}

const unsigned char* MD5::get()const
{
	return m_digest;
}

void MD5::encrypt(const unsigned char* input)
{
	R_memset((unsigned char*)m_digest,0,sizeof(m_digest));
	m_digest[16]='\0';

	_init();

	_update(input,R_strlen((char*)input));

	_final();
}

void MD5::_init()
{
	/*����ǰ����Ч��Ϣ�ĳ������0,����ܼ�,��û����Ч��Ϣ,���ȵ�Ȼ��0��*/
	m_ctx.count[0] = m_ctx.count[1] = 0;

	/* Load magic initialization constants.*/
	/*��ʼ�����ӱ������㷨Ҫ�����������û��������*/
	m_ctx.state[0] = 0x67452301;
	m_ctx.state[1] = 0xefcdab89;
	m_ctx.state[2] = 0x98badcfe;
	m_ctx.state[3] = 0x10325476;
}

void MD5::_update(const unsigned char* input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	/*����������Ϣ��bits���ȵ��ֽ�����ģ64, 64bytes=512bits��
	�����ж�������Ϣ���ϵ�ǰ����������Ϣ���ܳ����ܲ��ܴﵽ512bits��
	����ܹ��ﵽ��Դչ���512bits����һ�δ���*/
	index = (unsigned int)((m_ctx.count[0] >> 3) & 0x3F);

	/* Update number of bits *//*����������Ϣ��bits����*/
	if((m_ctx.count[0] += ((uint32)inputLen << 3)) < ((uint32)inputLen << 3))
		m_ctx.count[1]++;
	m_ctx.count[1] += ((uint32)inputLen >> 29);

	/*�������е��ֽ������Ȼ�������ֽڿ��� �ճ�64��������*/
	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	/*�����ǰ������ֽ��� ���� �����ֽ������Ȳ���64�ֽ�������������ֽ���*/
	if(inputLen >= partLen) 
	{
		/*�õ�ǰ��������ݰ�context->buffer�����ݲ���512bits*/
		R_memcpy((POINTER)&m_ctx.buffer[index], (POINTER)input, partLen);
		/*�û����������������512bits���Ѿ����浽context->buffer�У� ��һ��ת����ת��������浽context->state��*/
		MD5Helper::MD5Transform(m_ctx.state,m_ctx.buffer);
		/*
		�Ե�ǰ�����ʣ���ֽ���ת�������ʣ����ֽ�<�������input��������>����512bits�Ļ� ����
		ת��������浽context->state��
		*/
		for(i = partLen; i + 63 < inputLen; i += 64 )/*��i+63<inputlen��Ϊi+64<=inputlen���������*/
			MD5Helper::MD5Transform(m_ctx.state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	/*�����뻺�����еĲ��������512bits��ʣ��������䵽context->buffer�У������Ժ���������*/
	R_memcpy((POINTER)&m_ctx.buffer[index], (POINTER)&input[i], inputLen-i);
}

void MD5::_final()
{
	unsigned char bits[8];
	uint32 index, padLen;

	/* Save number of bits */
	/*��Ҫ��ת������Ϣ(���е�)��bits���ȿ�����bits��*/
	MD5Helper::Encode(bits,m_ctx.count,8);

	/* Pad out to 56 mod 64. */
	/* �������е�bits���ȵ��ֽ�����ģ64, 64bytes=512bits*/
	index = (uint32)((m_ctx.count[0] >> 3) & 0x3f);
	
	/*������Ҫ�����ֽ�����padLen��ȡֵ��Χ��1-64֮��*/
	padLen = (index < 56) ? (56 - index) : (120 - index);
	
	/*��һ�κ������þ��Բ����ٵ���MD5Transform�ı����ã���Ϊ��һ�β�������512bits*/
	_update(MD5Helper::PADDING,padLen);
	
	/* Append length (before padding) */
	/*����ԭʼ��Ϣ��bits���ȣ�bits���ȹ̶�����64bits��ʾ������һ���ܹ�ǡ�ɴչ�512bits�������Ҳ������*/
	_update(bits, 8);
	
	/* Store state in digest */
	/*�����յĽ�����浽digest�С�ok�����ڴ󹦸����*/
	MD5Helper::Encode(m_digest, m_ctx.state,16);

	/* Zeroize sensitive information. */
	R_memset((void*)(&m_ctx), 0, sizeof(MD5_CTX));
}

M_BASE_NAMESPACE_END
#endif