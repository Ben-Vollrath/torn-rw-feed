#pragma once

#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"
#include "cryptopp/base64.h"

namespace CryptoUtils
{
	inline std::vector<CryptoPP::byte> randomBytes(size_t size)
	{
		CryptoPP::AutoSeededRandomPool rng;
		std::vector<CryptoPP::byte> bytes(size);
		rng.GenerateBlock(bytes.data(), bytes.size());
		return bytes;
	}

	inline std::string hexEncode(const CryptoPP::byte* data, size_t size)
	{
		std::string encoded;
		CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(encoded), false);
		// 'false' -> lowercase; use 'true' for uppercase hex
		encoder.Put(data, size);
		encoder.MessageEnd();
		return encoded;
	}

	inline std::string base64URLEncode(const CryptoPP::byte* data, size_t size)
	{
		std::string encoded;
		CryptoPP::Base64URLEncoder encoder(new CryptoPP::StringSink(encoded), false);

		encoder.Put(data, size);
		encoder.MessageEnd();
		return encoded;
	}

	inline std::string base64URLDecode(const std::string& encoded)
	{
		std::string decoded;
		CryptoPP::Base64URLDecoder decoder(new CryptoPP::StringSink(decoded));

		decoder.Put(reinterpret_cast<const CryptoPP::byte*>(encoded.data()), encoded.size());
		decoder.MessageEnd();
		return decoded;
	}


	inline std::string sha256Hex(const CryptoPP::byte* data, size_t n)
	{
		CryptoPP::SHA256 sha;
		sha.Update(data, n);
		CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
		sha.Final(digest);

		std::string hex;
		CryptoPP::HexEncoder enc(new CryptoPP::StringSink(hex), /*uppercase=*/false);
		enc.Put(digest, sizeof(digest));
		enc.MessageEnd();
		return hex;
	}
}
