///----------------------------------------------------------------------------|
/// "ui.h"
/// Автор: Royal_X
///----------------------------------------------------------------------------:
#ifndef UI_H
#define UI_H
#include "config-game.h"

void OgreBites::setTextBoxFont(OgreBites::TextBox* tb, 
                                const Ogre::String& fontName)
{
    if (!tb) return;

    if (tb->mTextArea)
        tb->mTextArea->setFontName(fontName);

    //if (tb->mCaptionTextArea) tb->mCaptionTextArea->setFontName(fontName);
}
 
///---------|
/// Models. |
///---------:
namespace mdl
{   
    //using namespace Ogre;
    //using namespace OgreBites;

    ///------------------------------------------------------------------------|
    /// Автор: Royal_X
    ///------------------------------------------------------- ClickableTextBox:
    class ClickableTextBox
    {
    private:
        TrayManager* mTrayMgr;
        TextBox*     mTextBox;
 
        DisplayString mTextShort;
        DisplayString mTextLong;
        DisplayString mCaption;
 
        int mWidthS      =  80;
        int mWidthL      = 350;
        int mShortHeight =  34;
        int mLongHeight  = 130;
 
        bool mIsLongText = false;

        void createTextBox()
        {
            static int id = 0;
            String name = "ClickableTextBox_" + std::to_string(id++);
            mTextBox = mTrayMgr->createTextBox(
                TrayLocation::TL_TOPLEFT,
                name,
                mCaption,
                float(mIsLongText ? mWidthL     : mWidthS),
                float(mIsLongText ? mLongHeight : mShortHeight)
            );
            mTextBox->setText(mIsLongText ? mTextLong : mTextShort);
            setTextBoxFont (mTextBox, "ArialFont");
        }

        void destroyTextbox()
        {   if (mTrayMgr && mTextBox)
            {   mTrayMgr->destroyWidget(mTextBox);
                mTextBox = nullptr;
            }
        }

        void toggleText()
        {   if (mTextBox == nullptr) return;
            mIsLongText = !mIsLongText;
            destroyTextbox();
            createTextBox();
        }

    public:
        ClickableTextBox(TrayManager* trayManager)
            : mTrayMgr(trayManager)
            , mTextShort("")
            , mTextLong("")
            , mCaption("")
        {
            createTextBox();
        }

        ClickableTextBox(TrayManager* trayManager, 
            const DisplayString& textShort,
            const DisplayString& textLong,
            const DisplayString& caption
            )
            : mTrayMgr(trayManager)
            , mTextShort(textShort)
            , mTextLong(textLong)
            , mCaption(caption)
        {
            createTextBox();
        }
        ~ClickableTextBox()
        { 
        }

        void setText(const DisplayString& textShort, const DisplayString& textLong)
        {
            mTextShort = textShort;
            mTextLong  = textLong;
            if (mTextBox)
                mTextBox->setText(mIsLongText ? mTextLong : mTextShort);
        }

        void setCaption(const DisplayString& caption)
        {
            mCaption = caption;
            if (mTextBox)
                mTextBox->setCaption(mCaption);
        }

        bool keyPressed(const KeyboardEvent& evt)
        {   switch(evt.keysym.sym)
            {   case OgreBites::SDLK_F1:
                {   toggleText();
                    return true;
                }
            }
            return false;
        }

        bool mousePressed(const OgreBites::MouseButtonEvent& evt)
        {
            if (evt.button != OgreBites::BUTTON_LEFT)
                return false;
 
            if (mTextBox && mTextBox->isVisible())
            {
                int currentWidth  = mIsLongText ? mWidthL     : mWidthS;
                int currentHeight = mIsLongText ? mLongHeight : mShortHeight;

                if (evt.x <= currentWidth &&
                    evt.y <= currentHeight)
                {
                    toggleText();
                    return true;
                }
            }
            return false;
        }
    };

    ///------------------------------------------------------------------------|
    /// UI
    ///--------------------------------------------------------------------- UI:
    struct  UI : Base
    {       UI()
            {   std::cout << "Объект UI создан! Автор: Royal_X\n";
            }
    
        std::unique_ptr<OgreBites::TrayManager> trayMgr;
        std::unique_ptr<ClickableTextBox>           ctb;

        bool keyPressed(const KeyboardEvent& evt)
        {   return ctb->keyPressed(evt);
        }

        bool mousePressed(const OgreBites::MouseButtonEvent& evt)
        {   return ctb->mousePressed(evt);
        }

        void setup()
        {   
            OverlaySystem*  overlaySystem = ctx->getOverlaySystem();

            scnMgr->addRenderQueueListener(overlaySystem);
 
            Ogre::RenderWindow* mWindow = ctx->getRenderWindow();

            trayMgr = std::make_unique<OgreBites::TrayManager>("UI", mWindow);

            ctx->addInputListener(trayMgr.get());
            trayMgr->hideCursor();
 
            ctb = std::make_unique<ClickableTextBox>(trayMgr.get());
            ctb->setText(
                "Short text", 
                "CURSOR : LEFT, RIGHT, DOWN\n"
                "SPACE  : Сдвинуть жемчуг\n"
                "F5, F6 : Вращение сцены\n");
            ctb->setCaption("F1::Help");
        }
    };
}
 
 
#endif // UI_H
