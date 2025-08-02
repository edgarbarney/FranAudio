// FranticDreamer 2022-2025
#pragma once

#include <string>

namespace FranAudio
{
	/// <summary>
	/// Logger class for logging messages and errors.
	/// 
	/// To disable logging, define <i>FRANAUDIO_DISABLE_LOGGING</i>.
	/// </summary>
	class Logger
	{
	public:
		/// <summary>
		/// Logs a message to the console.
		/// 
		/// If line is true, a new line will be added after the message.
		/// 
		/// <para>
		/// This log message will have a preceding '[INFO]' tag
		/// and will be printed in cyan.
		/// </para>
		/// 
		/// </summary>
		/// <param name="message">Message to log</param>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogMessage(const std::string& message, bool newLine = true);

		/// <summary>
		/// Logs an error message to the console.
		/// 
		/// If line is true, a new line will be added after the message.
		/// 
		/// <para>
		/// This log message will have a preceding '[ERROR]' tag 
		/// and will be printed in red.
		/// </para>
		/// 
		/// </summary>
		/// <param name="message">Error message to log</param>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogError(const std::string& message, bool newLine = true);

		/// <summary>
		/// Logs a warning message to the console.
		/// 
		/// If line is true, a new line will be added after the message.
		/// 
		/// <para>
		/// This log message will have a preceding '[WARNING]' tag
		/// and will be printed in yellow.
		/// </para>
		/// 
		/// </summary>
		/// <param name="message">Warning message to log</param>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogWarning(const std::string& message, bool newLine = true);

		/// <summary>
		/// Logs a success message to the console.
		/// 
		/// If line is true, a new line will be added after the message.
		/// 
		/// <para>
		/// This log message will have a preceding '[SUCCESS]' tag
		/// and will be printed in green.
		/// </para>
		/// 
		/// </summary>
		/// <param name="message">Success message to log</param>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogSuccess(const std::string& message, bool newLine = true);

		/// <summary>
		/// Logs a generic message to the console.
		/// 
		/// If line is true, a new line will be added after the message.
		/// 
		/// <para>
		/// This log message will have no preceding tag
		/// and will be printed in default colour.
		/// </para>
		/// 
		/// </summary>
		/// <param name="message">Message to log</param>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogGeneric(const std::string& message, bool newLine = true);

		/// <summary>
		/// Logs a separator to the console.
		/// </summary>
		/// <param name="newLine">Should add a newline? (Default = true)</param>
		static void LogSeperator(bool newLine = true);

		/// <summary>
		/// Logs a new line to the console.
		/// </summary>
		static void LogNewline();
	};
}