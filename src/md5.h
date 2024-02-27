#ifndef _MD5_
#define _MD5_

	// Define the MD5 Hashing Class
	#define E(x, y, z)      ((z) ^ ((x) & ((y) ^ (z))))
	#define G(x, y, z)      ((y) ^ ((z) & ((x) ^ (y))))
	#define H(x, y, z)      ((x) ^ (y) ^ (z))
	#define I(x, y, z)      ((y) ^ ((x) | ~(z)))

	// Define the MD5 Hashing Class
	#define STEP(f, a, b, c, d, x, t, s) (a) += f((b), (c), (d)) + (x) + (t); (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); (a) += (b);

	// Define the MD5 Hashing Class
	#define MD5_SET(n) (this->block[(n)] = (unsigned long)ptr[(n) * 4] | ((unsigned long)ptr[(n) * 4 + 1] << 8) | ((unsigned long)ptr[(n) * 4 + 2] << 16) | ((unsigned long)ptr[(n) * 4 + 3] << 24))
	#define MD5_GET(n) (this->block[(n)])

	// MD5 Hashing Class
	class MD5 {

		private:

			// Process the given data
			const void* Process(const void* data, size_t length) {

				const unsigned char *ptr;
				unsigned long a, b, c, d;
				unsigned long saved_a, saved_b, saved_c, saved_d;

				ptr = (unsigned char*)data;

				a = this->a;
				b = this->b;
				c = this->c;
				d = this->d;

				do {
					saved_a = a;
					saved_b = b;
					saved_c = c;
					saved_d = d;

				/* Round 1
				* E() has been used instead of F() because F() is already defined in the Arduino core
				*/
					STEP(E, a, b, c, d, MD5_SET(0), 0xd76aa478, 7)
					STEP(E, d, a, b, c, MD5_SET(1), 0xe8c7b756, 12)
					STEP(E, c, d, a, b, MD5_SET(2), 0x242070db, 17)
					STEP(E, b, c, d, a, MD5_SET(3), 0xc1bdceee, 22)
					STEP(E, a, b, c, d, MD5_SET(4), 0xf57c0faf, 7)
					STEP(E, d, a, b, c, MD5_SET(5), 0x4787c62a, 12)
					STEP(E, c, d, a, b, MD5_SET(6), 0xa8304613, 17)
					STEP(E, b, c, d, a, MD5_SET(7), 0xfd469501, 22)
					STEP(E, a, b, c, d, MD5_SET(8), 0x698098d8, 7)
					STEP(E, d, a, b, c, MD5_SET(9), 0x8b44f7af, 12)
					STEP(E, c, d, a, b, MD5_SET(10), 0xffff5bb1, 17)
					STEP(E, b, c, d, a, MD5_SET(11), 0x895cd7be, 22)
					STEP(E, a, b, c, d, MD5_SET(12), 0x6b901122, 7)
					STEP(E, d, a, b, c, MD5_SET(13), 0xfd987193, 12)
					STEP(E, c, d, a, b, MD5_SET(14), 0xa679438e, 17)
					STEP(E, b, c, d, a, MD5_SET(15), 0x49b40821, 22)

				/* Round 2 */
					STEP(G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
					STEP(G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
					STEP(G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
					STEP(G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
					STEP(G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
					STEP(G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
					STEP(G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
					STEP(G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
					STEP(G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
					STEP(G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
					STEP(G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
					STEP(G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
					STEP(G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
					STEP(G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
					STEP(G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
					STEP(G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

				/* Round 3 */
					STEP(H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
					STEP(H, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
					STEP(H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
					STEP(H, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
					STEP(H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
					STEP(H, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
					STEP(H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
					STEP(H, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
					STEP(H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
					STEP(H, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
					STEP(H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
					STEP(H, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
					STEP(H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
					STEP(H, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
					STEP(H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
					STEP(H, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

				/* Round 4 */
					STEP(I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
					STEP(I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
					STEP(I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
					STEP(I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
					STEP(I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
					STEP(I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
					STEP(I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
					STEP(I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
					STEP(I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
					STEP(I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
					STEP(I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
					STEP(I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
					STEP(I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
					STEP(I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
					STEP(I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
					STEP(I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

					a += saved_a;
					b += saved_b;
					c += saved_c;
					d += saved_d;

					ptr += 64;
				} while (length -= 64);

				this->a = a;
				this->b = b;
				this->c = c;
				this->d = d;

				return ptr;

			}

			// Internal state
			unsigned long lo, hi;
			unsigned long a, b, c, d;

			// Internal data buffer
			unsigned char buffer[64];
			unsigned long block[16];

		public:

			// Constructor
			MD5() { 
				
				// Reset internal state
				this->Reset(); 
				
			}

			// Reset internal state
			void Reset(void) {

				a = 0x67452301;
				b = 0xefcdab89;
				c = 0x98badcfe;
				d = 0x10325476;
				lo = 0;
				hi = 0;

				memset(buffer, 0, 64*sizeof(unsigned char));
				memset(block, 0, 16*sizeof(unsigned long));
				
			}

			// Add data to the interative hash solution
			void Update(const void* data, size_t size) {

				unsigned long saved_lo;
				unsigned long used, free;

				saved_lo = this->lo;
				if ((this->lo = (saved_lo + size) & 0x1fffffff) < saved_lo) {
					this->hi++;
				}
				this->hi += size >> 29;

				used = saved_lo & 0x3f;

				if (used) {
					free = 64 - used;

					if (size < free) {
					memcpy(&this->buffer[used], data, size);
					return;
					}

					memcpy(&this->buffer[used], data, free);
					data = (unsigned char *)data + free;
					size -= free;
					this->Process(this->buffer, 64);
				}

				if (size >= 64) {
					data = this->Process(data, size & ~(size_t)0x3f);
					size &= 0x3f;
				}

				memcpy(this->buffer, data, size);

			}

			// Finalize the hash to the given 16-byte buffer
			void Finalize(uint8_t* _Result) {

				// Define the used and free variables
				uint32_t _Used, _Free;

				_Used = this->lo & 0x3f;

				this->buffer[_Used++] = 0x80;

				_Free = 64 - _Used;

				if (_Free < 8) {
					memset(&this->buffer[_Used], 0, _Free);
					this->Process(this->buffer, 64);
					_Used = 0;
					_Free = 64;
				}

				memset(&this->buffer[_Used], 0, _Free - 8);

				this->lo <<= 3;
				this->buffer[56] = this->lo;
				this->buffer[57] = this->lo >> 8;
				this->buffer[58] = this->lo >> 16;
				this->buffer[59] = this->lo >> 24;
				this->buffer[60] = this->hi;
				this->buffer[61] = this->hi >> 8;
				this->buffer[62] = this->hi >> 16;
				this->buffer[63] = this->hi >> 24;

				this->Process(this->buffer, 64);

				// Calculate the _Result
				_Result[0] = this->a;
				_Result[1] = this->a >> 8;
				_Result[2] = this->a >> 16;
				_Result[3] = this->a >> 24;
				_Result[4] = this->b;
				_Result[5] = this->b >> 8;
				_Result[6] = this->b >> 16;
				_Result[7] = this->b >> 24;
				_Result[8] = this->c;
				_Result[9] = this->c >> 8;
				_Result[10] = this->c >> 16;
				_Result[11] = this->c >> 24;
				_Result[12] = this->d;
				_Result[13] = this->d >> 8;
				_Result[14] = this->d >> 16;
				_Result[15] = this->d >> 24;

			}

			// Calculate the hash of the given null-terminated string constant
			static void Hash(const char* str, unsigned char* hash) {

				// Create a new MD5 instance
				MD5 md5 = MD5();

				// Update the hash with the given string
				for(; *str; str++) md5.Update(str, 1);

				// Finalize the hash
				md5.Finalize(hash);

			}

			// Calculate the hash of the given buffer
			static void Hash(const void* buffer, size_t size, unsigned char* hash) {

				// Create a new MD5 instance
				MD5 md5 = MD5();

				// Update the hash with the given buffer
				md5.Update(buffer, size);

				// Finalize the hash
				md5.Finalize(hash);

			}

			// Produce a hex digest of the given hash
			static void Digest(const unsigned char* hash, char* digest) {

				static const char digits[17] = "0123456789abcdef";

				for(int i = 0; i < 16; ++i){
					digest[i*2] = digits[hash[i] >> 4];
					digest[(i*2) + 1] = digits[hash[i] & 0x0F];
				}

				digest[32] = 0;

			}

	};

#endif