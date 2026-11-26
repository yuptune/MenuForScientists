#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

static bool g_hasPlayedIntro = false;

class MyPopup : public geode::Popup {
protected:
	bool init(std::string const& value) {
		if (!Popup::init(260.f, 195.f))
			return false;

		this->setTitle("Menu For Scientists");

		auto toggleMenu1 = CCMenu::create();
		toggleMenu1->setContentSize({ 210.f, 30.f });
		toggleMenu1->setLayout(
			RowLayout::create()
				->setGap(10.f)
				->setAxisAlignment(AxisAlignment::Start)
		);

		auto toggle1 = CCMenuItemExt::createTogglerWithStandardSprites(
			0.75f,
			[](CCMenuItemToggler* sender) {
				bool isChecked = !sender->isToggled();
				Mod::get()->setSavedValue("bootycheeks", isChecked);
				log::info("toggled bootycheeks: {}", isChecked);
			}
		);
		toggle1->toggle(Mod::get()->getSavedValue<bool>("bootycheeks", false));

		auto label1 = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
		label1->setScale(0.45f);

		toggleMenu1->addChild(toggle1);
		toggleMenu1->addChild(label1);
		toggleMenu1->updateLayout();

		auto toggleMenu2 = CCMenu::create();
		toggleMenu2->setContentSize({ 210.f, 30.f });
		toggleMenu2->setLayout(
			RowLayout::create()
				->setGap(10.f)
				->setAxisAlignment(AxisAlignment::Start)
		);

		auto toggle2 = CCMenuItemExt::createTogglerWithStandardSprites(
			0.75f,
			[](CCMenuItemToggler* sender) {
				bool isChecked = !sender->isToggled();
				Mod::get()->setSavedValue("daze_intro", isChecked);
				log::info("toggled daze intro OK! {}", isChecked);
			}
		);
		toggle2->toggle(Mod::get()->getSavedValue<bool>("daze_intro", false));

		auto label2 = CCLabelBMFont::create("Daze Intro", "bigFont.fnt");
		label2->setScale(0.45f);

		toggleMenu2->addChild(toggle2);
		toggleMenu2->addChild(label2);
		toggleMenu2->updateLayout();

		auto container = CCNode::create();
		container->setContentSize({ 210.f, 75.f });
		container->setAnchorPoint({ 0.5f, 0.5f });
		container->ignoreAnchorPointForPosition(false);
		container->setLayout(
			ColumnLayout::create()
				->setGap(10.f)
				->setAxisReverse(true)
				->setAxisAlignment(AxisAlignment::Center)
		);
		container->addChild(toggleMenu1);
		container->addChild(toggleMenu2);
		container->updateLayout();

		m_mainLayer->addChildAtPosition(container, Anchor::Center, { 0.f, 12.f });

		auto restartMenu = CCMenu::create();
		restartMenu->setContentSize({ 210.f, 35.f });
		restartMenu->setLayout(RowLayout::create());

		auto restartSprite = ButtonSprite::create("kill hinds", "bigFont.fnt", "GJ_button_06.png");
		restartSprite->setScale(0.55f);

		auto restartBtn = CCMenuItemExt::createSpriteExtra(
			restartSprite,
			[](CCMenuItemSpriteExtra* sender) {
				geode::createQuickPopup(
					"Restart Game",
					"Are you sure you want to <cr>restart</c> Geometry Dash?",
					"Oh, Hell no!", "Restart the game :(",
					[](auto, bool btn2) {
						if (btn2) {
							geode::utils::game::restart(true);
						}
					}
				);
			}
		);

		restartMenu->addChild(restartBtn);
		restartMenu->updateLayout();

		m_mainLayer->addChildAtPosition(restartMenu, Anchor::Bottom, { 0.f, 24.f });

		return true;
	}

public:
	static MyPopup* create(std::string const& text) {
		auto ret = new MyPopup();
		if (ret->init(text)) {
			ret->autorelease();
			return ret;
		}

		delete ret;
		return nullptr;
	}
};

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		auto leftMenu = CCMenu::create();
		leftMenu->setID("leftmenu"_spr);
		leftMenu->setPosition({ 10.f, winSize.height / 2.f });
		leftMenu->setAnchorPoint({ 0.f, 0.5f });
		leftMenu->setContentSize({ 100.f, winSize.height });

		leftMenu->setLayout(
			ColumnLayout::create()
				->setGap(5.f)
				->setAxisAlignment(AxisAlignment::Center)
				->setCrossAxisAlignment(AxisAlignment::Start)
				->setCrossAxisLineAlignment(AxisAlignment::Start)
		);

		auto btnSprite = ButtonSprite::create("Open");
		btnSprite->setScale(0.6f);

		auto btn = CCMenuItemExt::createSpriteExtra(
			btnSprite,
			[](CCMenuItemSpriteExtra* sender) {
				MyPopup::create("Reveal the Hi")->show();
			}
		);

		leftMenu->addChild(btn);
		leftMenu->updateLayout();
		this->addChild(leftMenu);

		if (!g_hasPlayedIntro && Mod::get()->getSavedValue<bool>("daze_intro", false)) {
			g_hasPlayedIntro = true;

			if (auto video = CCSprite::create("daze_intro.gif"_spr)) {
				video->setID("dazeintro"_spr);
				video->setPosition(winSize / 2.f);

				auto vidSize = video->getContentSize();
				if (vidSize.width > 0 && vidSize.height > 0) {
					float scaleX = winSize.width / vidSize.width;
					float scaleY = winSize.height / vidSize.height;
					video->setScale(std::max(scaleX, scaleY));
				}

				auto introOverlay = CCMenu::create();
				introOverlay->setID("dazeintro-overlay"_spr);
				introOverlay->setPosition({ 0.f, 0.f });
				introOverlay->setContentSize(winSize);
				introOverlay->setZOrder(999);
				introOverlay->addChild(video);

				FMODAudioEngine::sharedEngine()->pauseAllMusic(true);
				FMODAudioEngine::sharedEngine()->playEffect("daze_intro.ogg"_spr);

				WeakRef<CCMenu> weakOverlay = introOverlay;
				auto finishIntro = [weakOverlay]() {
					if (auto overlay = weakOverlay.lock()) {
						FMODAudioEngine::sharedEngine()->stopAllEffects();
						FMODAudioEngine::sharedEngine()->resumeAllMusic();
						overlay->removeFromParent();
					}
				};

				auto skipArea = CCNode::create();
				skipArea->setContentSize(winSize);
				auto skipBtn = CCMenuItemExt::createSpriteExtra(
					skipArea,
					[finishIntro](CCMenuItemSpriteExtra*) {
						finishIntro();
					}
				);
				skipBtn->setPosition(winSize / 2.f);
				introOverlay->addChild(skipBtn);

				introOverlay->runAction(CCSequence::createWithTwoActions(
					CCDelayTime::create(5.0f),
					CallFuncExt::create(finishIntro)
				));

				this->addChild(introOverlay);
			} else {
				log::warn("bro the intro gif didnt friggn load");
			}
		}

		return true;
	}
};

class $modify(TurnItOffNow, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		auto btnSprite = ButtonSprite::create("Menu For Scientists");
		btnSprite->setScale(0.45f);

		auto btn = CCMenuItemExt::createSpriteExtra(
			btnSprite,
			[](CCMenuItemSpriteExtra* sender) {
				MyPopup::create("Reveal the Hi")->show();
			}
		);

		if (auto leftMenu = this->getChildByID("left-button-menu")) {
			leftMenu->addChild(btn);
			leftMenu->updateLayout();
		}
	}
};

class $modify(PlayDatShit, PlayLayer) {
	struct Fields {
		CCSprite* m_centerImage = nullptr;
	};

	void setupHasCompleted() {
		PlayLayer::setupHasCompleted();

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		if (auto sprite = CCSprite::create("hi.jpg"_spr)) {
			sprite->setID("theoverlay"_spr);
			sprite->setPosition(winSize / 2.f);
			sprite->setZOrder(100);
			sprite->setVisible(Mod::get()->getSavedValue<bool>("bootycheeks", false));

			this->addChild(sprite);
			m_fields->m_centerImage = sprite;
		}
	}

	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);

		if (m_fields->m_centerImage) {
			m_fields->m_centerImage->setVisible(
				Mod::get()->getSavedValue<bool>("bootycheeks", false)
			);
		}
	}
};
