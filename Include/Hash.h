#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* 左旋转 */
#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* SHA1 上下文 */
typedef struct
{
	uint32_t state[5];
	uint64_t total_len;
	uint8_t buffer[64];
	size_t buffer_len;
} SHA1_CTX;

/* 读取大端序 32 位 */
inline uint32_t _sha1_read_be32(const uint8_t* p)
{
	return ((uint32_t)p[0] << 24) |
		((uint32_t)p[1] << 16) |
		((uint32_t)p[2] << 8) |
		(uint32_t)p[3];
}

/* 写入大端序 32 位 */
inline void _sha1_write_be32(uint8_t* p, uint32_t v)
{
	p[0] = (v >> 24) & 0xFF;
	p[1] = (v >> 16) & 0xFF;
	p[2] = (v >> 8) & 0xFF;
	p[3] = v & 0xFF;
}

/* 处理 64 字节块 */
inline void _sha1_transform(SHA1_CTX* ctx, const uint8_t* block)
{
	uint32_t w[80];
	uint32_t a, b, c, d, e, f, k, temp;
	int i;

	for (i = 0; i < 16; i++)
	{
		w[i] = _sha1_read_be32(block + i * 4);
	}

	for (i = 16; i < 80; i++)
	{
		w[i] = ROL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
	}

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];

	for (i = 0; i < 80; i++)
	{
		if (i < 20)
		{
			f = (b & c) | ((~b) & d);
			k = 0x5A827999;
		}
		else if (i < 40)
		{
			f = b ^ c ^ d;
			k = 0x6ED9EBA1;
		}
		else if (i < 60)
		{
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		}
		else
		{
			f = b ^ c ^ d;
			k = 0xCA62C1D6;
		}

		temp = ROL(a, 5) + f + e + k + w[i];
		e = d;
		d = c;
		c = ROL(b, 30);
		b = a;
		a = temp;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
}

/**
 * 初始化编码环境的上下文，用于流式编码使用
 * @param ctx 上下文信息结构体
 */
void sha1_init(SHA1_CTX* ctx)
{
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xC3D2E1F0;
	ctx->total_len = 0;
	ctx->buffer_len = 0;
}

/**
 *  通过流式数据传入将数据编码为sha1
 * @param ctx  编码需要的上下文结构体
 * @param data 需要被编码的数据
 * @param len 需要被编码的数据的长度
 */
void sha1_update(SHA1_CTX* ctx, const uint8_t* data, size_t len)
{
	size_t need;

	ctx->total_len += len;

	if (ctx->buffer_len > 0)
	{
		need = 64 - ctx->buffer_len;
		if (len < need)
		{
			memcpy(ctx->buffer + ctx->buffer_len, data, len);
			ctx->buffer_len += len;
			return;
		}
		memcpy(ctx->buffer + ctx->buffer_len, data, need);
		_sha1_transform(ctx, ctx->buffer);
		data += need;
		len -= need;
		ctx->buffer_len = 0;
	}

	while (len >= 64)
	{
		_sha1_transform(ctx, data);
		data += 64;
		len -= 64;
	}

	if (len > 0)
	{
		memcpy(ctx->buffer, data, len);
		ctx->buffer_len = len;
	}
}

/**
 * 结束sha1的流式编码
 * @param ctx 编码使用的上下文结构体
 * @param out 编码完成后的sha1数据
 */
void sha1_final(SHA1_CTX* ctx, uint8_t* out)
{
	uint64_t bit_len;
	int i;

	ctx->buffer[ctx->buffer_len++] = 0x80;

	if (ctx->buffer_len > 56)
	{
		memset(ctx->buffer + ctx->buffer_len, 0, 64 - ctx->buffer_len);
		_sha1_transform(ctx, ctx->buffer);
		ctx->buffer_len = 0;
	}

	memset(ctx->buffer + ctx->buffer_len, 0, 56 - ctx->buffer_len);

	bit_len = ctx->total_len * 8;
	ctx->buffer[56] = (bit_len >> 56) & 0xFF;
	ctx->buffer[57] = (bit_len >> 48) & 0xFF;
	ctx->buffer[58] = (bit_len >> 40) & 0xFF;
	ctx->buffer[59] = (bit_len >> 32) & 0xFF;
	ctx->buffer[60] = (bit_len >> 24) & 0xFF;
	ctx->buffer[61] = (bit_len >> 16) & 0xFF;
	ctx->buffer[62] = (bit_len >> 8) & 0xFF;
	ctx->buffer[63] = bit_len & 0xFF;

	_sha1_transform(ctx, ctx->buffer);

	for (i = 0; i < 5; i++)
	{
		_sha1_write_be32(out + i * 4, ctx->state[i]);
	}
}

/**
 * 一次性将数据编码为sha1值
 * @param data 需要被编码的数据
 * @param len 需要被编码的数据的长度
 * @param out 编码为sha1的数据（长度为20的数组）
 */
void sha1(const uint8_t* data, size_t len, uint8_t* out)
{
	SHA1_CTX ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, data, len);
	sha1_final(&ctx, out);
}

/**
 * 将已经编码的数据转换为16进制的文本数据用于打印
 * 和现实使用
 * @param hash 已经编码过的数据
 * @param hex 输出的16进制文本
 */
inline void hash_to_hex(const uint8_t* hash, char* hex)
{
	int i;
	for (i = 0; i < 20; i++)
	{
		sprintf(hex + i * 2, "%02x", hash[i]);
	}
	hex[40] = '\0';
}

/* 测试 */
int _hashmain(void)
{
	uint8_t hash[20];
	int i;

	/* 测试 1 */
	sha1((uint8_t*)"hello", 5, hash);
	printf("SHA1(\"hello\") = ");
	for (i = 0; i < 20; i++) printf("%02x", hash[i]);
	printf("\n");
	/* 期望: aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d */

	/* 测试 2: 流式 */
	SHA1_CTX ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, (uint8_t*)"hel", 3);
	sha1_update(&ctx, (uint8_t*)"lo", 2);
	sha1_final(&ctx, hash);
	printf("SHA1(\"hel\"+\"lo\") = ");
	for (i = 0; i < 20; i++) printf("%02x", hash[i]);
	printf("\n");

	/* 测试 3 */
	sha1((uint8_t*)"", 0, hash);
	printf("SHA1(\"\") = ");
	for (i = 0; i < 20; i++) printf("%02x", hash[i]);
	printf("\n");
	/* 期望: da39a3ee5e6b4b0d3255bfef95601890afd80709 */

	return 0;
}
