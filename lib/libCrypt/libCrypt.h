struct rc4_context {
	unsigned int i;
	unsigned int j;
	unsigned char s[256];
};

int encryptXOR(void* key, void* data);

int rc4_init(struct rc4_context* context, const unsigned char* key, unsigned long long length);

int rc4_cipher(struct rc4_context* context, const unsigned char* input, unsigned char* output, unsigned long long length);
