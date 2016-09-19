#include "pch.h"
#include "cocos2d.h"
#include "WinRTSaveImage.h"
#include <ppltasks.h>
#include <filesystem>

using namespace Windows::Storage;
using namespace concurrency;
using namespace std::tr2::sys;
using namespace cocos2d;

static std::wstring_convert<std::codecvt_utf8<wchar_t>> s_converter;
static std::mutex s_mutex;

// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
static inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
	std::string ret = path;
	size_t len = ret.length();
	for (size_t i = 0; i < len; ++i)
	{
		if (ret[i] == '\\')
		{
			ret[i] = '/';
		}
	}
	return ret;
}

// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
static inline std::string convertPathFormatToWin32Style(const std::string& path)
{
	std::string ret = path;
	size_t len = ret.length();
	for (size_t i = 0; i < len; ++i)
	{
		if (ret[i] == '/')
		{
			ret[i] = '\\';
		}
	}
	return ret;
}

std::string platformStringToString(Platform::String^ s) {
	if (s == nullptr) {
		return std::string("");
	}

	std::lock_guard<std::mutex> lock(s_mutex);
	return s_converter.to_bytes(s->Data());
}

Platform::String^ stringToPlatformString(const std::string& s) {
	if (s.empty()) {
		return ref new Platform::String();
	}
	std::lock_guard<std::mutex> lock(s_mutex);
	std::wstring converted = s_converter.from_bytes(s);
	return ref new Platform::String(converted.c_str());
}

std::string getFilePath(Platform::String^ s)
{

	if (s == nullptr) {
		return "";
	}

	return convertPathFormatToUnixStyle(platformStringToString(s));
}


Platform::String^ getWin32Path(const std::string& s) {

	if (s.empty()) {
		return ref new Platform::String();
	}

	return stringToPlatformString(convertPathFormatToWin32Style(s));
}

Platform::String^ getFilenameFromPath(const std::string& s) {

	if (s.empty()) {
		return ref new Platform::String();
	}

	path path(s);

#if (_MSC_VER >= 1900)
	return ref new Platform::String(path.filename().c_str());
#else
	return stringToPlatformString(path.filename());
#endif
}

// Moves file at originalPath to the Windows 10 User's Pictures folder. Original file is deleted.
// callback function will be called on the cocos2d-x thread
void WinRTSaveImage::SaveToPicturesFolder(const std::string& originalPath, const std::function<void(bool, const std::string&, const std::string&)>& callback)
{
	auto t = create_task([=]()
	{
		auto folder = Windows::Storage::KnownFolders::PicturesLibrary;
		Platform::String^ inPath = getWin32Path(originalPath);
		Platform::String^ saveFilename = getFilenameFromPath(originalPath);

		auto t = create_task(StorageFile::GetFileFromPathAsync(inPath)).then([=](StorageFile^ originalFile)
		{
			return create_task(originalFile->CopyAsync(folder, saveFilename, NameCollisionOption::ReplaceExisting)).then([=](StorageFile^ savedFile)
			{
				return savedFile;
			});
		});

		t.then([=](task<StorageFile^> previousTask)
		{
			bool result = false;
			std::string message = "";
			std::string path = "";
			// check if any exceptions occured in the above async functions
			try
			{
				auto savedFile = previousTask.get();
				path = platformStringToString(savedFile->Path);
				result = true;
			}
			catch (Platform::COMException^ e)
			{
				message = platformStringToString(e->Message);
			}

			// delete the original file
			if(CCFileUtils::getInstance()->isFileExist(originalPath))
			{
				CCFileUtils::getInstance()->removeFile(originalPath);
			}

			// Execute callback on cocos2d-x thread
			cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
				callback(result, path, message);
			});
		});
	});
}
