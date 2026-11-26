#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class MyPopup : public geode::Popup {
protected:
	bool init(std::string const& value) {
		if (!Popup::init(240.f, 160.f))
			return false;

		this->setTitle("Menu For Scientists");

		auto toggleMenu = CCMenu::create();
		toggleMenu->setContentSize({ 200.f, 40.f });
		toggleMenu->setLayout(
			RowLayout::create()
				->setGap(10.f)
				->setAxisAlignment(AxisAlignment::Center)
		);

		auto toggle = CCMenuItemExt::createTogglerWithStandardSprites(
			0.8f,
			[](CCMenuItemToggler* sender) {
				bool isChecked = !sender->isToggled();
				Mod::get()->setSavedValue("bootycheeks", isChecked);
				log::info("toggled: {}", isChecked);
			}
		);

		toggle->toggle(Mod::get()->getSavedValue<bool>("bootycheeks", false));

		auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
		label->setScale(0.5f);

		toggleMenu->addChild(toggle);
		toggleMenu->addChild(label);
		toggleMenu->updateLayout();

		m_mainLayer->addChildAtPosition(toggleMenu, Anchor::Center);

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

		if (auto leftMenu = this->getChildByID("leftbtnmenu")) {
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
