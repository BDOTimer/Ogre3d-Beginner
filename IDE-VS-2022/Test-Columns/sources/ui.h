///----------------------------------------------------------------------------|
/// "ui.h"
/// Автор: Royal_X
/// Проверка пулов...
///----------------------------------------------------------------------------:
#ifndef UI_H
#define UI_H
#include "config-game.h"


namespace myl
{
    using namespace ::Ogre;
    using namespace ::OgreBites;

    struct ToolFonts
    {
        inline static const char* NAMEFONT{"JetBrainsMonoFont"};

        static void setFont(Ogre::OverlayElement* elem,
                      const Ogre::String& fontName, 
                      const ColourValue&  color = ColourValue{0, 0, 0.2f})
        {
            if (elem->getTypeName() == "TextArea")
            {
                Ogre::TextAreaOverlayElement* textElem = 
                    static_cast<Ogre::TextAreaOverlayElement*>(elem);
                textElem->setFontName(fontName);
                textElem->setCharHeight(18);
                textElem->setColour(color);
                return;
            }
        
            Ogre::OverlayContainer* container
                = dynamic_cast<Ogre::OverlayContainer*>(elem);
            Ogre::OverlayContainer::ChildMap children
                = container->getChildren();

            for (auto& child : children)
            {
                Ogre::OverlayElement* oe = child.second;
        
                if (oe->getTypeName() == "TextArea")
                {   setFont(oe, fontName);
                }
            }
        }

        static void createTextElement()///-///////////////////////////////////-?
        {   Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
            Ogre::OverlayElement* text
                = om.createOverlayElement("TextArea", "TestText");

            Ogre::TextAreaOverlayElement* textElem = 
                    static_cast<Ogre::TextAreaOverlayElement*>(text);
            textElem->setFontName("JetBrainsMonoFont");
            textElem->setCharHeight(16);
            textElem->setCaption("Привет, мир!");
            textElem->setColour(Ogre::ColourValue(1, 1, 1, 1));
            textElem->setPosition(100, 100);
            textElem->setDimensions(300, 50);

            //Ogre::Overlay* overlay = om.create("MyOverlay");
            //overlay->add2D(textElem);  // Добавляем элемент как 2D слой
            //overlay->show(); 
        }
    };
}


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
        int mWidthL      = 360;
        int mShortHeight =  34;
        int mLongHeight  = 250;
 
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
            mIsLongText  = !mIsLongText;
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
	
	    void createScoreLabel(TrayManager* tmg, std::string name)
	    {   mTrayMgr = tmg;

	        mLabel = mTrayMgr->createLabel(
	            TrayLocation::TL_TOPRIGHT,
	            name,
	            std::to_string(mScore),
				110.0f // same width as mWidthS in ClickableTextBox
	        );
		}
		void updateDisplay()
	    {
			if (mLabel == nullptr) return;
	        mLabel->setCaption(std::to_string(mScore));
		}
	public:
        ScoreLabel() = default;
	    ScoreLabel(TrayManager* trayManager)
	        : mTrayMgr(trayManager)
	    {
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

        friend struct ScoreLabels;
	};


    ///------------------------------------------------------------------------|
    /// ScoreLabels
    ///------------------------------------------------------------ ScoreLabels:
    struct  ScoreLabels
	{       ScoreLabels()
            {
            }

        void setScore(Args_t a)
        {   const auto& id    = (unsigned)a[0] ^ 1;
            const auto& score = (unsigned)a[1];
            labels[id].set(score);
        }

    private:
        std::array<ScoreLabel, 2> labels;

        void setup(       TrayManager* tM)
        {   labels[0].createScoreLabel(tM, "ScoreLabel1");
            labels[1].createScoreLabel(tM, "ScoreLabel2");

            addEvent(setScore);
        }

        friend struct UI;
    };


    using namespace OgreBites;
    /// trayMgr->showOkDialog("Внимание", "Сообщение");
    ///------------------------------------------------------------------------|
    /// MenuStart
    ///-------------------------------------------------------------- MenuStart:
    struct MenuStart
    {
        void setup(OgreBites::TrayManager* tM)
        {   
            using T = TrayLocation;

            createMyMaterials   ();
            createCustomTemplate();

            Button* bt;
            Label*  lb;

            const char* NAMEFONT{myl::ToolFonts::NAMEFONT};

            tM->createDecorWidget(
                T::TL_CENTER, "wdMS1", "MyTemplates/FancyFrame"
            );

            lb = tM->createLabel(
                T::TL_CENTER, "lbMS1", "СТАРТ ИГРЫ", 150
            );
            myl::ToolFonts::setFont(lb->
                getOverlayElement(), NAMEFONT, ColourValue{1,1,0});

            tM->createSeparator(T::TL_CENTER, "sp1");

            bt = tM->createButton(
                T::TL_CENTER, "btStart1", "Один Игрок", 150
            );
            myl::ToolFonts::setFont(bt->getOverlayElement(), NAMEFONT);

            bt = tM->createButton(
                T::TL_CENTER, "btStart2", "Два Игрока", 150
            );
            myl::ToolFonts::setFont(bt->getOverlayElement(), NAMEFONT);

            bt = tM->createButton(
                T::TL_CENTER, "btTuning", "Настройки", 150
            );
            myl::ToolFonts::setFont(bt->getOverlayElement(), NAMEFONT);

            tM->createSeparator(T::TL_CENTER, "sp2");
            
            bt = tM->createButton(
                T::TL_CENTER, "btExit", "Выход в ОС", 150
            );
            myl::ToolFonts::setFont(bt->getOverlayElement(), NAMEFONT);

            tM->createDecorWidget(
                T::TL_CENTER, "wdMS2", "MyTemplates/FancyFrame"
            );
        }

        void createCustomTemplate()
        {
            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    
            // 1. Создаем шаблон Overlay элемента (не материал!)
            Ogre::OverlayContainer* fancyFrame =   
                static_cast<Ogre::OverlayContainer*>(
                    om.createOverlayElement("Panel", "MyTemplates/FancyFrame")
            );

            // 2. Присваиваем материал (реальное имя материала)
            fancyFrame->setMaterialName("MyMaterials/FancyBorder");
            fancyFrame->setColour(Ogre::ColourValue(0.2f, 0.4f, 0.8f, 0.7f));
            fancyFrame->setDimensions(100, 100);
        }

        void createMyMaterials()
        {
            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton()
                .create(
                "MyMaterials/FancyBorder", // Имя материала
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
    
            Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
    
            pass->setLightingEnabled  (false);
            pass->setDepthCheckEnabled(false);
            pass->setDepthWriteEnabled(false);
            pass->setDiffuse(ColourValue(0.0f, 0.0f, 0.6f, 0.6f));
            pass->setAmbient(ColourValue(0.0f, 0.0f, 0.8f, 1.0f));
    
            // ИЛИ использовать текстуру
            // pass->createTextureUnitState("my_texture.png");
        }

        bool isMaterialExist(const Ogre::String& name)
        {   return Ogre::MaterialManager::getSingleton().resourceExists(name);
        }

        bool isTemplateExist(const Ogre::String& name)
        {   try
            {   return Ogre::OverlayManager
                           ::getSingleton().getOverlayElement(name) != nullptr;
            }
            catch (...) { return false; }
        }
    };


    ///------------------------------------------------------------------------|
    /// Help
    ///------------------------------------------------------------------- Help:
    struct Help
    {
        OgreBites::TrayManager* trayMgr;
        Button*                  btHelp;
        Button*                    btSM;

        void setup(OgreBites::TrayManager* tM)
        {   
            trayMgr = tM;

            using T = TrayLocation;
            const char* NAMEFONT{myl::ToolFonts::NAMEFONT};

            btHelp = tM->createButton(
                T::TL_TOPLEFT, "btHelp", "F1::Help", 100
            );
            myl::ToolFonts::setFont(btHelp->getOverlayElement(), NAMEFONT);

            btSM = tM->createButton(
                T::TL_TOPLEFT, "bt", "Старт-Меню"
            );
            myl::ToolFonts::setFont(btSM->getOverlayElement(), NAMEFONT);
        }

        void keyPressed(const KeyboardEvent& evt)
        {   switch(evt.keysym.sym)
            {   case OgreBites::SDLK_F1:
                {   showHelp();
                }
            }
        }

        void buttonHit(OgreBites::Button* button) 
        {   if (button->getName() == "btHelp")
            {   showHelp();
            }
        }

        void showHelp()
        {   
            /*
            trayMgr->showOkDialog("Help",
                "'1'      : Новая игра - 1 игрок\n"
                "'2'      : Новая игра - 2 игрока\n"
                "CURSOR   : LEFT/'A', RIGHT/'D'\n"
                "UP  /'W' : Рофлить жемчуг\n"
                "DOWN/'S' : Бросить жемчуг\n"
                " -----------------\n"
                "F5, F6   : Авто-вращение сцены\n"
                "SPACE    : Пауза\n"
                "'0'      : Cбросить камеру\n"
                "ESCAPE   : Выход из игры\n"
            );
            */

            trayMgr->showOkDialog("Help",
                "'1'     : New Game - 1 player\n"
                "'2'     : New game - 2 players\n"
                "CURSOR  : LEFT/'A', RIGHT/'D'\n"
                "UP /'W' : Roll the pearls\n"
                "DOWN/'S': Throw the pearls\n"
                " -----------------\n"
                "F5, F6 : Auto-rotation of the scene\n"
                "SPACE   : Pause\n"
                "'0'     : Reset camera\n "
                "ESCAPE  : Exit the game \n"
            );
        }
    };


    ///------------------------------------------------------------------------|
    /// UI
    ///--------------------------------------------------------------------- UI:
    struct  UI : Glob
    {       UI()
            {   std::cout << "Объект UI создан!\n";
            }
           ~UI()
            {   trayMgr = nullptr;
            }
    
        OgreBites::TrayManager*       trayMgr;
        ScoreLabels               scoreLabels;
        MenuStart                   menuStart;
        Help                             help;

        bool keyPressed(const KeyboardEvent& evt)
        {   help.keyPressed(evt);
            return true;
        }

        void buttonHit(OgreBites::Button* button) 
        {   help.buttonHit(button);
        }

        void setup(OgreBites::TrayManager* tM)
        {   
            trayMgr = tM;

            OverlaySystem*  overlaySystem = ctx->getOverlaySystem();
            scnMgr->addRenderQueueListener(overlaySystem);

            ctx->addInputListener(trayMgr);
            trayMgr->hideCursor();

		/// score = std::make_unique<ScoreLabel>(trayMgr);

            scoreLabels.setup(trayMgr);
            menuStart  .setup(trayMgr);
            help       .setup(trayMgr);
        }
    };
}
 
 
#endif // UI_H
