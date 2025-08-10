// FranticDreamer 2022-2025
#pragma once

#include <print>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>

namespace FranAudioShared
{
	namespace Network
	{
		// Constants
		// These should match the server's values
		// TODO: Move these to a configuration file

		constexpr size_t messageBufferSize = 1024;
		constexpr const char* listenAddress = "127.0.0.1"; // localhost
		constexpr unsigned short listenPort = 31069; // Hehe
		constexpr const int messageTimeout = 1000; //In milliseconds

		/// <summary>
		/// Represents a network function with a name and parameters.
		/// 
		/// This is used for client-server communication in FranAudio.
		/// The function name is the first part of the message, followed by parameters separated by '|'.
		/// Example: "$[functionName]|[param1]|[param2]|...|[paramN]"
		struct NetworkFunction
		{
			std::string functionName;
			std::vector<std::string> params;

			/// <summary>
			/// Default constructor.
			/// </summary>
			NetworkFunction()
			{

			}

			/// <summary>
			/// Default constructor.
			/// </summary>
			NetworkFunction(std::string functionName, std::vector<std::string> params)
				: functionName(std::move(functionName)), params(std::move(params)) 
			{

			}

			/// <summary>
			/// Parse constructor.
			/// </summary>
			NetworkFunction(std::string_view input)
			{
				auto parsedFunction = ParseFunction(input);
				functionName = std::move(parsedFunction.functionName);
				params = std::move(parsedFunction.params);
			}

			/// <summary>
			/// Parse a string input to create a NetworkFunction object.
			/// 
			/// - Imput format should be like this:
			/// "$[functionName]|[param1]|[param2]|...|[paramN]"
			/// </summary>
			/// 
			/// <param name="input">Input string, generally the received network message.</param>
			/// <returns></returns>
			static NetworkFunction ParseFunction(std::string_view input)
			{
				if (input.size() < 3 || input.front() != '$')
				{
	#if !(defined FRANAUDIO_CLIENT_DISABLE_LOGGING || defined FRANAUDIO_SERVER_DISABLE_LOGGING)
					std::println("NetworkFunction ParseInput: Invalid input format for network function");
	#endif
					return {};
				}

				input.remove_prefix(1);

				// Split on '|' and convert each part to std::string_view
				auto partsView = std::views::transform
				(
					std::views::split(input, '|'),
					[](auto&& subrange)
					{
						return std::string_view(&*subrange.begin(), static_cast<size_t>(std::ranges::distance(subrange)));
					}
				);

				auto it = partsView.begin();
				if (it == partsView.end())
				{
	#if !(defined FRANAUDIO_CLIENT_DISABLE_LOGGING || defined FRANAUDIO_SERVER_DISABLE_LOGGING)
					std::println("NetworkFunction ParseInput: Missing function name for network function");
	#endif
					return {};
				}

				NetworkFunction result;
				result.functionName = *it; // First element = function name
				++it;

				// Remaining elements are parameters
				for (; it != partsView.end(); ++it)
				{
					result.params.emplace_back(*it);
				}

				return result;
			}

			/// <summary>
			/// Parse a string input to create a NetworkFunction object.
			/// 
			/// - Imput format should be like this:
			/// "$[functionName]|[param1]|[param2]|...|[paramN]"
			/// </summary>
			/// 
			/// <param name="input">Input string, generally the received network message.</param>
			/// <returns></returns>

			/// <summary>
			/// Convert the NetworkFunction object to a string representation.
			///		
			/// - Output format will be like this:
			/// "$[functionName]|[param1]|[param2]|...|[paramN]"
			/// </summary>
			/// 
			/// <returns>String representation of the NetworkFunction</returns>
			std::string ToString() const
			{
				std::string result = "$" + functionName;

				for (const auto& param : params)
				{
					result += "|" + param;
				}

				return result;
			}
		
		};
	}
}
