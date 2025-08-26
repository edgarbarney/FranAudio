// FranticDreamer 2022-2025
#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <type_traits>
#include <concepts>

#include "FranAudioShared/Containers/UnorderedMap.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudioShared::Serialisation
{
	// ========================
	// Serialisable Interface
	// ========================

	/// <summary>
	/// Interface for objects that can be serialized to and deserialized from a byte buffer.
	/// </summary>
	struct Serialisable
	{
		virtual void Serialise(std::vector<char>& out) const = 0;
		virtual void Deserialise(const char* data, size_t& offset, size_t size) = 0;
		virtual ~Serialisable() = default;
	};

	/// <summary>
	/// A simple binary serialiser and deserialiser for unordered maps and vectors.
	///
	/// Mainly used for network communication in FranAudioClient and FranAudioServer.
	/// </summary>
	namespace BinarySerialiser
	{
		// ========================
		// Helpers for Trivially Copyable
		// ========================

		/// <summary>
		/// Serializes a trivially copyable primitive value into a byte vector.
		/// </summary>
		/// <typeparam name="T">The type of the value to serialize. Must be trivially copyable.</typeparam>
		/// <param name="value">The primitive value to serialize.</param>
		/// <param name="out">The vector to which the serialized bytes will be appended.</param>
		template <typename T>
		requires std::is_trivially_copyable_v<T>
		inline void SerialisePrimitive(const T& value, std::vector<char>& out)
		{
			const char* temp = reinterpret_cast<const char*>(&value);
			out.insert(out.end(), temp, temp + sizeof(T));
		}

		/// <summary>
		/// Deserializes a trivially copyable primitive value of type T from a binary data buffer.
		/// </summary>
		/// <typeparam name="T">The trivially copyable type to deserialize.</typeparam>
		/// <param name="data">Pointer to the binary data buffer to read from.</param>
		/// <param name="offset">Reference to the current offset in the data buffer; updated after reading.</param>
		/// <param name="size">Total size of the data buffer in bytes.</param>
		/// <returns>The deserialized value of type T. Returns a default-constructed T if there is not enough data.</returns>
		template <typename T>
		requires std::is_trivially_copyable_v<T>
		inline T DeserialisePrimitive(const char* data, size_t& offset, size_t size)
		{
			if (offset + sizeof(T) > size)
			{
				// No need to spam logs for every failed "small" deserialisation
				//FranAudioShared::Logger::LogError("BinarySerialiser: Not enough data to deserialise primitive type.");
				return {};
			}

			T value{};
			std::memcpy(&value, data + offset, sizeof(T));
			offset += sizeof(T);

			return value;
		}
		
		// ========================
		// Helpers for std::string
		// Mainly for container serialisation
		// ========================

		inline void SerialiseString(const std::string& s, std::vector<char>& out)
		{
			uint64_t len = s.size();
			SerialisePrimitive(len, out);
			out.insert(out.end(), s.begin(), s.end());
		}

		/// <summary>
		/// Deserializes a string from a binary data buffer, updating the offset as it reads.
		/// </summary>
		/// <param name="data">Pointer to the binary data buffer containing the serialized string.</param>
		/// <param name="offset">Reference to the current offset in the data buffer; will be updated after deserialization.</param>
		/// <param name="size">Total size of the data buffer.</param>
		/// <returns>The deserialized string. Returns an empty string if there is not enough data to read.</returns>
		inline std::string DeserialiseString(const char* data, size_t& offset, size_t size)
		{
			uint64_t len = DeserialisePrimitive<uint64_t>(data, offset, size);

			if (offset + len > size)
			{
				FranAudioShared::Logger::LogError("BinarySerialiser: Not enough data to deserialise string.");
				return {};
			}

			std::string s(data + offset, data + offset + len);
			offset += len;
			return s;
		}
	
		// ========================
		// Generic Serialisation Helpers
		// ========================

		/// <summary>
		/// Serializes a value of type T into a vector of bytes.
		/// </summary>
		/// <typeparam name="T">The type of the value to serialize.</typeparam>
		/// <param name="value">The value to serialize.</param>
		/// <param name="out">A vector of bytes where the serialized data will be stored.</param>
		template <typename T>
		inline void Serialise(const T& value, std::vector<char>& out)
		{
			if constexpr (std::is_base_of_v<Serialisable, T>) 
			{
				value.serialise(out);
			}
			else if constexpr (std::is_trivially_copyable_v<T>)
			{
				SerialisePrimitive(value, out);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				SerialiseString(value, out);
			}
			else
			{
				FranAudioShared::Logger::LogError("BinarySerialiser: Unsupported type for Serialise");
				return;
			}
		}

		/// <summary>
		/// Deserializes an object of type T from a binary data buffer.
		/// </summary>
		/// <typeparam name="T">The type of object to deserialize. Must be either derived from Serialisable, trivially copyable, or std::string.</typeparam>
		/// <param name="data">Pointer to the binary data buffer to deserialize from.</param>
		/// <param name="offset">Reference to the current offset within the data buffer. This will be updated as data is read.</param>
		/// <param name="size">The total size of the data buffer.</param>
		/// <returns>A deserialized object of type T. If the type is unsupported, returns a default-constructed T.</returns>
		template <typename T>
		inline T Deserialise(const char* data, size_t& offset, size_t size)
		{
			if constexpr (std::is_base_of_v<Serialisable, T>)
			{
				T temp{};
				temp.deserialise(data, offset, size);
				return temp;
			}
			else if constexpr (std::is_trivially_copyable_v<T>)
			{
				return DeserialisePrimitive<T>(data, offset, size);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				return DeserialiseString(data, offset, size);
			}
			else
			{
				FranAudioShared::Logger::LogError("BinarySerialiser: Unsupported type for Deserialise");
				return {};
			}
		}

		// =====================
		// Vector Serialisation
		// =====================

		/// <summary>
		/// Serializes a vector of elements into a binary-safe string.
		/// </summary>
		/// <typeparam name="T">The type of elements contained in the vector.</typeparam>
		/// <param name="source">The vector of elements to serialize.</param>
		/// <returns>A binary-safe string containing the serialized representation of the vector and its elements.</returns>
		template <typename T>
		inline std::string SerialiseVector(const std::vector<T>& source)
		{
			std::vector<char> raw;
			uint64_t size = source.size();
			SerialisePrimitive(size, raw);

			for (auto& v : source)
			{
				Serialise(v, raw);
			}

			return std::string(raw.begin(), raw.end());
		}

		/// <summary>
		/// Deserializes a vector of elements from a binary string buffer.
		/// </summary>
		/// <typeparam name="T">The type of elements to deserialize into the vector.</typeparam>
		/// <param name="buffer">A string containing the binary data to deserialize.</param>
		/// <returns>A std::vector<T> containing the deserialized elements from the buffer.</returns>
		template <typename T>
		std::vector<T> DeserialiseVector(const std::string& buffer)
		{
			size_t offset = 0;
			size_t size = buffer.size();
			const char* data = buffer.data();

			uint64_t count = DeserialisePrimitive<uint64_t>(data, offset, size);

			std::vector<T> vector;
			vector.reserve(count);
			for (uint64_t i = 0; i < count; i++)
			{
				vector.push_back(Deserialise<T>(data, offset, size));
			}

			return vector;
		}

		// =====================
		// Unordered_map Serialisation
		// =====================

		/// <summary>
		/// Serializes an unordered map into a binary-safe string representation.
		/// </summary>
		/// <typeparam name="K">The type of the keys in the unordered map.</typeparam>
		/// <typeparam name="V">The type of the values in the unordered map.</typeparam>
		/// <param name="source">The unordered map to serialize.</param>
		/// <returns>A vector of strings containing the binary-safe serialized data of the unordered map.</returns>
		template <typename K, typename V>
		std::string SerialiseUnorderedMap(const FranAudioShared::Containers::UnorderedMap<K, V>& source)
		{
			std::vector<char> rawData;
			uint64_t size = source.size();
			SerialisePrimitive(size, rawData);

			for (auto& [k, v] : source)
			{
				Serialise(k, rawData);
				Serialise(v, rawData);
			}

			return std::string(rawData.begin(), rawData.end());
		}

		/// <summary>
		/// Deserializes an unordered map from a binary string source.
		/// </summary>
		/// <typeparam name="K">The type of the keys in the unordered map.</typeparam>
		/// <typeparam name="V">The type of the values in the unordered map.</typeparam>
		/// <param name="source">A string containing the binary data representing the unordered map.</param>
		/// <returns>An unordered_map<K, V> reconstructed from the binary data in the source string.</returns>
		template <typename K, typename V>
		FranAudioShared::Containers::UnorderedMap<K, V> DeserialiseUnorderedMap(const std::string& source)
		{
			size_t offset = 0;
			size_t size = source.size();
			const char* data = source.data();

			uint64_t count = DeserialisePrimitive<uint64_t>(data, offset, size);
			FranAudioShared::Containers::UnorderedMap<K, V> map;

			for (uint64_t i = 0; i < count; i++)
			{
				K k = Deserialise<K>(data, offset, size);
				V v = Deserialise<V>(data, offset, size);
				map.emplace(std::move(k), std::move(v));
			}

			return map;
		}
	}
}
