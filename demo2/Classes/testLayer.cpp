//
//  testLayer.cpp
//  HelloCpp
//
//  Created by Yang Chao (wantnon) on 13-11-6.
//
//

#include "testLayer.h"

bool CtestLayer::init(){
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    //enable touch
	setTouchEnabled( true );
    //enable update
    scheduleUpdate();
    //update eye pos
    updateEyePos();
    //-----------------------------
    //root3d
    m_root3d=new Cc3dRoot();
    m_root3d->autorelease();
    m_root3d->init();
    m_root3d->setNodeName("root3d");
    this->addChild(m_root3d);
    
    //camera
    Cc3dCamera*camera=m_root3d->getCamera3D();
    camera->setEyePos(cc3dv4(0, 0, m_r, 1));
    camera->setCenter(cc3dv4(0, 0, 0, 1));
    camera->setUp(cc3dv4(0, 1, 0, 0));
    camera->setProjectionMode(ec3dPerspectiveMode);
    //lightSource
    Cc3dLightSource*lightSource=new Cc3dLightSource();
    lightSource->autorelease();
    lightSource->init();
    m_root3d->addChild(lightSource);
    lightSource->setAmbient(cc3dv4(0.8, 0.8, 0.8, 1));
    lightSource->setPosition3D(cc3dv4(60, 90, 120, 1));
    //program
    Cc3dProgram*program=c3dGetProgram_c3dClassicLighting();

    //actor3D
    m_actor3D=c3dSimpleLoadActor("toolKitRes/model/apple_cfc");
    m_actor3D->setLightSource(lightSource);
    m_actor3D->setCamera3D(camera);
    m_actor3D->setPassUnifoCallback(passUnifoCallback_classicLighting);
    m_actor3D->setProgram(program);
    m_actor3D->setNodeName("actor3D");
    m_root3d->addChild(m_actor3D,0);
        
    m_actor3D->scale3D(0.05, 0.05, 0.05);
    m_actor3D->setPosition3D(Cc3dVector4(0,-1.5,0,1));
    
    //submit

    m_actor3D->submit(GL_STATIC_DRAW);
    

    

    //controlButton_swithProjMode
    {
        CCScale9Sprite* btnUp=CCScale9Sprite::create("button.png");
        CCScale9Sprite* btnDn=CCScale9Sprite::create("button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("proj mode", "Helvetica", 30);
        CCControlButton* controlButton=CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(180,80));
        controlButton->setPosition(ccp(400,100));
        controlButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler)(&CtestLayer::switchProjModeCallBack), CCControlEventTouchDown);
        this->addChild(controlButton);
        m_controlButton_swithProjMode=controlButton;
        
    }
    
    //controlButton_transform
    {
        CCScale9Sprite* btnUp=CCScale9Sprite::create("button.png");
        CCScale9Sprite* btnDn=CCScale9Sprite::create("button_dn.png");
        CCLabelTTF*title=CCLabelTTF::create("transform", "Helvetica", 30);
        CCControlButton* controlButton=CCControlButton::create(title, btnUp);
        controlButton->setBackgroundSpriteForState(btnDn,CCControlStateHighlighted);
        controlButton->setPreferredSize(CCSize(180,80));
        controlButton->setPosition(ccp(700,100));
        controlButton->addTargetWithActionForControlEvents(this, (SEL_CCControlHandler)(&CtestLayer::transformCallBack), CCControlEventTouchDown);
        this->addChild(controlButton);
        m_controlButton_transform=controlButton;
        
    }
    //projection mode label
    m_pLabel=CCLabelTTF::create("proj mode: Perspective", "Arial", 35);
    m_pLabel->setPosition(ccp(origin.x + visibleSize.width*(3.0/4),
                              origin.y + visibleSize.height - m_pLabel->getContentSize().height-100));
    this->addChild(m_pLabel, 1);

    
    return true;
}

void CtestLayer::updateEyePos(){
    float cosA=cosf(m_A*M_PI/180);
    float sinA=sinf(m_A*M_PI/180);
    float cosB=cosf(m_B*M_PI/180);
    float sinB=sinf(m_B*M_PI/180);
    m_eyePos.setx(m_r*cosB*sinA);
    m_eyePos.sety(m_r*sinB);
    m_eyePos.setz(m_r*cosB*cosA);
    m_eyePos.setw(1);
}

void CtestLayer::switchProjModeCallBack(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent){
    Cc3dCamera*camera=m_root3d->getCamera3D();
    switch(camera->getProjectionMode())
    {
        case ec3dPerspectiveMode:{
            camera->setProjectionMode(ec3dOrthographicMode);
            m_pLabel->setString("proj mode: Orthographic");
        }break;
        case ec3dOrthographicMode:{
            camera->setProjectionMode(ec3dPerspectiveMode);
            m_pLabel->setString("proj mode: Perspective");
        }break;
            
    }
}
void CtestLayer::transformCallBack(CCObject *senderz, CCControlEvent controlEvent){
    if(m_isDoUpdate){
        //restore inital matrix
        m_actor3D->setTransform3D(m_initialMat);
        //stop update
        m_isDoUpdate=false;
        
    }else{
        //store inital matrix
        m_initialMat=m_actor3D->getTransform3D();
        //start update
        m_isDoUpdate=true;
    }
}
void CtestLayer::update(float dt){
    if(m_isDoUpdate==false)return;
    m_actor3D->rotate3D(cc3dv4(0, 1, 0, 0), 120*dt);

    
}

void CtestLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint pointInWinSpace = touch->getLocationInView();
        
        //----update mos
        m_mosPosf=m_mosPos;
        m_mosPos=pointInWinSpace;
        
    }
}
void CtestLayer::ccTouchesMoved(cocos2d::CCSet* touches , cocos2d::CCEvent* event)
{
    
    CCSetIterator it;
    CCTouch* touch;
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint pointInWinSpace = touch->getLocationInView();
       
        //----update mos
        m_mosPosf=m_mosPos;
        m_mosPos=pointInWinSpace;
        //----update eyePos
        m_A+=-(m_mosPos.x-m_mosPosf.x)*0.4;
        m_B+=(m_mosPos.y-m_mosPosf.y)*0.4;
        if(m_B>89.9)m_B=89.9;
        if(m_B<-89.9)m_B=-89.9;
        updateEyePos();
        m_root3d->getCamera3D()->setEyePos(m_eyePos);
    }
}

void CtestLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    
    CCSetIterator it;
    CCTouch* touch;
	for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        if(!touch)
            break;
        
        CCPoint pointInWinSpace = touch->getLocationInView();
        //note: for 3d mode, CCDirector::convertToGL() not works as we expected
        //  CCPoint pointInWinSpace = CCDirector::sharedDirector()->convertToGL(pointInWinSpace);
        
        //----update mos
        m_mosPosf=m_mosPos;
        m_mosPos=pointInWinSpace;
        
    }
}

