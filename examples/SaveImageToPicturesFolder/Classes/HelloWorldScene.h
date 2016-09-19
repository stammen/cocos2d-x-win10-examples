#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // menu selector callbacks
	void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);

private:
	void saveScreenToPicturesFolderCallback(cocos2d::Ref* pSender);

};

#endif // __HELLOWORLD_SCENE_H__
