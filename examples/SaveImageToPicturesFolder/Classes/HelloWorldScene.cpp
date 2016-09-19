#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
#include "WinRTSaveImage.h"
#endif

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add "HelloWorld" splash screen"
	auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
	sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
	// add the Save Screen to Pictures Folder menu
	auto saveMenu = Menu::create();
	auto saveLabel = Label::createWithTTF("Save Screen to Pictures Folder", "fonts/Marker Felt.ttf", 24);
	auto menuItemLabel = MenuItemLabel::create(saveLabel, CC_CALLBACK_1(HelloWorld::saveScreenToPicturesFolderCallback, this));
	menuItemLabel->setPosition(Vec2::ZERO);
	saveMenu->addChild(menuItemLabel, 1);
	this->addChild(saveMenu, 0);

    return true;
}

void HelloWorld::saveScreenToPicturesFolderCallback(Ref* pSender)
{
	std::string fileName = "CaptureScreenTest.png";


	utils::captureScreen([this, fileName](bool succeed, const std::string& outputFile)
	{
		if (!succeed)
		{
			log("Capture screen failed.");
			return;
		}

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		// we need to copy the image file saved by cocos2d-x to the User's Pictures folder
		auto saveCallback = [this, outputFile](bool result, const std::string& picturesFolderFile, const std::string& errorMessage) {
			if (result)
			{
				CCLOG("Screen capture saved to: %s", picturesFolderFile.c_str());
			}
			else
			{
				CCLOG("Error %s saving screen capture.", errorMessage.c_str());
			}
		};

		WinRTSaveImage::SaveToPicturesFolder(outputFile, saveCallback);
#endif
	}, fileName);
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
}
