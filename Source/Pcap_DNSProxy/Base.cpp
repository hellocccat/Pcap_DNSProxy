﻿// This code is part of Pcap_DNSProxy
// A local DNS server based on WinPcap and LibPcap
// Copyright (C) 2012-2016 Chengr28
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "Base.h"

//Global variables
extern CONFIGURATION_TABLE Parameter;
extern GLOBAL_STATUS GlobalRunningStatus;

//Check empty buffer
bool CheckEmptyBuffer(
	const void * const Buffer, 
	const size_t Length)
{
//Null pointer
	if (Buffer == nullptr)
	{
		return false;
	}
	else {
	//Scan all data.
		for (size_t Index = 0;Index < Length;++Index)
		{
			if (*(((uint8_t *)Buffer) + Index) != 0)
				return false;
		}
	}

	return true;
}

//Convert multiple bytes to wide char string
bool MBS_To_WCS_String(
	const uint8_t * const Buffer, 
	const size_t MaxLen, 
	std::wstring &Target)
{
//Check buffer.
	Target.clear();
	if (Buffer == nullptr || MaxLen == 0)
		return false;
	auto Length = strnlen_s((const char *)Buffer, MaxLen);
	if (Length == 0 || CheckEmptyBuffer(Buffer, Length))
		return false;

//Convert string.
	std::shared_ptr<wchar_t> TargetPTR(new wchar_t[Length + PADDING_RESERVED_BYTES]());
	wmemset(TargetPTR.get(), 0, Length + PADDING_RESERVED_BYTES);
#if defined(PLATFORM_WIN)
	if (MultiByteToWideChar(
			CP_ACP, 
			0, 
			(LPCCH)Buffer, 
			MBSTOWCS_NULLTERMINATE, 
			TargetPTR.get(), 
			(int)(Length + PADDING_RESERVED_BYTES)) == 0)
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS))
	if (mbstowcs(TargetPTR.get(), (const char *)Buffer, Length + PADDING_RESERVED_BYTES) == (size_t)RETURN_ERROR)
#endif
	{
		return false;
	}
	else {
		if (wcsnlen_s(TargetPTR.get(), Length + PADDING_RESERVED_BYTES) == 0)
			return false;
		else 
			Target = TargetPTR.get();
	}

	return true;
}

//Convert wide char string to multiple bytes
bool WCS_To_MBS_String(
	const wchar_t * const Buffer, 
	const size_t MaxLen, 
	std::string &Target)
{
//Check buffer pointer.
	Target.clear();
	if (Buffer == nullptr || MaxLen == 0)
		return false;
	auto Length = wcsnlen_s(Buffer, MaxLen);
	if (Length == 0 || CheckEmptyBuffer(Buffer, sizeof(wchar_t) * Length))
		return false;

//Convert string.
	std::shared_ptr<uint8_t> TargetPTR(new uint8_t[Length + PADDING_RESERVED_BYTES]());
	memset(TargetPTR.get(), 0, Length + PADDING_RESERVED_BYTES);
#if defined(PLATFORM_WIN)
	if (WideCharToMultiByte(
			CP_ACP, 
			0, 
			Buffer, 
			MBSTOWCS_NULLTERMINATE, 
			(LPSTR)TargetPTR.get(), 
			(int)(Length + PADDING_RESERVED_BYTES), 
			nullptr, 
			nullptr) == 0)
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS))
	if (wcstombs((char *)TargetPTR.get(), Buffer, Length + PADDING_RESERVED_BYTES) == (size_t)RETURN_ERROR)
#endif
	{
		return false;
	}
	else {
		if (strnlen_s((const char *)TargetPTR.get(), Length + PADDING_RESERVED_BYTES) == 0)
			return false;
		else 
			Target = (const char *)TargetPTR.get();
	}

	return true;
}

//Convert lowercase/uppercase words to uppercase/lowercase words(C-Style version)
void CaseConvert(
	uint8_t * const Buffer, 
	const size_t Length, 
	const bool IsLowerToUpper)
{
//Null pointer
	if (Buffer == nullptr || Length == 0)
	{
		return;
	}
	else {
	//Convert words.
		for (size_t Index = 0;Index < Length;++Index)
		{
		//Lowercase to uppercase
			if (IsLowerToUpper)
				Buffer[Index] = (uint8_t)toupper(Buffer[Index]);
		//Uppercase to lowercase
			else 
				Buffer[Index] = (uint8_t)tolower(Buffer[Index]);
		}
	}

	return;
}

//Convert lowercase/uppercase words to uppercase/lowercase words(C++ string version)
void CaseConvert(
	std::string &Buffer, 
	const bool IsLowerToUpper)
{
	for (auto &StringIter:Buffer)
	{
	//Lowercase to uppercase
		if (IsLowerToUpper)
			StringIter = (char)toupper(StringIter);
	//Uppercase to lowercase
		else 
			StringIter = (char)tolower(StringIter);
	}

	return;
}

//Convert lowercase/uppercase words to uppercase/lowercase words(C++ wstring version)
void CaseConvert(
	std::wstring &Buffer, 
	const bool IsLowerToUpper)
{
	for (auto &StringIter:Buffer)
	{
	//Lowercase to uppercase
		if (IsLowerToUpper)
			StringIter = (wchar_t)toupper(StringIter);
	//Uppercase to lowercase
		else 
			StringIter = (wchar_t)tolower(StringIter);
	}

	return;
}

//Make string reversed
void MakeStringReversed(
	std::string &String)
{
//String check
	if (String.length() <= 1U)
		return;

//Make string reversed
	uint8_t StringIter = 0;
	for (size_t Index = 0;Index < String.length() / 2U;++Index)
	{
		StringIter = String.at(String.length() - 1U - Index);
		String.at(String.length() - 1U - Index) = String.at(Index);
		String.at(Index) = StringIter;
	}

	return;
}

//Make string reversed
void MakeStringReversed(
	std::wstring &String)
{
//String check
	if (String.length() <= 1U)
		return;

//Make string reversed
	wchar_t StringIter = 0;
	for (size_t Index = 0;Index < String.length() / 2U;++Index)
	{
		StringIter = String.at(String.length() - 1U - Index);
		String.at(String.length() - 1U - Index) = String.at(Index);
		String.at(Index) = StringIter;
	}

	return;
}

//Reversed string comparing
bool CompareStringReversed(
	const std::string &RuleItem, 
	const std::string &TestItem)
{
	if (!RuleItem.empty() && !TestItem.empty() && TestItem.length() >= RuleItem.length() && memcmp(RuleItem.c_str(), TestItem.c_str(), RuleItem.length()) == 0)
		return true;

	return false;
}

//Reversed string comparing
bool CompareStringReversed(
	const wchar_t * const RuleItem, 
	const wchar_t * const TestItem)
{
	std::wstring InnerRuleItem(RuleItem), InnerTestItem(TestItem);

//Length check
	if (InnerRuleItem.empty() || InnerTestItem.empty() || InnerTestItem.length() < InnerRuleItem.length())
	{
		return false;
	}
	else {
	//Make string reversed to compare.
		MakeStringReversed(InnerRuleItem);
		MakeStringReversed(InnerTestItem);

	//Compare each other.
		if (memcmp(InnerRuleItem.c_str(), InnerTestItem.c_str(), InnerRuleItem.length()) == 0)
			return true;
	}

	return false;
}

//Sort compare(IPFilter)
bool SortCompare_IPFilter(
	const DIFFERNET_FILE_SET_IPFILTER &Begin, 
	const DIFFERNET_FILE_SET_IPFILTER &End)
{
	return Begin.FileIndex < End.FileIndex;
}

//Sort compare(Hosts)
bool SortCompare_Hosts(
	const DIFFERNET_FILE_SET_HOSTS &Begin, 
	const DIFFERNET_FILE_SET_HOSTS &End)
{
	return Begin.FileIndex < End.FileIndex;
}

//Base64 encoding
//Base64 encoding or decoding is from https://github.com/zhicheng/base64.
size_t Base64_Encode(
	uint8_t * const Input, 
	const size_t Length, 
	uint8_t * const Output, 
	const size_t OutputSize)
{
//Initialization
	if (Length == 0)
		return 0;
	size_t Index[]{0, 0, 0};
	memset(Output, 0, OutputSize);

//Convert from binary to Base64.
	for (Index[0] = Index[1U] = 0;Index[0] < Length;++Index[0])
	{
	//From 6/gcd(6, 8)
		Index[2U] = Index[0] % 3U;
		switch (Index[2U])
		{
			case 0:
			{
				Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[(Input[Index[0]] >> 2U) & 0x3F];
				continue;
			}
			case 1U:
			{
				Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[((Input[Index[0] - 1U] & 0x3) << 4U) + ((Input[Index[0]] >> 4U) & 0xF)];
				continue;
			}
			case 2U:
			{
				Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[((Input[Index[0] - 1U] & 0xF) << 2U) + ((Input[Index[0]] >> 6U) & 0x3)];
				Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[Input[Index[0]] & 0x3F];
			}
		}
	}

//Move back.
	Index[0] -= 1U;

//Check the last and add padding.
	if ((Index[0] % 3U) == 0)
	{
		Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[(Input[Index[0]] & 0x3) << 4U];
		Output[Index[1U]++] = BASE64_PAD;
		Output[Index[1U]++] = BASE64_PAD;
	}
	else if ((Index[0] % 3U) == 1U)
	{
		Output[Index[1U]++] = GlobalRunningStatus.Base64_EncodeTable[(Input[Index[0]] & 0xF) << 2U];
		Output[Index[1U]++] = BASE64_PAD;
	}

	return strnlen_s((const char *)Output, OutputSize);
}

//Base64 decoding
//Base64 encoding or decoding is from https://github.com/zhicheng/base64.
size_t Base64_Decode(
	uint8_t *Input, 
	const size_t Length, 
	uint8_t *Output, 
	const size_t OutputSize)
{
//Initialization
	if (Length == 0)
		return 0;
	size_t Index[]{0, 0, 0};
	int StringIter = 0;
	memset(Output, 0, OutputSize);

//Convert from Base64 to binary.
	for (Index[0] = Index[1U] = 0;Index[0] < Length;++Index[0])
	{
		StringIter = 0;
		Index[2U] = Index[0] % 4U;
		if (Input[Index[0]] == (uint8_t)BASE64_PAD)
			return strnlen_s((const char *)Output, OutputSize);
		if (Input[Index[0]] < BASE64_DECODE_FIRST || Input[Index[0]] > BASE64_DECODE_LAST || 
			(StringIter = GlobalRunningStatus.Base64_DecodeTable[Input[Index[0]] - BASE64_DECODE_FIRST]) == -1)
				return 0;
		switch (Index[2U])
		{
			case 0:
			{
				Output[Index[1U]] = (uint8_t)(StringIter << 2U);
				continue;
			}
			case 1U:
			{
				Output[Index[1U]++] += (StringIter >> 4U) & 0x3;

			//If not last char with padding
				if (Index[0] < (Length - 3U) || Input[Length - 2U] != (uint8_t)BASE64_PAD)
					Output[Index[1U]] = (StringIter & 0xF) << 4U;
				continue;
			}
			case 2U:
			{
				Output[Index[1U]++] += (StringIter >> 2U) & 0xF;

			//If not last char with padding
				if (Index[0] < (Length - 2U) || Input[Length - 1U] != (uint8_t)BASE64_PAD)
					Output[Index[1U]] = (StringIter & 0x3) << 6U;
				continue;
			}
			case 3U:
			{
				Output[Index[1U]++] += (uint8_t)StringIter;
			}
		}
	}

	return strnlen_s((const char *)Output, OutputSize);
}

#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS))
//Increase time with milliseconds
uint64_t IncreaseMillisecondTime(
	const uint64_t CurrentTime, 
	const timeval IncreaseTime)
{
	return CurrentTime + IncreaseTime.tv_sec * SECOND_TO_MILLISECOND + IncreaseTime.tv_usec / MICROSECOND_TO_MILLISECOND;
}

//Get current system time
uint64_t GetCurrentSystemTime(
	void)
{
	timeval CurrentTime;
	memset(&CurrentTime, 0, sizeof(CurrentTime));
	if (gettimeofday(&CurrentTime, nullptr) == 0)
		return IncreaseMillisecondTime(0, CurrentTime);

	return 0;
}
#endif
