#pragma once

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

#include <codecvt>
#include <string>
#include <mutex>
#include <functional>

class WinRTSaveImage
{
public:
	// Moves file at originalPath to the Windows 10 User's Pictures folder. Original file is deleted.
	// callback function will be called on the cocos2d-x thread
	static void SaveToPicturesFolder(const std::string& originalPath, const std::function<void(bool, const std::string& savedPath, const std::string& error)>& callback = nullptr);

private:
	// string conversion functions optimized for Windows Phone
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> s_converter;
	static std::mutex s_mutex;
};

#endif

