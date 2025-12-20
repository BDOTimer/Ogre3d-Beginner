///----------------------------------------------------------------------------|
/// "ui.h"
/// Автор: Royal_X
///----------------------------------------------------------------------------:
#ifndef UI_H
#define UI_H
#include "config-game.h"

/*  
	Mods for TextBox
	OgreTrays.h:
    Line 347: 
    inline friend void applyTextBoxMods(TextBox* tb);
*/ 
void OgreBites::applyTextBoxMods(OgreBites::TextBox* tb)
{
    if (!tb) return;

    if (tb->mTextArea)
        tb->mTextArea->setFontName("JetBrainsMonoFont");
    if (tb->mScrollTrack)
		tb->mScrollTrack->setVisible(false);

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
 
        int mWidthS      = 110;
        int mWidthL      = 350;
        int mShortHeight =  34;
        int mLongHeight  = 180;
 
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
            applyTextBoxMods(mTextBox);
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

	class ScoreLabel
	{
	private:
		TrayManager*    mTrayMgr;
	    int		        mScore{0};
		Label*  mLabel{ nullptr };
	
	    void createScoreLabel()
	    {
	        String scoreText = std::format("{:07}", mScore);
	        mLabel = mTrayMgr->createLabel(
	            TrayLocation::TL_TOPRIGHT,
	            "ScoreLabel",
	            scoreText,
				110.0f // same width as mWidthS in ClickableTextBox
	        );
		}
		void updateDisplay()
	    {
			if (mLabel == nullptr) return;
	        String scoreText = std::format("{:07}", mScore);
			mLabel->setCaption(scoreText);
		}
	public:
	    ScoreLabel(TrayManager* trayManager)
	        : mTrayMgr(trayManager)
	    {
	        createScoreLabel();
	    }
	    ~ScoreLabel()
	    {
	    }
	    void add(int points)
	    {
			mScore = Math::Clamp(mScore + points, 0, 9999999);
	        updateDisplay();
	    }
	    void set(int points)
	    {
			mScore = Math::Clamp(points, 0, 9999999);
	        updateDisplay();
		}
	    void reset()
	    {
	        mScore = 0;
	        updateDisplay();
	    }
	    int get() const
	    {
	        return mScore;
	    }
	};

    ///------------------------------------------------------------------------|
    /// UI
    ///--------------------------------------------------------------------- UI:
    struct  UI : Base
    {       UI()
            {   std::cout << "Объект UI создан!\n";
            }
           ~UI()
            {   trayMgr = nullptr;
            }
    
        OgreBites::TrayManager*        		trayMgr;
        std::unique_ptr<ClickableTextBox>  	ctb;
		std::unique_ptr<ScoreLabel>			score;

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

            trayMgr = new OgreBites::TrayManager("UI", mWindow);

            ctx->addInputListener(trayMgr);
            trayMgr->hideCursor();
 
            ctb = std::make_unique<ClickableTextBox>(trayMgr);
            ctb->setText(
                "", 
                "CURSOR : LEFT, RIGHT\n"
                "UP     : Рофлить жемчуг\n"
                "DOWN   : Бросить жемчуг\n"
                "F5, F6 : Вращение сцены\n"
                "SPACE  : Пауза\n"
                "ESCAPE : Выход из игры\n"
            );
            ctb->setCaption("F1::Help");

			score = std::make_unique<ScoreLabel>(trayMgr);

			// Test
			/*score->set(100500);
			score->add(250);
			score->reset();
			score->add(450);*/
        }
    };
}
 
 
#endif // UI_H
